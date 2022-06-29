#pragma once
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <imgui_internal.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers


#define WATERFALL_RESOLUTION 65535

namespace ImGui {
    class WaterFall{
    public:
        WaterFall(int fft_size, int waterfall_height);
        ~WaterFall();
        void init();
        void draw(float* fft_mag);
        void updateFreqSel(float sel_freq){
            freqSel = sel_freq;
        }
        void setFreqAndBandwidth(float cf, float bw);
        
        void registerFreqSelHandler(void (*handler)(double freq, void*), void*ctx){
            m_freqhandler = handler;
            m_freqhandler_context = ctx;
        }
                
        ImVec2 fftAreaMin;
        ImVec2 fftAreaMax;
        ImVec2 freqAreaMin;
        ImVec2 freqAreaMax;
        ImVec2 wfMin;
        ImVec2 wfMax;
        ImVec2 widgetPos;
        ImVec2 widgetEndPos;
        ImVec2 widgetSize;
        
    private:
        void updatePallette(float colors[][3], int colorCount);        
        void drawFFT(float*);
        void drawWaterfall(float*);
        ImVec2 mouseDownPos;
        
        ImGuiWindow* window;
        GLuint textureId;
        
        uint32_t waterfallPallet[WATERFALL_RESOLUTION];
        float centerFreq, lowerFreq, upperFreq, viewBandwidth;

        float fftMin;
        float fftMax;
        float waterfallMin;
        float waterfallMax;
        float vRange;
        float range;
        float maxHSteps;
        float maxVSteps;
        double freqSel;
        int  fftHeight;
        int waterfallHeight;
        int dataWidth;
        uint32_t* waterfallFb;
        int fftLines;

        void (*m_freqhandler)(double freq, void*);
        void *m_freqhandler_context;

        
    };
}
