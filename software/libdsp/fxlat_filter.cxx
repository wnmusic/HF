/*
Copyright (c) 2019, Ning Wang <nwang.cooper@gmail.com> All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
    
     Redistributions of source code must retain the above copyright notice, 
     this list of conditions and the following disclaimer.

     Redistributions in binary form must reproduce the above copyright notice, 
     this list of conditions and the following disclaimer in the 
     documentation and/or other materials provided with the distribution.
 
     Neither the name of its contributors can be used to endorse or promote 
     products derived from this software witthout specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "fxlat_filter.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <immintrin.h>


fxlat_filter::fxlat_filter(const float *taps, int n_taps, int dec, int blksize, float center):
    m_poly(dec), m_blksize( blksize) ,m_hist(NULL), m_n_taps(n_taps)
{

    printf("ntaps: %d\n", n_taps);
    m_float_taps = (float*)volk_malloc(m_n_taps*sizeof(float), volk_get_alignment());
    memcpy(m_float_taps, taps, sizeof(float)*m_n_taps);
    
    m_taps = (lv_32fc_t*) volk_malloc(m_n_taps*sizeof(lv_32fc_t), volk_get_alignment());

    m_hist = (float *)volk_malloc(2*sizeof(float)*(n_taps + blksize - 1), volk_get_alignment());
    memset(m_hist, 0, 2*sizeof(float)*(n_taps + blksize - 1));
    m_hist_c = (lv_32fc_t*) m_hist;

    m_scratch = (lv_32fc_t*)volk_malloc(m_blksize * sizeof(lv_32fc_t), volk_get_alignment());

    m_phase = lv_cmake(1.0f, 0.0f);
    set_center(center);
}

fxlat_filter::~fxlat_filter()
{
    volk_free(m_hist);    
    volk_free(m_taps);
    volk_free(m_scratch);    
}

/* to reduce compute sin and cos for the high sampling rate, we ues the same 
 * trick as in gnuradio
 * make the filter bandpass at center frequency and decimate, and then, shift 
 * back to infalted center
 */
void fxlat_filter::set_center(float center)
{
    /* center is baesd on the sample rate before decimation */
    m_center = center > 1.0f ? 1.0f : center < -1.0f ? -1.0f : center;
    /* notice that m_taps is the filipped version of taps */
    for (unsigned i = m_n_taps -1, j=0; j<m_n_taps; i--,j++){
        m_taps[i] = m_float_taps[j] * lv_cmake(cosf(j*m_center*M_PI), sinf(j*m_center*M_PI));
    }
    m_phase_inc = lv_cmake(cosf(m_center * M_PI * m_poly), -sinf(m_center * M_PI * m_poly));
}


int fxlat_filter::process_r2c(float* in, int n_in, lv_32fc_t* out, int out_len)
{
    
    int n_out = 0;
    assert(n_in == m_blksize);
    assert(out_len >= m_blksize/m_poly);
    (void)n_in;
    (void)out_len;

    /* first copy the input data into history buffer*/
    memcpy(&m_hist[m_n_taps-1], in, sizeof(float)*m_blksize);
    for (unsigned i=0; i<m_blksize; i+=m_poly){
        volk_32fc_32f_dot_prod_32fc(&m_scratch[n_out], m_taps, &m_hist[i], m_n_taps);
        n_out++;
    }
    memcpy(m_hist, &m_hist[m_blksize], sizeof(float)*(m_n_taps - 1));
    /* now we have to rotate back */
    volk_32fc_s32fc_x2_rotator_32fc(out, m_scratch, m_phase_inc, &m_phase, n_out);
    return n_out;
}

int fxlat_filter::process_c2c(std::complex<float>* in, int n_in, std::complex<float>* out, int out_len)
{
    int n_out = 0;
    assert(n_in == m_blksize);
    assert(out_len >= m_blksize/m_poly);
    (void)n_in;
    (void)out_len;

    /* first copy the input data into history buffer*/
    memcpy(&m_hist_c[m_n_taps-1], in, sizeof(lv_32fc_t)*m_blksize);
    for (unsigned i=0; i<m_blksize; i+=m_poly){
        volk_32fc_x2_dot_prod_32fc(&m_scratch[n_out], &m_hist_c[i], m_taps, m_n_taps);
        n_out++;
    }
    memcpy(m_hist_c, &m_hist_c[m_blksize], sizeof(lv_32fc_t)*(m_n_taps - 1));
    /* now we have to rotate back */
    volk_32fc_s32fc_x2_rotator_32fc(out, m_scratch, m_phase_inc, &m_phase, n_out);
    return n_out;

}
