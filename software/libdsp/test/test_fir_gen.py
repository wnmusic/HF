import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plot
import sys
import os
import pytest

sys.path.append('../../build/libdsp/test')
from pydsp import *


@pytest.mark.parametrize(
    "window_type, M",
    [(0,256), (0, 255), (1, 256), (1, 255)]
)
def test_fir_gen(window_type, M):
    beta = 3.5
    fc = 0.1
    fir = fir_lowpass(M, window_type, fc, beta);
    ntaps, taps = fir.get_taps(M);
    if (window_type == 0):
        w = np.hamming(M)
    elif (window_type == 1):
        w = np.kaiser(M, beta)
    h = fc*np.sinc(fc*(np.arange(ntaps)-(M-1)/2)) * w
    
    assert np.max(h - taps) < 1e-6, 'max error mag is: {}'.format(np.max(h - taps))
    return taps;

@pytest.mark.parametrize(
    "window_type, M",
    [(0,255), (1, 127)]
)
def test_hilbert_xfm(window_type, M):
    beta = 3.5
    fir = hilbert_transform(M, window_type, beta);
    ntaps, taps = fir.get_taps(M);
    if (window_type == 0):
        w = np.hamming(M)
    elif (window_type == 1):
        w = np.kaiser(M, beta)

    x = np.arange(M) - (M-1)//2
    h = np.sin(0.5*x*np.pi) * np.sinc(0.5*x) * w
    assert np.max(h - taps) < 1e-6, 'max error mag is: {}'.format(np.max(h - taps))
    return taps

if __name__ == '__main__':
    import scipy.signal as signal
    M=802
    #taps = test_hilbert_xfm(1, M);
    n = np.arange(2048)
    taps = test_fir_gen(0, M);
    x = np.sin(0.025*np.pi*n);
    x1 = np.convolve(taps, x);
    plot.plot(20*np.log10(np.abs(np.fft.fft(taps))))
    plot.show()
    plot.plot(x1)
    plot.show()
    
