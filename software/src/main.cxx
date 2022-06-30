#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include "imgui_plot.h"
#include <thread>
#include <math.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/utils/thread.hpp>
#include "sdl_audio_impl.h"
#include "spectrum.h"
#include "demod_am.h"
#include "uhd_rx.h"
#include "waterfall.h"
#include "volume_meter.h"
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void update_sel_freq(double freq, void*ctx)
{
    float *p_freq = (float*)ctx;
    *p_freq = (float)freq;
}
 
int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 960, "UHD HF App", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    double rf_sample_rate = 500000; //500KHz
    double rf_band_center = 21.25e6;
    float curr_rf_gain = 20.0f;

    float curr_rf_freq = rf_band_center;
    float curr_wf_freq = 0;
    float curr_tune_freq = 21.300e6;
    float curr_tune_bandwidth = 3000;    
    int curr_demod_mode = AM_SSB_LSB_MODE;
    
    unsigned fft_size = 1024;
    const int spec_hist_len = 256;
    float *fft_mag = new float[fft_size];    
    debug_buf < std::complex <float> > if_buf(fft_size);    
    
    std::complex<float> *cplx_data = new std::complex<float>[fft_size];
    
    spectrum spec(fft_size, true);
    FILE *fp = fopen("uhd_hf.settings", "rb");
    if (fp){
        unsigned ret;
        ret = fscanf(fp, "band_center: %le, sample_rate: %le\n", &rf_band_center, &rf_sample_rate);
        ret = fscanf(fp, "rf: %e, bw: %e, mode: %d\n", &curr_rf_freq, &curr_tune_bandwidth, &curr_demod_mode);
        ret = fscanf(fp, "if gain: %f\n", &curr_rf_gain);
        fclose(fp);
    }
    
    std::string device("addr=192.168.1.21");
    std::string subdev("A:A");
    std::string ref("internal");

    uhd_rx *rx = new uhd_rx(device
                           ,subdev
                           ,ref
                           ,fft_size
                           ,rf_sample_rate
                           ,rf_band_center
                           ,10.0f /* this is the gain for the hardware */
                           ,rf_sample_rate
                           );
    rf_sample_rate = rx->get_sample_rate();
    
    sink_ifce *audio = new sdl_audio_sink();
    const double norm_ssb_offset = 3000.0/rf_sample_rate * 2.0;
    double norm_cf = (curr_rf_freq - rf_band_center)/rf_sample_rate*2.0;
    double norm_bw = curr_tune_bandwidth/rf_sample_rate*2.0;
    
    demod_am sig_chain(curr_demod_mode
                      ,norm_cf
                      ,norm_bw
                      ,norm_ssb_offset
                      ,rx
                      ,audio);

    sig_chain.set_fft_buf(&if_buf);
    sig_chain.set_if_gain(powf(10.0f, curr_rf_gain/20.0f));
    
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // Main loop
    std::thread rx_thread(uhd_rx::start, rx);
    std::thread proc_thread(demod_am::start, &sig_chain);

    ImGui::WaterFall wf(fft_size, spec_hist_len);
    wf.init();
    wf.setFreqAndBandwidth(rf_band_center, rf_sample_rate);
    wf.updateFreqSel(curr_rf_freq);
    wf.registerFreqSelHandler(update_sel_freq, &curr_wf_freq);
    
    while (!glfwWindowShouldClose(window))
    {
        unsigned nb_data, rate;
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (if_buf.read(cplx_data, nb_data, rate))        
        {
            ImGui::Begin("Spectrum");
            spec.process_complex(cplx_data, nb_data, false, fft_mag);

            wf.draw(fft_mag);
            ImGui::End();
        }

        {
            float band_center_MHz = rf_band_center/1e6;
            float rf_freq = curr_wf_freq;
            float gain = curr_rf_gain;
            float bw = curr_tune_bandwidth;
            float aud_vol;
            int mode = curr_demod_mode;
            ImGui::Begin("Control & Display", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::InputFloat("BandCenter", &band_center_MHz, 0.01f, 1.0f, "%.2f MHz");

            if (band_center_MHz*1e6 != rf_band_center){
                rf_band_center = band_center_MHz*1e6;                
                rx->set_rf_freq(rf_band_center);
                wf.setFreqAndBandwidth(rf_band_center, rf_sample_rate);
            }

            sig_chain.get_signal_power(&aud_vol);
            ImGui::VolumeMeter(aud_vol, 0.0, -60.0f, 0.0f, ImVec2(180, 20));
            ImGui::NewLine();
            ImGui::SliderFloat("Gain", &gain, 10.0f, 60.0f, "%.2f dB");
            if (gain != curr_rf_gain){
                curr_rf_gain = gain;
                sig_chain.set_if_gain(powf(10.0f, curr_rf_gain/20.0f));
            }
            
            ImGui::NewLine();
            ImGui::InputFloat("Freq", &rf_freq, 10.0f, 1000.0f, "%.6e Hz");
            ImGui::SliderFloat("BandWidth", &bw, 2e3f, 10e3f, "%.3g");
            ImGui::RadioButton("DSB", &mode, AM_DSB_MODE); ImGui::SameLine();
            ImGui::RadioButton("USB", &mode, AM_SSB_USB_MODE); ImGui::SameLine();
            ImGui::RadioButton("LSB", &mode, AM_SSB_LSB_MODE);

            if (rf_freq != curr_wf_freq)
            {
                wf.updateFreqSel(rf_freq);
                curr_wf_freq = rf_freq;
            }

            if (bw != curr_tune_bandwidth){
                curr_tune_bandwidth = bw;
            }
            
            if ((curr_rf_freq - rf_band_center)/rf_sample_rate*2.0 != norm_cf
               || curr_tune_bandwidth/rf_sample_rate*2.0 != norm_bw
               || mode != curr_demod_mode)
            {
                norm_cf = (curr_rf_freq - rf_band_center)/rf_sample_rate*2.0;
                norm_bw = curr_tune_bandwidth/rf_sample_rate*2.0;
                sig_chain.tune(mode
                              ,norm_cf
                              ,norm_ssb_offset
                              ,norm_bw
                              );
                curr_demod_mode = mode;
            }

            ImGui::SameLine();
            if (ImGui::Button("Save")){
                FILE *fp = fopen("uhd_hf.settings", "w");
                fprintf(fp, "band_center: %.6le, sample_rate: %6le\n", rf_band_center, rf_sample_rate);                
                fprintf(fp, "rf: %.6le, bw: %.6le, mode: %d\n", curr_rf_freq, curr_tune_bandwidth, curr_demod_mode);
                fprintf(fp, "if gain: %.6f\n", curr_rf_gain);
                fclose(fp);
            }
            ImGui::End();
        }        

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    
    uhd_rx::stop(rx);    
    sig_chain.stop();

    rx_thread.join();
    proc_thread.join();
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    delete[] fft_mag;
    delete[] cplx_data;
    delete rx;
    delete audio;
    return 0;
}

