#pragma once
/* demodulate AM signal, input is a complex baseband, output with audio */
#define AM_DSB_MODE       (0)
#define AM_SSB_USB_MODE   (1)
#define AM_SSB_LSB_MODE   (2)


static inline unsigned ceil_to_power2(unsigned i)
{
    unsigned n = 1;
    while(i){
        n = n << 1;
        i = i >> 1;
    }
    return n;
}
