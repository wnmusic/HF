#include "demod_am.h"
#include "sdl_audio_impl.h"
#include <iostream>
#include <string.h>


int main(int argc, char* argv[])
{
    sdl_audio aud;
    float buf[128]; 

    while(1)
    {
        int num_samples = aud.read(buf, 128);
        if (num_samples == 128){
            aud.write(buf, 128);
        }
    }
}
