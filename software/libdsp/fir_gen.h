#pragma once
#include <math.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>

static double bessi0( double x )
/*------------------------------------------------------------*/
/* PURPOSE: Evaluate modified Bessel function In(x) and n=0.  */
/*------------------------------------------------------------*/
{
   double ax,ans;
   double y;


   if ((ax=fabs(x)) < 3.75) {
      y=x/3.75,y=y*y;
      ans=1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492
         +y*(0.2659732+y*(0.360768e-1+y*0.45813e-2)))));
   } else {
      y=3.75/ax;
      ans=(exp(ax)/sqrt(ax))*(0.39894228+y*(0.1328592e-1
         +y*(0.225319e-2+y*(-0.157565e-2+y*(0.916281e-2
         +y*(-0.2057706e-1+y*(0.2635537e-1+y*(-0.1647633e-1
         +y*0.392377e-2))))))));
   }
   return ans;
}


enum{
    HAMMING_WINDOW = 0,
    KAISER_WINDOW = 1
}WINDOW_TYPE;

class fir_lowpass
{
public:
    fir_lowpass(int ntaps, int window, double norm_bw, double beta=3.0){
        
        m_taps = new float[ntaps];
        m_n_taps = ntaps;
        double div = bessi0(beta);
        unsigned M = ntaps -1;
        float alpha = M*1.0f/2;
        for (unsigned i=0; i<=M; i++){
            float x = i*1.0f- alpha;
            float w;
            if (window == HAMMING_WINDOW){
                w = 0.54f - 0.46f*cosf(2.0*M_PI*i/M);
            }else if (window == KAISER_WINDOW){
                w = (float) (bessi0( beta * sqrt(1.0 - x*x/alpha/alpha)) / div);
            }
            if (fabs(x) < 1e-8f){
                m_taps[i] = norm_bw;
            }else{
                m_taps[i] = sinf(norm_bw * M_PI * x) / (M_PI * x) * w;
            }
        }
    }
    
    ~fir_lowpass(){
        delete[] m_taps;
    }
    
    float *taps(){return m_taps;}
    int get_taps(float *out, int out_len){
        memcpy(out, m_taps, sizeof(float)*m_n_taps);
        return m_n_taps;
    }
    float grp_delay(){return (m_n_taps -1)*0.5f;}
private:
    float *m_taps;
    int m_n_taps;
};


class hilbert_transform{
public:    
    hilbert_transform(int ntaps, int window = KAISER_WINDOW,  double beta=3.0):m_n_taps(ntaps){

        if (! (ntaps & 0x01)){
            throw std::runtime_error("number of taps must be odd");
        }

        m_taps = new float[ntaps];
        double div = bessi0(beta);
        unsigned M = ntaps -1;
        float alpha = M*1.0f/2;
        for (unsigned i=0; i<=M; i++){
            float x = i*1.0f- alpha;
            float w;
            if (window == HAMMING_WINDOW){
                w = 0.54f - 0.46f*cosf(2.0*M_PI*i/M);
            }else if (window == KAISER_WINDOW){
                w = (float) (bessi0( beta * sqrt(1.0 - x*x/alpha/alpha)) / div);
            }
            if (fabs(x) < 1e-8f){
                m_taps[i] = 0.0f;
            }else{
                m_taps[i] = 2.0 * sinf(M_PI * x * 0.5) * sinf(M_PI * x * 0.5) / (M_PI * x) * w;
            }
        }
    }    
    ~hilbert_transform(){
        delete [] m_taps;
    }

    float *taps(){ return m_taps;}
    
    int get_taps(float *out, int out_len){
        memcpy(out, m_taps, sizeof(float)*m_n_taps);
        return m_n_taps;
    }
    int grp_delay(){return (m_n_taps -1)/2;}
    
private:
    float *m_taps;
    int m_n_taps;
};
