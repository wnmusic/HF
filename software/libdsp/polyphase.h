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

#ifndef POLYPHASE_H_
#define POLYPHASE_H_

#include <complex>
#include <string.h>
class polyphase
{
public:
  /* taps and n_taps defines the filter
   * dec defines the decimation ratio 
   * The taps are real, no matter what the input is, which is specified 
   * by b_complex 
   */
    polyphase(const float *taps, int n_taps, int n_poly, int blksize);
    ~polyphase();

    /* return the output len which is blksize/dec  */
    int filter_and_decimate(float* in, int n_in, float* out, int out_len);
    int filter_and_decimate_c2c(std::complex<float>* in, int n_in, std::complex<float>* out, int out_len);
    int filter_and_upsample(float* in, int n_in, float* out, int out_len);

    float *taps(){ return m_taps;}
    void set_taps(float *taps, int n_taps){
        
        unsigned i, j;
        unsigned _n = n_taps < m_n_taps ? n_taps : m_n_taps;
        memset(m_hist, 0, 2*sizeof(float)*(m_n_taps + m_blksize - 1));
        for (i = m_filter_len -1, j=0; taps && j<_n; i--,j++){
            m_taps[i] = taps[j];
        }
    }
 private:
    float          *m_taps;
    int             m_n_taps;
    int             m_n_poly_taps;    
    int             m_filter_len;
    float          *m_hist;
    std::complex<float>      *m_hist_c;
    int             m_poly;
    int             m_blksize;
};


#endif
