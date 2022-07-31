#include "demod_am.h"
#include <volk/volk.h>
#include <iostream>

demod_am::demod_am(int          mode
                  ,double       center_freq
                  ,double       bw
                  ,double       ssb_off
                  ,source_ifce *p_source
                  ,sink_ifce   *p_sink
                  ): m_source(p_source)
                   , m_sink(p_sink)
{
    static const int taps_per_poly = 13;
    
    input_rate     = m_source->get_source_rate();
    audio_rate     = m_sink->get_sink_rate();
    decimation     = (unsigned)floor(input_rate/audio_rate);

    block_size2    = (unsigned)floor(audio_rate * 0.01);
    block_size     = block_size2 * decimation;
    m_if_gain      = 1.0f;

    std::cerr << "decimaton: " <<decimation << " rf rate: " << input_rate <<" aud ratte: " << audio_rate
              << " conv rate: " << 1.0f * input_rate/decimation/audio_rate << std::endl;

    m_filter       = new fxlat_filter(decimation, taps_per_poly, center_freq, bw, block_size);
    m_resampler    = new resample(block_size2);
    p_scratch_buf[0] = new std::complex<float>[block_size];
    p_scratch_buf[1] = new std::complex<float>[block_size];

    p_if_buf = NULL;

    tune(mode, center_freq, ssb_off, bw);
}

demod_am::~demod_am()
{
    delete m_filter;
    delete m_resampler;
    delete[] p_scratch_buf[0];
    delete[] p_scratch_buf[1];
}

void demod_am::work(void)
{
    starting = true;
    static int proc_time = 0;
    float am_dc_level = 0.0f;
    float pwr = 0.0f;
                
    while(starting)
    {
        unsigned nin = m_source->read(p_scratch_buf[1], block_size);
        if (nin < block_size){
            std::cerr << "read size is less than block_size, semothing is wrong, exit";
            break;
        }
        if(p_if_buf){
            p_if_buf->write(p_scratch_buf[1], block_size, input_rate);
        }
        
        for (unsigned i=0 ;i<block_size; i++){
            p_scratch_buf[1][i] *= m_if_gain;
        }
        //down converter
        m_filter->process_c2c(p_scratch_buf[1], block_size, p_scratch_buf[0], block_size2);
        float *mag = (float*)p_scratch_buf[1];
        if (demod_mode == AM_DSB_MODE){
            volk_32fc_magnitude_32f(mag, p_scratch_buf[0], block_size2);
        }
        else if(demod_mode == AM_SSB_USB_MODE){
            for (unsigned i=0; i<block_size2; i++){
                mag[i] = p_scratch_buf[0][i].real();
            }
        }
        else if (demod_mode == AM_SSB_LSB_MODE){
             for (unsigned i=0; i<block_size2; i++){
                mag[i] = p_scratch_buf[0][i].imag();
            }
        }
        
        if (demod_mode == AM_DSB_MODE){
            pwr = 0.0f;
            for (unsigned i=0; i<block_size2; i++){
                am_dc_level = am_dc_level * 0.99f + 0.01f * mag[i];
                mag[i] -= am_dc_level;
                pwr += mag[i] * mag[i];
            }
        }else{
            pwr = 0.0f;
            for (unsigned i=0; i<block_size2; i++){
                pwr += mag[i] * mag[i];
            }
        }

        m_aud_amp = m_aud_amp * 0.9f + 0.1f * pwr;
        
        //insert to audio buffer
        float *aud = (float*)p_scratch_buf[0];
        unsigned nout = m_resampler->process(mag, block_size2,
                                            aud, block_size
                                            ,1.0f * input_rate/decimation/audio_rate);

        if (!m_sink->write(aud, nout)){
            std::cerr<<"x";
        }

    }
}


void demod_am::tune(int mode, float cf, float offset, float bw)
{
    if (demod_mode != mode || cf != center_freq || ssb_offset != offset)
    {
        if (mode == AM_DSB_MODE){
            m_filter->set_center(cf);
        }
        else if(mode == AM_SSB_USB_MODE){
            m_filter->set_center(cf+offset, -offset);
        }
        else if(mode == AM_SSB_LSB_MODE){
            m_filter->set_center(cf-offset, offset);
        }
    }

    if (bw != demod_bw)
    {
        m_filter->set_bandwidth(bw);
    }

    demod_bw = bw;
    center_freq = cf;
    demod_mode = mode;
    ssb_offset = offset;
}

