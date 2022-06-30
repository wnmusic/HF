#pragma once
#include "sink_ifce.h"
#include "source_ifce.h"
#include "polyphase.h"
#include "resample.h"
#include "fxlat_filter.h"
#include "debug_buf.h"
/* demodulate AM signal, input is a complex baseband, output with audio */
#define AM_DSB_MODE       (0)
#define AM_SSB_USB_MODE   (1)
#define AM_SSB_LSB_MODE   (2)

class demod_am
{

public:
    demod_am(int          mode
            ,double       center_freq
            ,double       bw
            ,double       ssb_off
            ,source_ifce *p_source
            ,sink_ifce   *p_sink
            );

    ~demod_am();

    static void start(void* ctx){
        demod_am *obj = (demod_am*)ctx;
        obj->work();
    }

    void stop(){
        starting = false;
    }

    void get_signal_power(float *aud_pwr_db){
        if(aud_pwr_db){
            *aud_pwr_db = 10.0f*log10f(m_aud_amp + 1e-12f);
        }
    }

    void set_if_gain(float if_gain){
        m_if_gain = if_gain;
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
    double input_rate;
    unsigned decimation;
    unsigned block_size, block_size2, aud_block_size;
    double center_freq;
    double demod_bw;
    double ssb_offset;
    int demod_mode;
    bool starting;
    float m_aud_amp;
    float m_if_gain;

    source_ifce  *m_source;
    sink_ifce    *m_sink;
    
    fxlat_filter* m_filter;
    resample* m_resampler;
    debug_buf <std::complex<float> > *p_if_buf;

};
