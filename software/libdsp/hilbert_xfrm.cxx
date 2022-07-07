#include "hilbert_xfrm.h"
#include <iostream>

hilbert_xfrm::hilbert_xfrm(unsigned blocksize)
        : m_blksize(blocksize)
{
        
    unsigned n_fft = ceil_to_power2(m_blksize);
    unsigned n_hilbert_taps = n_fft - m_blksize + 1;
    while (n_hilbert_taps < 64){
        n_fft *= 2;
        n_hilbert_taps = n_fft - m_blksize + 1;
    }
    
    hilbert_transform hxfm(n_hilbert_taps);
    delayline_len = hxfm.grp_delay();
        
    std::cout << "blkconv fft: "<<n_fft << " Hilbert taps: " << n_hilbert_taps <<" delay: " << delayline_len << std::endl;
    m_conv   = new blkconv(hxfm.taps(), n_hilbert_taps, n_fft);
        
    p_delayline = new float[delayline_len + m_blksize];            
    memset(p_delayline, 0, sizeof(float)*(delayline_len + m_blksize));
}

hilbert_xfrm::~hilbert_xfrm()
{
    delete m_conv;
    delete[] p_delayline;
}


int hilbert_xfrm::process(float *in, int n_in, std::complex<float> *out, int out_len, int b_usb)
{
        
    float *imag_buf = m_conv->get_process_buf();        
    memcpy(&p_delayline[delayline_len], in, sizeof(float)*n_in);
    memcpy(imag_buf, in, sizeof(float)*n_in);
    m_conv->process();
    if (b_usb){
        for (int i=0; i<m_blksize; i++){
            out[i] = std::complex<float>(p_delayline[i], imag_buf[i]);
        }
    }else{
        for (int i=0; i<m_blksize; i++){
            out[i] = std::complex<float>(p_delayline[i], -imag_buf[i]);
        }

    }
    memcpy(p_delayline, &p_delayline[m_blksize], sizeof(float)*delayline_len);
    return m_blksize;
}
