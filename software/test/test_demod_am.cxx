#include "demod_am.h"
#include "rawfile_source.h"
#include "rawfile_sink.h"
#include <iostream>
#include <string.h>

int main(int argc, char* argv[])
{
    int mode = AM_DSB_MODE;
    if (argc > 2){
        mode = atoi(argv[2]);
        std::cout << "Demod mode: " << mode << std::endl;
    }
    
    if (argc > 1 && strstr(argv[1], "AM_21.1M_sr500k_cf21.25M.data"))
    {
        double cf = (21.1-21.25)*1e6/500e3*2.0;
        double bw = 3000/500e3*2.0;
        double off = 1500/500e3*2.0;

        if (mode == AM_SSB_USB_MODE){
            cf -= off;
        }
        if (mode == AM_SSB_LSB_MODE){
            cf += off;
        }
        
        rawfile_sink<float> sink("demod_out.data", 0.016);
        complex_rawfile_source source(argv[1]);
        demod_am sig(mode
                    ,cf
                    ,bw
                    ,off
                    ,&source
                    ,&sink
                    );
        sig.work();
    }

    return 0;
}
