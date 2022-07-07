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
    [(1024, 256)]
)
def test_transform(N, B):
    hilbert = hilbert_xfrm(B);
    n = np.arange(N)
    x0 = np.sin(0.025*np.pi*n, dtype=np.float32);
    y = []
    for b in range(N//B):
        Ny,y0 = hilbert.process(x0[b*B:(b+1)*B], B, 1)
        y += y0[0:Ny].tolist()
    y = np.array(y)

    y_ref = signal.hilbert(x0)
    plot.plot(np.real(y))
    plot.plot(np.imag(y))
    plot.show()

    plot.plot(np.abs(np.fft.fft(y_ref)))
    plot.plot(np.abs(np.fft.fft(y)))
    plot.show()



if __name__ == '__main__':
    test_transform(1024, 80)
    
