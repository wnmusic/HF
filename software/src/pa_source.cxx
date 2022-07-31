#include "pa_source.h"
#include <exception>
#include <iostream>
#include <math.h>

pa_source::pa_source(const char* dev_name)
{

    PaError err;
    int numDevices, devNum = -1;
    err = Pa_Initialize();
    if( err != paNoError ){
        throw std::runtime_error("PA intialized failed");
    }

    bzero( &inputParameters, sizeof( inputParameters ) );
    inputParameters.channelCount = 1;       /* stereo input */
    inputParameters.sampleFormat = paFloat32; /* 32 bit floating point input */
    inputParameters.suggestedLatency = 0;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    numDevices = Pa_GetDeviceCount();
    
    for (int i=0; i<numDevices; i++){
        const   PaDeviceInfo *deviceInfo;
        deviceInfo = Pa_GetDeviceInfo( i );
        inputParameters.device = i;
        if(Pa_IsFormatSupported(&inputParameters, NULL, sample_rate) == 0){
            valid_dev_names.push_back(deviceInfo->name);
            valid_dev_number.push_back(i);
            if (dev_name && strcmp(deviceInfo->name, dev_name) == 0){
                devNum = i;
            }
        }
    }



    inputParameters.device = devNum >= 0 ? devNum : valid_dev_number[0];
    inputParameters.suggestedLatency =  Pa_GetDeviceInfo( inputParameters.device )->defaultLowOutputLatency;    
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,
              sample_rate,
              block_size*2,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              NULL,
              NULL);

    if( err != paNoError ){
        throw std::runtime_error("open PA stream error");
    }

    for(int i=0; i<test_table_len; i++ )
    {
        test_data.push_back(sinf( 1.0f*i/test_table_len * M_PI * 2. ));
    }    
    test_sample = 0;
    
    Pa_StartStream( stream );
}

pa_source::~pa_source()
{
    Pa_StopStream( stream );
    Pa_CloseStream( stream );
    Pa_Terminate();
}

void
pa_source::change_dev(const char* dev_name)
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


    inputParameters.device = devNum >= 0 ? devNum : Pa_GetDefaultInputDevice(); 
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,
              sample_rate,
              block_size*2,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              NULL,
              NULL);

    if( err != paNoError ){
        throw std::runtime_error("open PA stream error");
    }

    Pa_StartStream( stream );
}

unsigned pa_source::read(void*buf, int num_samples)
{
    int err =  Pa_ReadStream(stream, (float*)buf, num_samples);
    if (err ){
        std::cerr << Pa_GetErrorText(err) << "code: " << err << std::endl;
        if (paInputOverflow != err){
            return 0;
        }
    }
    if (b_test_tone){
        float *ptr = (float*)buf;
        for (int i=0; i<num_samples; i++){
            ptr[i] = test_data[test_sample++];
            if (test_sample >= test_table_len){
                test_sample = 0;
            }
        }
    }
    return num_samples;
}
