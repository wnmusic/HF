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

#ifndef RESAMPLE_H_
#define RESAMPLE_H_

static const float def_taps[] = {
    4.154052476224024e-06, 6.836163811385632e-05, 0.000311697949655354, 0.0005662626936100423, -0.00019575881015043706, -0.004200732801109552, -0.013667905703186989, -0.027301859110593796, -0.03584320470690727, -0.02054252102971077, 0.04132149741053581, 0.16359668970108032, 0.33717116713523865, 0.5246731638908386, 0.670900285243988, 0.7262777090072632, 0.6709002256393433, 0.5246731042861938, 0.33717113733291626, 0.16359665989875793, 0.04132148623466492, -0.020542524755001068, -0.03584319353103638, -0.027301862835884094, -0.013667899183928967, -0.004200733732432127, -0.0001957584754563868, 0.000566263624932617, 0.0003116970765404403, 6.836453394498676e-05, 4.154052476224024e-06
};
static const int def_upsample = 4;


class resample
{
public:
    /* taps and n_taps defines the interpolation filter, 
     * upsample defines how much upsampling 
     * blksize is the number of samples processed on each process call */
    resample(const float *taps, int n_taps, int upsample, int blksize);
    resample(int blksize) : resample(def_taps, sizeof(def_taps)/sizeof(float), def_upsample,  blksize){}

            
    ~resample();

    /* given a rate of "how many" input samples (step size, like 1.52) to 
     * produce an output
     * n_in should be less than blk_size
     * number of output samples generated is returned 
     */
    int process(float* in, int n_in, float *out, int out_len, float rate);
private:
    float          *m_history;
    int             m_phase_len;
    int             m_n_phase;
    float         **m_phase_taps;
    float         **m_out;
    
    int             m_blksize;
    int             m_pos;
    float           m_mu;
    float           m_last_remain;
    bool            m_is_leftover;

};


#endif
