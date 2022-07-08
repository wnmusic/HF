import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plot
import sys
import os
import pytest

sys.path.append('../../build/libdsp/test')
from pydsp import *

taps = [4.154052476224024e-06, 6.836163811385632e-05, 0.000311697949655354, 0.0005662626936100423, -0.00019575881015043706, -0.004200732801109552, -0.013667905703186989, -0.027301859110593796, -0.03584320470690727, -0.02054252102971077, 0.04132149741053581, 0.16359668970108032, 0.33717116713523865, 0.5246731638908386, 0.670900285243988, 0.7262777090072632, 0.6709002256393433, 0.5246731042861938, 0.33717113733291626, 0.16359665989875793, 0.04132148623466492, -0.020542524755001068, -0.03584319353103638, -0.027301862835884094, -0.013667899183928967, -0.004200733732432127, -0.0001957584754563868, 0.000566263624932617, 0.0003116970765404403, 6.836453394498676e-05, 4.154052476224024e-06]

@pytest.mark.parametrize(
    "N, B, D",
    [(2048, 32, 2), (2048, 32, 4)]
)
def test_filter0(N, B, D):
    taps = np.hamming(127);
    center = 0.2
    x0 = np.sin(0.22*np.pi*np.arange(N), dtype=np.float32)
    x0 = np.random.randn(N).astype(np.float32) + 1j*np.random.randn(N).astype(np.float32)
    flt = fxlat_filter(taps.astype(np.float32), D, B, center)

    y = []
    for b in range(N//B):
        Ny,y0 = flt.process_c2c(x0[b*B:(b+1)*B], B//D)
        y += y0[0:Ny].tolist()

    y = np.array(y)
    xs = x0*(np.exp(-1j*center*np.pi*np.arange(N)).astype(np.complex64))
    y_ref = signal.lfilter(taps, [1], xs)
    err = np.abs(y - y_ref[0::D])
    #plot.plot(np.real(y))
    #plot.plot(np.imag(y))
    #plot.plot(np.real(y_ref[0::D]))
    #plot.plot(np.imag(y_ref[1::D]))
    #plot.show()
    #plot.plot(np.abs(err)/(1+np.abs(y)))
    #plot.show()
    assert  np.mean(np.abs(err)) < 1e-3, "Error {}".format(np.mean(np.abs(err)))

@pytest.mark.parametrize(
    "N, B, D",
    [(2048, 32, 2),(2048, 32, 4),(2400, 24, 6)]
)
def test_filter1(N, B, D):
    taps = np.hamming(127);
    center = 0.2
    x0 = np.exp(1j*0.22*np.pi*np.arange(N), dtype=np.complex64)
    flt = fxlat_filter(taps.astype(np.float32), D, B, center)

    y = []
    for b in range(N//B):
        Ny,y0 = flt.process_c2c(x0[b*B:(b+1)*B], B//D)
        y += y0[0:Ny].tolist()

    y = np.array(y)
    xs = x0*(np.exp(-1j*center*np.pi*np.arange(N)).astype(np.complex64))
    y_ref = signal.lfilter(taps, [1], xs) 
    #plot.plot(np.real(y))
    #plot.plot(np.real(y_ref[0::D]))
    #plot.show()
    err = y - y_ref[0::D]
    assert  np.mean(err) < 1e-5, "Error {},  dut: {}, ref:{}".format(err, y, y_ref[0::D])

@pytest.mark.parametrize(
    "N, B, L",
    [(256, 32, 2),(256, 32, 4),(256, 32, 6)]
)
def test_up_filter(N, B, L):
    center = -0.3
    x0 = np.exp(1j*0.02*np.pi*np.arange(N), dtype=np.complex64)
    fc = 0.05/L

    flt = upsamp_fxlat_filter(L, 13, center, fc, B)
    y = []
    for b in range(N//B):
        Ny,y0 = flt.process_c2c(x0[b*B:(b+1)*B], B*L)
        y += y0[0:Ny].tolist()
    y = np.array(y)

    x1 = np.zeros(N*L, dtype=np.complex64)
    for (i, x) in enumerate(x0):
        x1[L*i] = x
    ntaps = L*13
    #taps = np.hamming(ntaps) * fc*np.sinc(fc*(np.arange(ntaps)-(ntaps-1)/2))
    taps = np.hamming(ntaps) * fc * np.sinc(fc*(np.arange(ntaps)-(ntaps-1)/2))
    y_ref = signal.lfilter(taps, [1], x1);
    y_ref = y_ref * np.exp(1j*np.pi*center*np.arange(len(y_ref)))
    freq = np.linspace(-1.0, 1.0, num=1024)
    
    #plot.plot(freq, np.abs(np.fft.fftshift(np.fft.fft(y_ref))))
    #plot.plot(freq, np.abs(np.fft.fftshift(np.fft.fft(y))))
    #plot.plot(np.real(y))
    #plot.plot(np.real(y_ref))
    plot.show()
    assert np.mean(np.abs(y - y_ref)) < 1e-5, "error is too big"
    
    
if __name__ == "__main__":

    print(os.getpid())
    input()
    test_up_filter(256, 32, 4)
    #test_filter0(2048, 32, 8)
