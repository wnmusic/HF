#include "sdl_audio_impl.h"
#include <stdexcept>
#include <iostream>

unsigned 
sdl_audio::write(void* buf, int num_samples)
{
    if (play_pause)
    {
        play_pause = false;
        SDL_PauseAudioDevice(devid_out, 0);
    }
            
    SDL_QueueAudio(devid_out, buf, sizeof(float)*num_samples);

    return num_samples;
}

    
sdl_audio::~sdl_audio()
{
    delete[] rec_buf;
    SDL_CloseAudio();
    SDL_Quit();
}

void
sdl_audio::cap_callback(void* userdata, unsigned char* stream, int len)
{
    sdl_audio *p_aud = (sdl_audio*)userdata;
    p_aud->fill_audio(stream, len);
}

void
sdl_audio::fill_audio(unsigned char* stream, int len)
{
    std::lock_guard<std::mutex> guard(mutex);              
    memcpy(&rec_buf[rec_pos], stream, len);
    rec_pos += len/4;
    condvar.notify_one();
}

sdl_audio::sdl_audio()
{
    const char* aud_devname = SDL_GetAudioDeviceName(0, SDL_FALSE);
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
        throw std::runtime_error("init SDL failed");
    }
    {
        int devcount = SDL_GetNumAudioDevices(SDL_FALSE);
        for (int i = 0; i < devcount; i++) {
            if (strstr(SDL_GetAudioDeviceName(i, SDL_FALSE), "Babyface")){
                printf("Output  device #%d: '%s'\n", i, SDL_GetAudioDeviceName(i, SDL_FALSE));
                aud_devname = SDL_GetAudioDeviceName(i, SDL_FALSE);
                break;
            }
        }


        SDL_AudioSpec desiredSpec;
        SDL_zero(desiredSpec);
        desiredSpec.freq = sample_rate;
        desiredSpec.format = AUDIO_F32SYS;
        desiredSpec.channels = 1;
        desiredSpec.samples = (unsigned)floor(sample_rate*0.016);
        desiredSpec.callback = NULL;

        SDL_AudioSpec obtainedSpec;
        SDL_zero(obtainedSpec);

        devid_out = SDL_OpenAudioDevice(aud_devname, SDL_FALSE, &desiredSpec, &obtainedSpec, 0);
        if (!devid_out) {
            fprintf(stderr, "Couldn't open an audio device for playback: %s!\n", SDL_GetError());
            devid_out = 0;
        } else {
            fprintf(stdout, "Obtained spec for output device (SDL Id = %d):\n", devid_out);
            fprintf(stdout, "    - Sample rate:       %d (required: %d)\n", obtainedSpec.freq, desiredSpec.freq);
            fprintf(stdout, "    - Format:            %d (required: %d)\n", obtainedSpec.format, desiredSpec.format);
            fprintf(stdout, "    - Channels:          %d (required: %d)\n", obtainedSpec.channels, desiredSpec.channels);
            fprintf(stdout, "    - Samples per frame: %d (required: %d)\n", obtainedSpec.samples, desiredSpec.samples);
            
            if (obtainedSpec.format != desiredSpec.format ||
               obtainedSpec.channels != desiredSpec.channels ||
               obtainedSpec.samples != desiredSpec.samples) {
                SDL_CloseAudio();
                throw std::runtime_error("init SDL failed");
            }
        }

    }
    {
        int devcount = SDL_GetNumAudioDevices(SDL_TRUE);
        for (int i = 0; i < devcount; i++) {
            if (strstr(SDL_GetAudioDeviceName(i, SDL_TRUE), "Babyface")){
                printf("Input  device #%d: '%s'\n", i, SDL_GetAudioDeviceName(i, SDL_TRUE));
                aud_devname = SDL_GetAudioDeviceName(i, SDL_TRUE);
                break;
            }
        }

        SDL_AudioSpec desiredSpec;
        SDL_zero(desiredSpec);
        desiredSpec.freq = sample_rate;
        desiredSpec.format = AUDIO_F32SYS;
        desiredSpec.channels = 1;
        desiredSpec.samples = (unsigned)floor(sample_rate*0.016);
        desiredSpec.callback = sdl_audio::cap_callback;
        desiredSpec.userdata = this;


        SDL_AudioSpec obtainedSpec;
        SDL_zero(obtainedSpec);

        devid_in = SDL_OpenAudioDevice(aud_devname, SDL_TRUE, &desiredSpec, &obtainedSpec, 0);
        if (!devid_in) {
            fprintf(stderr, "Couldn't open an audio device for capture: %s!\n", SDL_GetError());
            devid_in = 0;
        } else {
            fprintf(stdout, "Obtained spec for input device (SDL Id = %d):\n", devid_in);
            fprintf(stdout, "    - Sample rate:       %d (required: %d)\n", obtainedSpec.freq, desiredSpec.freq);
            fprintf(stdout, "    - Format:            %d (required: %d)\n", obtainedSpec.format, desiredSpec.format);
            fprintf(stdout, "    - Channels:          %d (required: %d)\n", obtainedSpec.channels, desiredSpec.channels);
            fprintf(stdout, "    - Samples per frame: %d (required: %d)\n", obtainedSpec.samples, desiredSpec.samples);
            
            if (obtainedSpec.format != desiredSpec.format ||
               obtainedSpec.channels != desiredSpec.channels ||
               obtainedSpec.samples != desiredSpec.samples) {
                SDL_CloseAudio();
                throw std::runtime_error("init SDL failed");
            }
        }
    }


    test_sample = 0;

    rec_buf = new float[1024];
    rec_pos = 0;
    b_test_tone = false;

    rec_pause = true;
    play_pause = true;

}


unsigned sdl_audio::read(void* buf, int num_samples)
{
    if (rec_pause){
        rec_pause = false;
        SDL_PauseAudioDevice(devid_in, 0);
    }

    std::unique_lock<std::mutex> lock(mutex);
    while(rec_pos < num_samples){
        condvar.wait(lock);
    }

    memcpy(buf, rec_buf, num_samples*sizeof(float));
    rec_pos -= num_samples;
    if (rec_pos > 0){
        memcpy(rec_buf, &rec_buf[num_samples], rec_pos * sizeof(float));
    }

    if (b_test_tone)
    {
        float *ptr = (float*)buf;
        for (int i=0; i<num_samples; i++){
            ptr[i] = 0.5f*sinf(test_sample*400.0f/sample_rate *2.0f *M_PI)
                + 0.5f*sinf(test_sample * 1400.0f/sample_rate * 2.0f *M_PI) ;
            //ptr[i] = sinf(test_sample * 1400.0f/sample_rate * 2.0f *M_PI) ;
            test_sample++;
            test_sample = test_sample >= sample_rate ? 0 : test_sample;
        }
        return num_samples;
    }

    return num_samples;
}
  
