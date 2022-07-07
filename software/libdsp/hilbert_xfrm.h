#pragma once
#include "fir_gen.h"
#include "blkconv.h"
#include <complex>

class hilbert_xfrm{
public:
    static inline unsigned ceil_to_power2(unsigned i){
        unsigned n = 1;
        unsigned i0 = i;
        while(i){
            n = n << 1;
            i = i >> 1;
        }
        return n;
    }
    
    hilbert_xfrm(unsigned blocksize);
    ~hilbert_xfrm();
    int process(float *in, int n_in, std::complex<float> *out, int out_len, int b_usb);
private:
    unsigned m_blksize;
    unsigned delayline_len;
    blkconv *m_conv;
    float *p_delayline;
};
