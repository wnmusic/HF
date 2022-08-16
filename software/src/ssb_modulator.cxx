#include "ssb_modulator.h"
#include <iostream>

ssb_modulator::ssb_modulator(int          mode
                            ,double       center_freq
                            ,double       bw
                            ,double       ssb_off
                            ,source_ifce *p_source
                            ,sink_ifce   *p_sink
                            ) : m_source(p_source)
                              , m_sink(p_sink)
{
    static const unsigned taps_per_poly = 13;
    if_rate = m_sink->get_sink_rate();
    audio_rate = m_source->get_source_rate();

    interp = (unsigned)floor(if_rate/audio_rate);
    block_size2 = (unsigned)floor(audio_rate * 0.016);
    block_size     = block_size2 * interp;
    m_if_gain  = 1.0f;
    aud_conv_rate = 1.0f * audio_rate*interp/if_rate;
    std::cout << "uprate: " <<interp << " if rate: " << if_rate <<" aud ratte: " << audio_rate  << " conv rate: " << aud_conv_rate << std::endl;

    m_resampler = new resample(block_size2);
    m_filter    = new upsamp_fxlat_filter(interp, taps_per_poly, center_freq, bw, block_size2);
    m_hilbert   = new hilbert_xfrm(block_size2);
    
    p_scratch_buf[0] = new std::complex<float>[block_size];
    p_scratch_buf[1] = new std::complex<float>[block_size];
    p_resampled = new float[2*block_size2];
    p_if_buf         = NULL;
    

    resample_buf_pos = 0;
    tune(mode, center_freq, ssb_off, bw);
}

ssb_modulator::~ssb_modulator()
{
    delete m_resampler;
    delete m_filter;
    delete m_hilbert;
    delete[] p_scratch_buf[0];
    delete[] p_scratch_buf[1];
    delete[] p_resampled;
}

void ssb_modulator::work(void)
{
    starting = true;

    while(starting){
        
        unsigned n_aud_samples = (unsigned)ceilf(aud_conv_rate * (block_size2 - resample_buf_pos));

        if(m_source->read(p_scratch_buf[0], n_aud_samples) != n_aud_samples){
            break;
        }
        float *p_aud = (float*)p_scratch_buf[0];
            
        for(int i=0; i<n_aud_samples; i++){
            m_sig_amp = m_sig_amp * 0.9f + 0.1f * p_aud[i] *p_aud[i];
            p_aud[i] *= m_if_gain;
        }

        int nout = m_resampler->process(p_aud, n_aud_samples
                                       ,&p_resampled[resample_buf_pos]
                                       ,2*block_size2 - resample_buf_pos
                                       ,aud_conv_rate);
            

        resample_buf_pos = nout + resample_buf_pos - block_size2;
        m_hilbert->process(p_resampled, block_size2, p_scratch_buf[0], block_size2, mod_mode==AM_SSB_USB_MODE);

        m_filter->process_c2c(p_scratch_buf[0], block_size2, p_scratch_buf[1], block_size);

        if(p_if_buf){
            p_if_buf->write(p_scratch_buf[1], block_size, if_rate);
        }

        if (m_sink->write(p_scratch_buf[1], block_size) != block_size){
            std::cerr<< "o";
        }
    }
}
    
void ssb_modulator::tune(int mode, float cf, float offset, float bw)
{
    if (mod_mode != mode || cf != center_freq || ssb_offset != offset)
    {
        if(mode == AM_SSB_USB_MODE){
            m_filter->set_center(cf+offset, -offset);
        }
        else if(mode == AM_SSB_LSB_MODE){
            m_filter->set_center(cf-offset, offset);
        }
    }

    if (bw != mod_bw)
    {
        m_filter->set_bandwidth(bw);
    }

    mod_bw = bw;
    center_freq = cf;
    mod_mode = mode;
    ssb_offset = offset;
}


