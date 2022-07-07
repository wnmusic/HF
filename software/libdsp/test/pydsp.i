%module pydsp
%{
#define SWIG_FILE_WITH_INIT
#include "resample.h"
#include "decimate.h"
#include "polyphase.h"
#include "fxlat_filter.h"
#include "fir_gen.h"
#include "upsamp_fxlat_filter.h"
#include "blkconv.h"
#include "hilbert_xfrm.h"        
%}

%include "numpy.i"

%init %{
import_array();
%}


%apply (float *IN_ARRAY1, int DIM1) {(const float *taps, int n_taps)};
%apply (float *IN_ARRAY1, int DIM1) {(float *in, int n_in)};
%apply (float *ARGOUT_ARRAY1, int DIM1) {(float *out, int out_len)};
%apply (std::complex<float> *IN_ARRAY1, int DIM1) {(std::complex<float> *in, int n_in)};
%apply (std::complex<float> *ARGOUT_ARRAY1, int DIM1) {(std::complex<float> *out, int out_len)};

%include "resample.h"
%include "decimate.h"
%include "polyphase.h"
%include "fxlat_filter.h"
%include "fir_gen.h"
%include "upsamp_fxlat_filter.h"
%include "blkconv.h"
%include "hilbert_xfrm.h"

