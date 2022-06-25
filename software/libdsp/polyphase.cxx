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

#include "polyphase.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <immintrin.h>
#include <volk/volk.h>

polyphase::polyphase(const float *taps, int n_taps, int n_poly, int blksize):
    m_poly(n_poly), m_blksize( blksize) ,m_hist(NULL), m_n_taps(n_taps)
{
    unsigned alignment = volk_get_alignment();
    
    m_n_poly_taps = (n_taps + (m_poly - 1))/m_poly;
    m_filter_len = m_n_poly_taps * m_poly;

    m_taps = (float*)volk_malloc(m_filter_len * sizeof(float), alignment);
    memset(m_taps, 0, sizeof(float)*m_filter_len);

    
    for (unsigned i = m_filter_len -1, j=0; taps && j<m_n_taps; i--,j++){
        m_taps[i] = taps[j];
    }

    m_hist = (float*)volk_malloc(2*(n_taps + blksize - 1) * sizeof(float), alignment);
    memset(m_hist, 0, 2*sizeof(float)*(n_taps + blksize - 1));
    m_hist_c = (lv_32fc_t*)m_hist;
}
polyphase::~polyphase()
{
    volk_free(m_taps);
    volk_free(m_hist);
}


int polyphase::filter_and_decimate(float* __restrict__ in, int n_in, float* __restrict__ out, int out_len)
{
    int n_out = 0;

    assert(n_in == m_blksize);
    assert(out_len >= m_blksize/m_poly);
    (void)n_in;
    (void)out_len;

    /* first copy the input data into history buffer*/
    memcpy(&m_hist[m_n_taps-1], in, sizeof(float)*m_blksize);
    
    /* polyphase filtering decimation */
    for (unsigned i=0; i<m_blksize; i+=m_poly){
        float r = 0.0f;
        for (unsigned n=0; n<m_filter_len; n+=m_poly){
#if __AVX__
            unsigned p= 0;
            __m256 y = _mm256_set1_ps(0.0f);
            for (p=0; p<(m_poly/8)*8; p+= 8)
            {
                __m256 x = _mm256_loadu_ps(&m_hist[i+n +p ]);
                __m256 h = _mm256_loadu_ps(&m_taps[n + p]);
                y = _mm256_add_ps(y, _mm256_mul_ps(x, h));
            }
            
            y = _mm256_add_ps(y, _mm256_permute_ps(y, _MM_SHUFFLE(0, 1, 2, 3)));
            y = _mm256_add_ps(y, _mm256_permute_ps(y, _MM_SHUFFLE(2, 3, 0, 1)));
            y = _mm256_add_ps(y, _mm256_permute2f128_ps(y, y, 1));
            r += _mm_cvtss_f32(_mm256_extractf128_ps(y, 0));

            for (; p<m_poly; p++){
                r += m_hist[i+n+p] * m_taps[n+p];
            }
#else            
            for (unsigned p=0; p<m_poly; p++){
                r += m_hist[i+n+p] * m_taps[n+p];
            }
#endif            
        }
        out[n_out] = r;
        n_out++;
    }
    /* progress the history buffer */
    memcpy(m_hist, &m_hist[m_blksize], sizeof(float)*(m_n_taps - 1));

    return n_out;
}


int polyphase::filter_and_decimate_c2c(std::complex<float>* __restrict__ in, int n_in, std::complex<float>* __restrict__ out, int out_len)
{

    int n_out = 0;
    assert(n_in == m_blksize);
    assert(out_len >= m_blksize/m_poly);
    (void)n_in;
    (void)out_len;

    /* first copy the input data into history buffer*/
    memcpy(&m_hist_c[m_n_taps-1], in, sizeof(lv_32fc_t)*m_blksize);
    for (unsigned i=0; i<m_blksize; i+=m_poly){
        volk_32fc_32f_dot_prod_32fc(&out[n_out], &m_hist_c[i], m_taps, m_n_taps);
        n_out++;
    }
    memcpy(m_hist_c, &m_hist_c[m_blksize], sizeof(lv_32fc_t)*(m_n_taps - 1));
    return n_out;
}

int polyphase::filter_and_upsample(float* __restrict__ in, int n_in, float* __restrict__ out, int out_len)
{
    int n_out = 0;

    assert(n_in == m_blksize);
    assert(out_len >= m_blksize*m_poly);
    (void)n_in;
    (void)out_len;

    /* first copy the input data into history buffer*/
    memcpy(&m_hist[m_n_poly_taps-1], in, sizeof(float)*m_blksize);
    
    /* polyphase filtering decimation */
    for (unsigned i=0; i<m_blksize; i++){
        float *p_out = &out[n_out];
        memset(p_out, 0, sizeof(float)*m_poly);
        
        for (unsigned n=0, m=0; n<m_filter_len; n+=m_poly,m++){
            for (unsigned p=0; p<m_poly; p++){
                p_out[m_poly-p-1] += m_hist[i+m] * m_taps[n+p];
            }
        }
        n_out += m_poly;
    }
    /* progress the history buffer */
    memcpy(m_hist, &m_hist[m_blksize], sizeof(float)*(m_n_poly_taps - 1));

    return n_out;
}
