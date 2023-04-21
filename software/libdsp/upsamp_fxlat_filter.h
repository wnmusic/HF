#pragma once
#include <volk/volk.h>
#include "fir_gen.h"
/* this block performs  upsampling & filter, and then shift the signal to
 * center
 *
 * for SSB application, the center frequency of the SSB is off by offset
 * bandwidth is the lowpass filter cutoff freqency
 *
 */
 
class upsamp_fxlat_filter
{
public:
    upsamp_fxlat_filter(int interp, int n_taps_per_poly, float center, float bandwidth, int blksize);    
    ~upsamp_fxlat_filter();
    
    void set_center(float center);
    void set_bandwidth(float bw);
    /* return the output len which is blksize/dec  */
    int process_c2c(std::complex<float>* in, int n_in, std::complex<float>* out, int out_len);
    
 private:
    fir_lowpass    *p_lpf;
    lv_32fc_t     **m_poly_taps;
    int             m_n_taps;
    int             m_ntaps_per_poly;
    lv_32fc_t      *m_hist_c;
    lv_32fc_t      *m_scratch;
    int             m_poly;
    int             m_blksize;
    float           m_center;
    lv_32fc_t       m_phase;
    lv_32fc_t       m_phase_inc;
    float           m_bandwidth;
};



