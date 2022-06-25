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


#ifndef FXLAT_FILTER_H_
#define FXLAT_FILTER_H_
#include <volk/volk.h>
class fxlat_filter
{
public:
  /* taps and n_taps defines the filter
   * dec defines the decimation ratio 
   * The taps are real, no matter what the input is, which is specified 
   * by b_complex 
   */
    fxlat_filter(const float *taps, int n_taps, int decimate, int blksize, float center = 0.0f);
    ~fxlat_filter();
    
    void set_center(float center);
        
    /* return the output len which is blksize/dec  */
    int process_r2c(float* in, int n_in, std::complex<float>* out, int out_len);
    int process_c2c(std::complex<float>* in, int n_in, std::complex<float>* out, int out_len);
    
 private:
    float          *m_float_taps;
    lv_32fc_t      *m_taps;
    int             m_n_taps;
    float          *m_hist;
    lv_32fc_t      *m_hist_c;
    lv_32fc_t      *m_scratch;
    int             m_poly;
    int             m_blksize;
    float           m_center;
    lv_32fc_t       m_phase;
    lv_32fc_t       m_phase_inc;
};


#endif
