#include "ssb_modulator.h"
#include "rawfile_source.h"
#include "rawfile_sink.h"
#include <iostream>
#include <string.h>

int main(int argc, char* argv[])
{
    int mode = AM_SSB_USB_MODE;
    if (argc > 2){
        mode = atoi(argv[2]);
        std::cout << "Modulator mode: " << mode << std::endl;
    }

    if (argc > 1 && strstr(argv[1], "Audio_400Hz_tone.data"))
    {
        double cf = (21.1-21.25)*1e6/500e3*2.0;
        double bw = 3000/500e3*2.0;

        rawfile_sink<std::complex<float>> sink("mod_out.data", 500000);
        rawfile_source<float> source(argv[1], 8000);
        ssb_modulator sig(mode
                         ,cf
                         ,bw
                         ,&source
                         ,&sink
                         );
        sig.work();
    }

    return 0;
}
