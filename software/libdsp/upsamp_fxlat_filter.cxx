#include "upsamp_fxlat_filter.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "fir_gen.h"


upsamp_fxlat_filter::upsamp_fxlat_filter(int interp, int n_taps_per_poly, float cf, float bandwidth, int blksize):m_poly(interp)
                                                                                                                 ,m_ntaps_per_poly(n_taps_per_poly)
                                                                                                                 ,m_blksize(blksize)
                                                                                                                 ,m_offset(0.0f)
                                                                                                                  
{
    p_lpf = NULL;
    m_poly_taps = new lv_32fc_t*[interp];
    for (int i=0; i<m_poly; i++){
        m_poly_taps[i] = (lv_32fc_t*) volk_malloc(m_ntaps_per_poly*sizeof(lv_32fc_t), volk_get_alignment());
    }
    m_n_taps = m_poly * m_ntaps_per_poly;
    
    m_hist_c = (lv_32fc_t *)volk_malloc(sizeof(lv_32fc_t)*(m_ntaps_per_poly + blksize - 1), volk_get_alignment());
    memset(m_hist_c, 0, sizeof(lv_32fc_t)*(m_ntaps_per_poly + blksize - 1));

    m_scratch = (lv_32fc_t*)volk_malloc(m_blksize * sizeof(lv_32fc_t), volk_get_alignment());

    m_phase = lv_cmake(1.0f, 0.0f);
    set_bandwidth(bandwidth);
    set_center(cf, m_offset);
}

void
upsamp_fxlat_filter::set_bandwidth(float bw)
{
    if (bw > 1.0f/m_poly){
        bw = 1.0f/m_poly;
    }

    if (p_lpf){
        delete p_lpf;
    }
    p_lpf = new fir_lowpass(m_n_taps, HAMMING_WINDOW, bw);
    set_center(m_center, m_offset);
    m_bandwidth = bw;
}

upsamp_fxlat_filter::~upsamp_fxlat_filter()
{
    volk_free(m_hist_c);
    for (int i = 0; i<m_poly; i++){
        volk_free(m_poly_taps[i]);
    }
    volk_free(m_scratch);
    delete[] m_poly_taps;
}

/* to reduce compute sin and cos for the high sampling rate, we ues the same 
 * trick as in gnuradio
 * we first shift to the center freqency plus some offset (For ssb), so that
 * after interpolation, it is right at the position, (m_center)
 * 
 * the filter bandpass at center frequency (center of SSB) ,
 * the input are normalized to the upsampled nyquist rate. 
 */
void upsamp_fxlat_filter::set_center(float center, float offset)
{
    float *m_float_taps = p_lpf->taps();
    /* center is baesd on the sample rate before interp */
    m_center = center > 1.0f ? 1.0f : center < -1.0f ? -1.0f : center;
    m_offset = offset > 1.0f ? 1.0f : offset < -1.0f ? -1.0f : offset;
    /* notice that m_taps is the filipped version of taps */
    for (unsigned i = m_n_taps -1, j=0; j<m_n_taps; i--,j++){
        m_poly_taps[i%m_poly][i/m_poly] = m_float_taps[j] * lv_cmake(cosf(j*m_center*M_PI), sinf(j*m_center*M_PI));
    }
    m_phase_inc = lv_cmake(cosf((m_center + m_offset) * M_PI * m_poly), sinf((m_center+m_offset) * M_PI * m_poly));
}

int upsamp_fxlat_filter::process_c2c(std::complex<float>* in, int n_in, std::complex<float>* out, int out_len)
{
    assert(n_in == m_blksize);
    assert(out_len >= m_blksize*m_poly);
    (void)n_in;
    (void)out_len;
    
    volk_32fc_s32fc_x2_rotator_32fc(m_scratch, in, m_phase_inc, &m_phase, n_in);
    memcpy(&m_hist_c[m_ntaps_per_poly-1], m_scratch, sizeof(lv_32fc_t)*m_blksize);
    for (int i=0; i<m_blksize; i++){
        for (int p=0; p<m_poly; p++){
            volk_32fc_x2_dot_prod_32fc(&out[i*m_poly + m_poly - p - 1], &m_hist_c[i], m_poly_taps[p], m_ntaps_per_poly);
        }
    }
    memcpy(m_hist_c, &m_hist_c[m_blksize], sizeof(lv_32fc_t)*(m_ntaps_per_poly - 1));
    return m_blksize*m_poly;
}
