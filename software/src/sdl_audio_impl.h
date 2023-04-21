#ifndef MY_SDL_AUDIO_H_
#define MY_SDL_AUDIO_H_

#include "sink_ifce.h"
#include "source_ifce.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <vector>
#include <mutex>
#include <condition_variable>

class sdl_audio : public sink_ifce, public source_ifce
{
public:
    virtual double get_sink_rate(){ return 1.0*sample_rate;}
    virtual double get_source_rate(){ return 1.0*sample_rate;}    
    virtual unsigned write(void* buf, int num_samples);
    virtual unsigned read(void* buf, int num_samples);
    
    ~sdl_audio();
    sdl_audio();

    void enable_test_tone(bool flag){
        b_test_tone = flag;
    }
    
    static void cap_callback(void* userdata, unsigned char* stream, int len);
    void fill_audio(unsigned char* stream, int len);
    
private:
    SDL_AudioDeviceID devid_out;
    SDL_AudioDeviceID devid_in;
    static const unsigned sample_rate = 8000;
    float *rec_buf;
    unsigned rec_pos;

    std::vector<float> test_data;
    bool b_test_tone;
    int test_sample;
    std::mutex mutex;
    std::condition_variable condvar;
    bool rec_pause;
    bool play_pause;
};


#endif
