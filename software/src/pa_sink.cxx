#include "pa_sink.h"
#include <exception>
#include <iostream>

pa_sink::pa_sink(const char* dev_name):m_buf(block_size*16)
{
    int numDevices, devNum = -1;    
    PaError err;
    err = Pa_Initialize();
    if( err != paNoError ){
        throw std::runtime_error("PA intialized failed");
    }
    
    bzero( &outputParameters, sizeof( outputParameters ) );    
    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.suggestedLatency = 0;
    
    numDevices = Pa_GetDeviceCount();
    for (int i=0; i<numDevices; i++){
        const   PaDeviceInfo *deviceInfo;
        deviceInfo = Pa_GetDeviceInfo( i );
        outputParameters.device = i;
        if(Pa_IsFormatSupported(NULL, &outputParameters, sample_rate) == 0){
            valid_dev_names.push_back(deviceInfo->name);
            valid_dev_number.push_back(i);
            if (dev_name && strcmp(deviceInfo->name, dev_name) == 0){
                devNum = i;
            }
        }
    }

    outputParameters.device = devNum >= 0 ? devNum : valid_dev_number[0];

    outputParameters.suggestedLatency =  Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;

    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              sample_rate,
              block_size,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              pa_sink::callback,
              this);

    if( err != paNoError ){
        throw std::runtime_error("open PA stream error");
    }

    Pa_StartStream( stream );
    init_fill = true;
}

pa_sink::~pa_sink()
{
    Pa_StopStream( stream );
    Pa_CloseStream( stream );
    Pa_Terminate();
}

void
pa_sink::change_dev(const char* dev_name)
{
    int err;
    Pa_StopStream(stream);
    Pa_CloseStream( stream );
    int devNum = -1;
    if (dev_name){
        for (int i=0; i<valid_dev_names.size(); i++){
            if (strcmp(valid_dev_names[i], dev_name) == 0){
                devNum = valid_dev_number[i];
                break;
            }
        }
    }

    outputParameters.device = devNum >= 0 ? devNum : Pa_GetDefaultOutputDevice(); 
    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              sample_rate,
              block_size,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              pa_sink::callback,
              this);

    if( err != paNoError ){
        throw std::runtime_error("open PA stream error");
    }

    Pa_StartStream( stream );
}


int 
pa_sink::callback(const void *inputBuffer, void *outputBuffer,
                 unsigned long framesPerBuffer,
                 const PaStreamCallbackTimeInfo* timeInfo,
                 PaStreamCallbackFlags statusFlags,
                 void *userData )
{
    pa_sink *obj  =(pa_sink*)userData;
    float *out = (float*)outputBuffer;
    (void) inputBuffer;

    if (obj->init_fill){
        if (obj->m_buf.get_count() < 8 * block_size){
            for (unsigned i=0; i<framesPerBuffer; i++){
                out[2*i]  = 0.0f;
                out[2*i+1] = 0.0f;
            }
        }
        else{
            obj->init_fill = false;
        }
    }
    else {
        if (!obj->m_buf.read(out, framesPerBuffer,
                            pa_sink::mono_to_stereo,
                            pa_sink::calc_mono_len
                            )){
            std::cerr<<"A";
            obj->init_fill = true;
        }
    }
    return 0;
}
                       
unsigned pa_sink::write(void*buf, int num_samples)
{
    return (unsigned) m_buf.write((float*)buf, num_samples);
}
