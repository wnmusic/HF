#include "waterfall.h"
#include <imgui.h>
#include <iostream>

float DEFAULT_COLOR_MAP[][3] = {
    { 0x00, 0x00, 0x20 },
    { 0x00, 0x00, 0x30 },
    { 0x00, 0x00, 0x50 },
    { 0x00, 0x00, 0x91 },
    { 0x1E, 0x90, 0xFF },
    { 0xFF, 0xFF, 0xFF },
    { 0xFF, 0xFF, 0x00 },
    { 0xFE, 0x6D, 0x16 },
    { 0xFF, 0x00, 0x00 },
    { 0xC6, 0x00, 0x00 },
    { 0x9F, 0x00, 0x00 },
    { 0x75, 0x00, 0x00 },
    { 0x4A, 0x00, 0x00 }
};

inline void printAndScale(double freq, char* buf) {
    double freqAbs = fabs(freq);
    if (freqAbs < 1000) {
        sprintf(buf, "%.6g", freq);
    }
    else if (freqAbs < 1000000) {
        sprintf(buf, "%.6lgK", freq / 1000.0);
    }
    else if (freqAbs < 1000000000) {
        sprintf(buf, "%.6lgM", freq / 1000000.0);
    }
    else if (freqAbs < 1000000000000) {
        sprintf(buf, "%.6lgG", freq / 1000000000.0);
    }
}
double freq_ranges[] = {
    1.0, 2.0, 2.5, 5.0,
    10.0, 20.0, 25.0, 50.0,
    100.0, 200.0, 250.0, 500.0,
    1000.0, 2000.0, 2500.0, 5000.0,
    10000.0, 20000.0, 25000.0, 50000.0,
    100000.0, 200000.0, 250000.0, 500000.0,
    1000000.0, 2000000.0, 2500000.0, 5000000.0,
    10000000.0, 20000000.0, 25000000.0, 50000000.0
};

inline double findBestRange(double bandwidth, int maxSteps) {
    for (int i = 0; i < 32; i++) {
        if (bandwidth / freq_ranges[i] < (double)maxSteps) {
            return freq_ranges[i];
        }
    }
    return 50000000.0;
}

static int cursor_to_idx(const ImVec2& pos, const ImRect& bb, int count) {
    const float t = ImClamp((pos.x - bb.Min.x) / (bb.Max.x - bb.Min.x), 0.0f, 0.9999f);
    const int v_idx = (int)(t*count);
    IM_ASSERT(v_idx >= 0 && v_idx < count);
    return v_idx;
}


#define CLAMP(x, lo, hi) ((x)<(lo) ? (lo) : (x) >(hi) ? (hi) : (x))
namespace ImGui {
    WaterFall::WaterFall(int fft_size, int waterfall_height) {
        fftMin = -70.0;
        fftMax = 0.0;
        waterfallMin = -70.0;
        waterfallMax = 0.0;
        fftHeight = 250;
        waterfallHeight = waterfall_height;
        dataWidth = fft_size;
        freqSel = 0.0f;
        m_freqhandler = NULL;
        waterfallFb = new uint32_t[fft_size*waterfallHeight];
        m_selfreq_view_loff = 0.0f;
        m_selfreq_view_roff = 0.0f;
        
        updatePallette(DEFAULT_COLOR_MAP, 13);
    }

    WaterFall::~WaterFall()
    {
        delete[] waterfallFb;
    }

    
    void WaterFall::updatePallette(float colors[][3], int colorCount) {
        for (int i = 0; i < WATERFALL_RESOLUTION; i++) {
            int lowerId = floorf(((float)i / (float)WATERFALL_RESOLUTION) * colorCount);
            int upperId = ceilf(((float)i / (float)WATERFALL_RESOLUTION) * colorCount);
            lowerId = CLAMP(lowerId, 0, colorCount - 1);
            upperId = CLAMP(upperId, 0, colorCount - 1);
            float ratio = (((float)i / (float)WATERFALL_RESOLUTION) * colorCount) - lowerId;
            float r = (colors[lowerId][0] * (1.0 - ratio)) + (colors[upperId][0] * (ratio));
            float g = (colors[lowerId][1] * (1.0 - ratio)) + (colors[upperId][1] * (ratio));
            float b = (colors[lowerId][2] * (1.0 - ratio)) + (colors[upperId][2] * (ratio));
            waterfallPallet[i] = ((uint32_t)255 << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r;
        }
    }
    
    void WaterFall::init()
    {
        glGenTextures(1, &textureId);
    }
    
    void WaterFall::setFreqAndBandwidth(float cf, float bw)
    {
        centerFreq = cf;
        viewBandwidth = bw;
        lowerFreq = (centerFreq ) - (viewBandwidth / 2.0);
        upperFreq = (centerFreq ) + (viewBandwidth / 2.0);

        if (m_freqhandler){
            m_freqhandler(cf, m_freqhandler_context);
        }
    }

    void WaterFall::drawFFT(float *latestFFT) {
        // Calculate scaling factor
        float startLine = floorf(fftMax / vRange) * vRange;
        float vertRange = fftMax - fftMin;
        float scaleFactor = fftHeight / vertRange;
        char buf[100];

        ImU32 trace = ImGui::GetColorU32(ImGuiCol_PlotLines);
        ImU32 shadow = ImGui::GetColorU32(ImGuiCol_PlotLines, 0.2);        
        ImU32 text = ImGui::GetColorU32(ImGuiCol_Text);
        float textVOffset = 10.0f;

        // Vertical scale
        for (float line = startLine; line > fftMin; line -= vRange) {
            float yPos = fftAreaMax.y - ((line - fftMin) * scaleFactor);
            window->DrawList->AddLine(ImVec2(fftAreaMin.x, roundf(yPos)),
                                      ImVec2(fftAreaMax.x, roundf(yPos)),
                                      IM_COL32(80, 80, 80, 255), 1.0f);
            sprintf(buf, "%d", (int)line);
            ImVec2 txtSz = ImGui::CalcTextSize(buf);
            window->DrawList->AddText(ImVec2(fftAreaMin.x - txtSz.x - textVOffset, roundf(yPos - (txtSz.y / 2.0))), text, buf);
        }

        // Horizontal scale
        double startFreq = ceilf(lowerFreq / range) * range;
        double horizScale = (double)dataWidth / viewBandwidth;
        float scaleVOfsset = 7 * 1.0f;
        for (double freq = startFreq; freq < upperFreq; freq += range) {
            double xPos = fftAreaMin.x + ((freq - lowerFreq) * horizScale);
            window->DrawList->AddLine(ImVec2(roundf(xPos), fftAreaMin.y + 1),
                                      ImVec2(roundf(xPos), fftAreaMax.y),
                                      IM_COL32(80, 80, 80, 255), 1.0f);
            window->DrawList->AddLine(ImVec2(roundf(xPos), fftAreaMax.y),
                                      ImVec2(roundf(xPos), fftAreaMax.y + scaleVOfsset),
                                      text, 1.0f);
            printAndScale(freq, buf);
            ImVec2 txtSz = ImGui::CalcTextSize(buf);
            window->DrawList->AddText(ImVec2(roundf(xPos - (txtSz.x / 2.0)), fftAreaMax.y + txtSz.y), text, buf);
        }

        if (freqSel >= lowerFreq && freqSel <=upperFreq)
        {
            double xPos = fftAreaMin.x + ((freqSel - m_selfreq_view_loff - lowerFreq) * horizScale);
            double xPos2 = fftAreaMin.x + ((freqSel + m_selfreq_view_roff - lowerFreq) * horizScale);            
            
            ImVec2 pos0 = ImVec2(roundf(xPos), fftAreaMin.y + 1);
            ImVec2 pos1 = ImVec2(roundf(xPos2), fftAreaMax.y);
            window->DrawList->AddRectFilled(pos0, pos1, IM_COL32(128, 128, 128, 32));
            window->DrawList->AddRect(pos0, pos1, IM_COL32(128, 128, 128, 128));
        };
            
        // Data
        if (latestFFT != NULL) {
            for (int i = 1; i < dataWidth; i++) {
                double aPos = fftAreaMax.y - ((latestFFT[i - 1] - fftMin) * scaleFactor);
                double bPos = fftAreaMax.y - ((latestFFT[i] - fftMin) * scaleFactor);
                aPos = CLAMP(aPos, fftAreaMin.y + 1, fftAreaMax.y);
                bPos = CLAMP(bPos, fftAreaMin.y + 1, fftAreaMax.y);
                window->DrawList->AddLine(ImVec2(fftAreaMin.x + i - 1, roundf(aPos)),
                                          ImVec2(fftAreaMin.x + i, roundf(bPos)), trace, 1.0);
                window->DrawList->AddLine(ImVec2(fftAreaMin.x + i, roundf(bPos)),
                                          ImVec2(fftAreaMin.x + i, fftAreaMax.y), shadow, 1.0);
            }
        }

        // X Axis
        window->DrawList->AddLine(ImVec2(fftAreaMin.x, fftAreaMax.y),
                                  ImVec2(fftAreaMax.x, fftAreaMax.y),
                                  text, 1.0f);
        // Y Axis
        window->DrawList->AddLine(ImVec2(fftAreaMin.x, fftAreaMin.y),
                                  ImVec2(fftAreaMin.x, fftAreaMax.y - 1),
                                  text, 1.0f);
    }

    void WaterFall::drawWaterfall(float* latestFFT) {

        memmove(&waterfallFb[dataWidth], waterfallFb, dataWidth * (waterfallHeight - 1) * sizeof(uint32_t));
        float pixel;
        float dataRange = waterfallMax - waterfallMin;
        for (int j = 0; j < dataWidth; j++) {
            pixel = (CLAMP(latestFFT[j], waterfallMin, waterfallMax) - waterfallMin) / dataRange;
            int id = (int)(pixel * (WATERFALL_RESOLUTION - 1));
            waterfallFb[j] = waterfallPallet[id];
        }

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dataWidth, waterfallHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (uint8_t*)waterfallFb);
        window->DrawList->AddImage((void*)(intptr_t)textureId, wfMin, wfMax);
    }

    void WaterFall::draw(float *fft_mag)
    {
        window = GetCurrentWindow();
        const int w_padding = 70.0;
        const int h_padding = 100.0;
        const int dragHeight = 40;
        int freqDrag = (int)freqSel;
        int freqDragMin = (int)(freqSel - 0.5*range);
        int freqDragMax = (int)(freqSel + 0.5*range);
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImRect frame_bb(
            window->DC.CursorPos,
            window->DC.CursorPos + ImVec2(dataWidth+w_padding, fftHeight+waterfallHeight+h_padding)
        );
        
        RenderFrame(
            frame_bb.Min,
            frame_bb.Max,
            GetColorU32(ImGuiCol_FrameBg),
            true,
            style.FrameRounding);
        
        widgetPos = frame_bb.Min + style.FramePadding;
        widgetEndPos = frame_bb.Max - style.FramePadding;
        widgetSize = ImVec2(widgetEndPos.x - widgetPos.x, widgetEndPos.y - widgetPos.y);

        fftAreaMin = ImVec2(widgetPos.x + (50.0f * 1.0f), widgetPos.y + (9.0f * 1.0f));
        fftAreaMax = ImVec2(fftAreaMin.x + dataWidth, fftAreaMin.y + fftHeight + 1);

        freqAreaMin = ImVec2(fftAreaMin.x, fftAreaMax.y + 1);
        freqAreaMax = ImVec2(fftAreaMax.x, fftAreaMax.y + (40.0f * 1.0f));

        wfMin = ImVec2(fftAreaMin.x, freqAreaMax.y + dragHeight + 1);
        wfMax = ImVec2(fftAreaMin.x + dataWidth, wfMin.y + waterfallHeight);


        maxHSteps = dataWidth / (ImGui::CalcTextSize("000.000").x + 10);
        maxVSteps = fftHeight / (ImGui::CalcTextSize("000.000").y);

        range = findBestRange(viewBandwidth, maxHSteps);
        vRange = findBestRange(fftMax - fftMin, maxVSteps);

        drawFFT(fft_mag);
        window->DC.CursorPos = ImVec2(freqAreaMin.x + 20, freqAreaMax.y+1);
        ImGui::DragInt("Tune", &freqDrag, 10, freqDragMin, freqDragMax, "%d Hz");
        if( freqDrag*1.0 != freqSel){
            freqSel = 1.0*freqDrag;
            if (m_freqhandler){
                m_freqhandler(freqSel, m_freqhandler_context);
            }
        }
        drawWaterfall(fft_mag);
        {
            bool mouseHovered, mouseHeld;
            ImVec2 mousePos = ImGui::GetMousePos();
            bool mouseClicked = ImGui::ButtonBehavior(ImRect(fftAreaMin, wfMax),
                                                     GetID("WaterfallID"),
                                                     &mouseHovered, &mouseHeld,
                                                     ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_PressedOnClick);

            
            // handle mouse click
            ImRect fftArea(fftAreaMin, fftAreaMax);
            ImRect freqArea(freqAreaMin, freqAreaMax);
            
            if (mouseClicked && 
               fftArea.Contains(mousePos)){
                const int v_idx = cursor_to_idx(g.IO.MousePos, fftArea, dataWidth);
                freqSel = lowerFreq + 1.0f*v_idx/dataWidth*viewBandwidth;
                if (m_freqhandler){
                    m_freqhandler(freqSel, m_freqhandler_context);
                }
                if (m_freqhandler){
                    m_freqhandler(freqSel, m_freqhandler_context);
                }
            }

        }
    }
}
