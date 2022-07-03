#include "pa_sink.h"
#include <exception>
#include <iostream>

pa_sink::pa_sink():m_buf(block_size*16)
{
    PaStreamParameters outputParameters;
    PaError err;
    err = Pa_Initialize();
    if( err != paNoError ){
        throw std::runtime_error("PA intialized failed");
    }

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
        throw std::runtime_error("No default output device.\n");
    }
    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    
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
