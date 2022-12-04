#pragma once
#include "sink_ifce.h"
#include "source_ifce.h"
#include "blkconv.h"
#include "resample.h"
#include "upsamp_fxlat_filter.h"
#include "debug_buf.h"
#include "common_defs.h"
#include "hilbert_xfrm.h"



class ssb_modulator
{

public:
    ssb_modulator(int          mode
                 ,double       center_freq
                 ,double       bw
                 ,double       ssb_off
                 ,source_ifce *p_source
                 ,sink_ifce   *p_sink
                 );
    
    ~ssb_modulator();
    
    static void start(void* ctx){
        ssb_modulator *obj = (ssb_modulator*)ctx;
        obj->work();
    }

    void stop(){
        starting = false;
    }

    void get_signal_power(float *sig_pwr_db){
        if(sig_pwr_db){
            *sig_pwr_db = 10.0f*log10f(m_sig_amp + 1e-12f);
        }
    }

    void set_if_gain(float if_gain){
        m_if_gain = powf(10.0f, if_gain*0.05f);
    }

    void set_fft_buf(debug_buf <std::complex <float> > *p_fft)
    {
        p_if_buf = p_fft;
    }

    void tune(int mode, float cf, float ssb_offset, float bw);

    void work();

private:
    std::complex<float> *p_scratch_buf[2];
    double audio_rate;
    double if_rate;
    unsigned interp;
    unsigned block_size, block_size2;
    double center_freq;
    double mod_bw;
    double ssb_offset;
    int mod_mode;
    bool starting;
    float m_sig_amp;
    float m_if_gain;
    float aud_conv_rate;
    source_ifce  *m_source;
    sink_ifce    *m_sink;
    unsigned delayline_len;
    
    upsamp_fxlat_filter* m_filter;
    resample* m_resampler;
    hilbert_xfrm  *m_hilbert;
    float *p_resampled;
    unsigned resample_buf_pos;

    debug_buf <std::complex<float> > *p_if_buf;
};
