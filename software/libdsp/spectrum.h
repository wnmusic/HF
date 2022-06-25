#ifndef _POWER_SPECTRUM_H
#define _POWER_SPECTRUM_H

#include <fftw3.h>
#include <stdlib.h>
#include <complex>

class spectrum
{
public:
    spectrum(unsigned size, bool b_avg, bool b_window=true, float alpha=0.9f):fft_size(size), m_b_average(b_avg), m_b_win(b_window), m_alpha(alpha){
        
        fft_in   = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*fft_size);
        fft_out  = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*fft_size);
        spec     = (float*)calloc(fft_size,  sizeof(float));
        window   = (float*)malloc(fft_size * sizeof(float));
        fft_plan = fftwf_plan_dft_1d(fft_size, fft_in, fft_out, FFTW_FORWARD, FFTW_ESTIMATE);

        //hamming window by default
        for (unsigned i=0; i<fft_size; i++){
            window[i] = 0.54f - 0.46f*cosf(2.0f*M_PI*i/fft_size);
        }
        norm_factor = 1.0f/fft_size;
    }
    
    ~spectrum(){
        fftwf_destroy_plan(fft_plan);
        fftwf_free(fft_in);
        fftwf_free(fft_out);
        free(spec);
        free(window);
    }

    void enable_average(bool en){ m_b_average = en;}
    void enable_window(bool en){m_b_win  =en;}

    
    void fft_and_log(float *out, bool flip){

        fftwf_execute(fft_plan);
        int d_start = 0, d_inc = 1;
        
        if (flip){
            d_start = fft_size - 1;
            d_inc = -1;
        }

        if (m_b_average){
            unsigned j = d_start;
            float pwr;
            for(unsigned i=fft_size/2; i<fft_size; i++, j+=d_inc){
                pwr = (fft_out[i][0]*fft_out[i][0] + fft_out[i][1]*fft_out[i][1])*norm_factor;
                spec[j] = spec[j]*m_alpha + (1.0f-m_alpha) * pwr;
                out[j] = 10.0f*log10f(spec[j]);                
            }
            for(unsigned i=0; i<fft_size/2; i++, j+=d_inc){
                pwr = (fft_out[i][0]*fft_out[i][0] + fft_out[i][1]*fft_out[i][1])*norm_factor;
                spec[j] = spec[j]*m_alpha + (1.0f-m_alpha) * pwr;
                out[j] = 10.0f*log10f(spec[j]);
            }                
        }else{
            unsigned j = d_start;
            float pwr;
            for(unsigned i=fft_size/2; i<fft_size; i++, j+=d_inc){
                pwr = (fft_out[i][0]*fft_out[i][0] + fft_out[i][1]*fft_out[i][1])*norm_factor;
                out[j] = 10.0f*log10f(pwr);
            }
            for(unsigned i=0; i<fft_size/2; i++, j+=d_inc){
                pwr = (fft_out[i][0]*fft_out[i][0] + fft_out[i][1]*fft_out[i][1])*norm_factor;
                out[j] = 10.0f*log10f(pwr);
            }
        }

        return;
    }

    void process_float(float *in, int in_len, bool flip, float* out){

        unsigned i=0;
        if (in_len > fft_size)
        {
            in_len = fft_size;
        }
        
        if (m_b_win){
            for (i=0; i<in_len; i++){
                fft_in[i][0] = in[i] * window[i];
                fft_in[i][1] = 0.0f;
            }
        }else{
            for (i=0; i<in_len; i++){
                fft_in[i][0] = in[i];
                fft_in[i][1] = 0.0f;
            }
        }
        for(; i<fft_size; i++){
            fft_in[i][0] = 0.0f;
            fft_in[i][1] = 0.0f;
        }
            
        fft_and_log(out, flip);
    }

    void process_complex(std::complex<float> *in, int in_len, bool flip,  float* out){
        unsigned i=0;
        if (in_len > fft_size)
        {
            in_len = fft_size;
        }
        if (m_b_win){
            for (i=0; i<in_len; i++){
                fft_in[i][0] = in[i].real() * window[i];
                fft_in[i][1] = in[i].imag() * window[i];
            }
        }
        else{
            for (i=0; i<in_len; i++){
                fft_in[i][0] = in[i].real();
                fft_in[i][1] = in[i].imag();
            }
        }
        for(; i<fft_size; i++){
            fft_in[i][0] = 0.0f;
            fft_in[i][1] = 0.0f;
        }
        
        fft_and_log(out, flip);
    }
    
private:
    
    fftwf_plan fft_plan;
    fftwf_complex *fft_in;
    fftwf_complex *fft_out;
    float *spec;
    float *window;
    float m_alpha;
    float norm_factor;
    unsigned fft_size;
    bool m_b_average;
    bool m_b_win;
};

#endif
