#include "sdl_audio_impl.h"
#include <stdexcept>

unsigned 
sdl_audio_sink::write(void* buf, int num_samples)
{
    SDL_QueueAudio(devid_out, buf, sizeof(float)*num_samples);
    if (proc_time > 2){
        SDL_PauseAudioDevice(devid_out, SDL_FALSE);
        }
    return num_samples;
}

    
sdl_audio_sink::~sdl_audio_sink()
{
    SDL_CloseAudio();
    SDL_Quit();
}

sdl_audio_sink::sdl_audio_sink()
{
    proc_time = 0;
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
        throw std::runtime_error("init SDL failed");
    }

    {
        int devcount = SDL_GetNumAudioDevices(SDL_FALSE);
        for (int i = 0; i < devcount; i++) {
            printf("Output  device #%d: '%s'\n", i, SDL_GetAudioDeviceName(i, SDL_FALSE));
        }
    }

    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = sample_rate;
    desiredSpec.format = AUDIO_F32SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 80;
    desiredSpec.callback = NULL;

    SDL_AudioSpec obtainedSpec;
    SDL_zero(obtainedSpec);

    const char* aud_devname = SDL_GetAudioDeviceName(0, 0);
    printf("Output  device: '%s'\n",aud_devname);
        
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
