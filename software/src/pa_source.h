#pragma once

#include "source_ifce.h"
#include <portaudio.h>
#include "ringbuf.h"
#include <vector> 

class pa_source : public source_ifce
{
public:
    virtual double get_sample_rate(){ return 1.0*sample_rate;}
    virtual unsigned read(void* buf, int num_samples);
    
    ~pa_source();
    pa_source(const char* dev_name);

    std::vector<const char*> & get_device_names(){return valid_dev_names;}    
    void change_dev(const char* dev_name);
    int dev(void){
        for (int i=0; i<valid_dev_number.size(); i++){
            if (inputParameters.device == valid_dev_number[i]){
                return i;
            }
        }
        return 0;
    }

private:        
    PaStream *stream;
    PaStreamParameters inputParameters;    
    static const unsigned sample_rate = 8000;
    static const unsigned block_size = 80;
    std::vector<const char*> valid_dev_names;
    std::vector<int> valid_dev_number;
    
};


