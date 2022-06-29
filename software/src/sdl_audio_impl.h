#ifndef MY_SDL_AUDIO_H_
#define MY_SDL_AUDIO_H_

#include "sink_ifce.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

class sdl_audio_sink : public sink_ifce
{
public:
    virtual double get_sample_rate(){ return 1.0*sample_rate;}
    virtual unsigned write(void* buf, int num_samples);
    
    ~sdl_audio_sink();
    sdl_audio_sink();

private:
    SDL_AudioDeviceID devid_out;
    static const unsigned sample_rate = 8000;
    unsigned proc_time;
};

#endif
