import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plot
import sys
import os
import pytest

sys.path.append('../../build/libdsp/test')
from pydsp import *

@pytest.mark.parametrize(
    "N, B",
    [(1024, 256), (1024, 128), (2000, 200)]
)
def test_blkconv(N, B):
    fc = 0.1
    Nfft = 512
    M = Nfft - B + 1
    w = np.hamming(M)
    h = fc*np.sinc(fc*(np.arange(M)-(M-1)/2)) * w
    flt = blkconv(h.astype(np.float32), Nfft)
    x0 = np.sin(0.01*np.pi*np.arange(N), dtype=np.float32)
    
    y = []
    for b in range(N//B):
        Ny,y0 = flt.process_f2f(x0[b*B:(b+1)*B], B)
        y += y0[0:Ny].tolist()
    y = np.array(y)

    y_ref = signal.lfilter(h, [1], x0)
    err = y - y_ref;
    #plot.plot(x0)
    #plot.plot(y)
    #plot.plot(y_ref)
    #plot.show()
    assert np.max(np.abs(err)) < 1e-5
    

if __name__ == '__main__':
    test_blkconv(1024, 128)
