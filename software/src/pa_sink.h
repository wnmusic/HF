#pragma once

#include "sink_ifce.h"
#include <portaudio.h>
#include "ringbuf.h"
#include <vector>
#include <stdio.h>

class pa_sink : public sink_ifce
{
public:
    virtual double get_sample_rate(){ return 1.0*sample_rate;}
    virtual unsigned write(void* buf, int num_samples);
    
    ~pa_sink();
    pa_sink(const char *dev_name);

    static int callback(const void *inputBuffer, void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void *userData );

    static int calc_mono_len(int frame_len) { return frame_len;}
    static int mono_to_stereo(void* dst, void* mono, int mono_len){
        float *out = (float*)dst;
        float *src = (float*)mono;
        for (unsigned i=0; i<mono_len; i++){
            out[2*i]   = src[i];
            out[2*i+1] = src[i];
        }
        return mono_len*sizeof(float)*2;
    }
    
    std::vector<const char*> & get_device_names(){return valid_dev_names;}

    void change_dev(const char* dev_name);
    int dev(void){
        for (int i=0; i<valid_dev_number.size(); i++){
            if (outputParameters.device == valid_dev_number[i]){
                return i;
            }
        }
        return 0;
    }
                    
private:        
    PaStream *stream;
    PaStreamParameters outputParameters;
    static const unsigned sample_rate = 8000;
    static const unsigned block_size = 80;
    ring_buffer<float> m_buf;
    bool init_fill;
    std::vector<const char*> valid_dev_names;
    std::vector<int> valid_dev_number;
};


