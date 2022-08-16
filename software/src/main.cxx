#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
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
#include "pa_sink.h"
#include "pa_source.h"
#include "spectrum.h"
#include "demod_am.h"
#include "ssb_modulator.h"
#include "uhd_fe.h"
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

static void get_view_offset(int mode, float bw, float* ploff, float *proff)
{
    float loff = 0.0f, roff = 0.0f;

    if( mode == AM_DSB_MODE){
        if (ploff) *ploff = bw/2;
        if (proff) *proff = bw/2;
    }
    else if (mode == AM_SSB_LSB_MODE){
        if (ploff) *ploff = bw;
        if (proff) *proff = 0;
    }
    else if (mode == AM_SSB_USB_MODE){
        if (ploff) *ploff = 0.0f;
        if (proff) *proff = bw;
    }

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
    double rx_sample_rate = 500000; //500KHz
    double tx_sample_rate = 1000000; //600KHz
    double rf_bandwidth   = 500000;
    double rf_band_center = 21.25e6;
    float curr_rf_gain = 20.0f;
    float curr_rf_freq = rf_band_center;
    float curr_wf_freq = 0.0f;
    float curr_tune_freq = 21.300e6;
    float curr_tune_bandwidth = 3000;    
    int curr_demod_mode = AM_SSB_LSB_MODE;
    int curr_audout_dev = -1;
    int curr_audin_dev = -1;
    float curr_tx_gain = 20.0f;
    bool test_tone_enable = false;
    
    float wf_sel_loff, wf_sel_roff;
    unsigned fft_size = 1024;
    const int spec_hist_len = 256;
    float *fft_mag = new float[fft_size];    
    debug_buf < std::complex <float> > if_buf(fft_size);    
    
    std::complex<float> *cplx_data = new std::complex<float>[fft_size];
    
    spectrum spec(fft_size, true);
    FILE *fp = fopen("uhd_hf.settings", "rb");
    char audin_name[256] = {0};
    char audout_name[256] = {0};
    if (fp){
        unsigned ret;
        ret = fscanf(fp, "band_center: %le, sample_rate: %le\n", &rf_band_center, &rx_sample_rate);
        ret = fscanf(fp, "rf: %e, bw: %e, mode: %d\n", &curr_rf_freq, &curr_tune_bandwidth, &curr_demod_mode);
        ret = fscanf(fp, "if gain: %f, tx gain: %f \n", &curr_rf_gain, &curr_tx_gain);
        ret = fscanf(fp, "audio input: %s\n", audin_name);
        ret = fscanf(fp, "audio output: %s\n", audout_name);
        fclose(fp);
    }


    pa_sink   audio_out(NULL);
    pa_source audio_in(NULL);
    std::vector<const char*> aud_in_devices = audio_in.get_device_names();
    std::vector<const char*> aud_out_devices = audio_out.get_device_names();
    
    for (int i=0; i<aud_in_devices.size(); i++){
        if (aud_in_devices[i] == audin_name){
            curr_audin_dev = i;
            break;
        }
    }
    if (curr_audin_dev >= 0 && curr_audin_dev != audio_in.dev()){
        audio_in.change_dev(audin_name);
    }else{
        curr_audin_dev = audio_in.dev();
    }

    for (int i=0; i<aud_out_devices.size(); i++){
        if (aud_out_devices[i] == audout_name){
            curr_audout_dev = i;
            break;
        }
    }
    if (curr_audout_dev >=0 && curr_audout_dev != audio_out.dev()){
        audio_out.change_dev(audout_name);
    }
    else{
        curr_audout_dev = audio_out.dev();
    }

    std::string device("addr=192.168.1.21");
    std::string subdev("A:A");
    std::string ref("internal");

    uhd_fe *uhd = new uhd_fe(device
                            ,subdev
                            ,ref
                            ,int(floor(rx_sample_rate*0.1))
                            ,rf_band_center
                            ,rx_sample_rate
                            ,tx_sample_rate
                            ,10.0f /* this is the gain for the hardware */
                            ,0.0f /* 0 dB , the max */
                            ,rf_bandwidth
                            ,rf_bandwidth
                            );
    rx_sample_rate = uhd->get_source_rate();
    tx_sample_rate = uhd->get_sink_rate();
    
    const double norm_rx_ssb_offset = 1500.0/rx_sample_rate * 2.0;
    double norm_rx_cf = (curr_rf_freq - rf_band_center)/rx_sample_rate*2.0;
    double norm_rx_bw = (curr_tune_bandwidth/2)/rx_sample_rate*2.0;
    demod_am demod(curr_demod_mode
                  ,norm_rx_cf
                  ,norm_rx_bw
                  ,norm_rx_ssb_offset
                  ,uhd
                  ,&audio_out);
    demod.set_fft_buf(&if_buf);
    demod.set_if_gain(powf(10.0f, curr_rf_gain/20.0f));

    const double norm_tx_ssb_offset = 1500.0/tx_sample_rate * 2.0;
    double norm_tx_cf = (curr_rf_freq - rf_band_center)/tx_sample_rate*2.0;
    double norm_tx_bw = (curr_tune_bandwidth/2)/tx_sample_rate*2.0;
    
    ssb_modulator mod(curr_demod_mode
                     ,norm_tx_cf
                     ,norm_tx_bw
                     ,norm_tx_ssb_offset
                     ,&audio_in
                     ,uhd
                     );
    mod.set_if_gain(powf(10.0f, curr_tx_gain/20.0f));
    
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // Main loop
    std::thread uhd_thread(uhd_fe::start, uhd);
    std::thread rx_thread(demod_am::start, &demod);
    std::thread tx_thread(ssb_modulator::start, &mod);    

    ImGui::WaterFall wf(fft_size, spec_hist_len);
    wf.init();
    wf.setFreqAndBandwidth(rf_band_center, rx_sample_rate);
    wf.registerFreqSelHandler(update_sel_freq, &curr_wf_freq);    
    wf.updateFreqSel(curr_rf_freq);
    get_view_offset(curr_demod_mode, curr_tune_bandwidth, &wf_sel_loff, &wf_sel_roff);
    wf.setSelFreqViewOffset(wf_sel_loff, wf_sel_roff);

    struct Funcs { static bool ItemGetter(void* data, int n, const char** out_str) {
        *out_str = ((std::vector<const char*>*)data)->at(n); return true;
    }
    };
    
    bool show_demo_window = false;
    while (!glfwWindowShouldClose(window))
    {
        unsigned nb_data, rate;
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        
        if (if_buf.read(cplx_data, nb_data, rate))        
        {
            ImGui::Begin("Spectrum");
            spec.process_complex(cplx_data, nb_data, false, fft_mag);

            wf.draw(fft_mag);
            ImGui::End();
        }

        {
            float band_center_MHz = rf_band_center/1e6;
            float gain = curr_rf_gain;
            float bw = curr_tune_bandwidth;
            float aud_vol;
            int mode = curr_demod_mode;
            ImGui::Begin("Control & Display", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::InputFloat("BandCenter", &band_center_MHz, 0.01f, 1.0f, "%.2f MHz");

            if (band_center_MHz*1e6 != rf_band_center){
                rf_band_center = band_center_MHz*1e6;                
                uhd->set_rf_freq(rf_band_center);
                wf.setFreqAndBandwidth(rf_band_center, rx_sample_rate);
            }

            demod.get_signal_power(&aud_vol);
            ImGui::VolumeMeter(aud_vol, 0.0, -60.0f, 0.0f, ImVec2(180, 20));
            ImGui::NewLine();
            ImGui::SliderFloat("Gain", &gain, 10.0f, 60.0f, "%.2f dB");
            if (gain != curr_rf_gain){
                curr_rf_gain = gain;
                demod.set_if_gain(powf(10.0f, curr_rf_gain/20.0f));
            }
            
            ImGui::NewLine();

            ImGui::SliderFloat("BandWidth", &bw, 2e3f, 10e3f, "%.3g");
            ImGui::RadioButton("DSB", &mode, AM_DSB_MODE); ImGui::SameLine();
            ImGui::RadioButton("USB", &mode, AM_SSB_USB_MODE); ImGui::SameLine();
            ImGui::RadioButton("LSB", &mode, AM_SSB_LSB_MODE);

            if (curr_wf_freq != curr_rf_freq)
            {
                curr_rf_freq = curr_wf_freq;
            }

            if (bw != curr_tune_bandwidth){
                curr_tune_bandwidth = bw;
            }
            
            if ((curr_rf_freq - rf_band_center)/rx_sample_rate*2.0 != norm_rx_cf
               || curr_tune_bandwidth/rx_sample_rate*2.0 != norm_rx_bw
               || mode != curr_demod_mode)
            {
                norm_rx_cf = (curr_rf_freq - rf_band_center)/rx_sample_rate*2.0;
                norm_rx_bw = (curr_tune_bandwidth/2)/rx_sample_rate*2.0;
                demod.tune(mode, norm_rx_cf, norm_rx_ssb_offset, norm_rx_bw);
                curr_demod_mode = mode;
                get_view_offset(curr_demod_mode, curr_tune_bandwidth, &wf_sel_loff, &wf_sel_roff);
                wf.setSelFreqViewOffset(wf_sel_loff, wf_sel_roff);


                norm_tx_cf = (curr_rf_freq - rf_band_center)/tx_sample_rate*2.0;
                norm_tx_bw = (curr_tune_bandwidth/2)/tx_sample_rate*2.0;
                mod.tune(mode, norm_tx_cf, norm_tx_ssb_offset, norm_tx_bw);
            }

            {
                int item = curr_audout_dev;
                ImGui::Combo("Audio Out Dev", &item, &Funcs::ItemGetter, &aud_out_devices, aud_out_devices.size());
                if (item != curr_audout_dev){
                    audio_out.change_dev(aud_out_devices[item]);
                    curr_audout_dev = audio_out.dev();
                    strcpy(audout_name, aud_out_devices[item]);
                }

            }

            ImGui::NewLine();
            {
                float vol;
                bool test = test_tone_enable;
                float tx_gain = curr_tx_gain; 
                mod.get_signal_power(&vol);
                ImGui::VolumeMeter(vol, 0, -40.0f, 20.0f, ImVec2(180, 20));
                ImGui::NewLine();
                ImGui::SliderFloat("Tx Gain", &tx_gain, 10.0f, 60.0f, "%.2f dB");
                if (tx_gain != curr_tx_gain){
                    mod.set_if_gain(tx_gain);
                    curr_tx_gain = tx_gain;
                }
                
                ImGui::NewLine();
                int item = curr_audin_dev;                
                ImGui::Combo("Audio In Dev", &item, &Funcs::ItemGetter, &aud_in_devices, aud_in_devices.size());
                if(item !=curr_audin_dev){
                    audio_in.change_dev(aud_in_devices[item]);
                    curr_audin_dev = audio_in.dev();
                    strcpy(audin_name,aud_in_devices[item]);
                }

                ImGui::Checkbox("Test Tone", &test);
                if (test != test_tone_enable)
                {
                    test_tone_enable = test;
                    audio_in.enable_test_tone(test_tone_enable);
                }

                ImGui::NewLine();
                if (ImGui::Button("PTT")){
                    uhd->set_ptt(true);
                }else{
                    uhd->set_ptt(false);
                }
            }

            
            ImGui::NewLine();
            if (ImGui::Button("Save")){
                FILE *fp = fopen("uhd_hf.settings", "w");
                fprintf(fp, "band_center: %.6le, sample_rate: %6le\n", rf_band_center, rx_sample_rate);                
                fprintf(fp, "rf: %.6le, bw: %.6le, mode: %d\n", curr_rf_freq, curr_tune_bandwidth, curr_demod_mode);
                fprintf(fp, "if gain: %f, tx gain: %f \n", curr_rf_gain, curr_tx_gain);
                fprintf(fp, "audio input: %s\n", audin_name);
                fprintf(fp, "audio output: %s\n", audout_name);
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
    
    uhd_fe::stop(uhd);    
    demod.stop();
    mod.stop();
    uhd_thread.join();
    rx_thread.join();
    tx_thread.join();
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    delete[] fft_mag;
    delete[] cplx_data;
    delete uhd;
    return 0;
}

