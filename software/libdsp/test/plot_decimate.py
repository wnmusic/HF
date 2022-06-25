import numpy as np

import matplotlib.pyplot as plot
import sys
import os

sys.path.append('../build/test')
from pydsp import *

N = 1024
B = 128
# a low pass at 0.18pi
taps = [4.154052476224024e-06, 6.836163811385632e-05, 0.000311697949655354, 0.0005662626936100423, -0.00019575881015043706, -0.004200732801109552, -0.013667905703186989, -0.027301859110593796, -0.03584320470690727, -0.02054252102971077, 0.04132149741053581, 0.16359668970108032, 0.33717116713523865, 0.5246731638908386, 0.670900285243988, 0.7262777090072632, 0.6709002256393433, 0.5246731042861938, 0.33717113733291626, 0.16359665989875793, 0.04132148623466492, -0.020542524755001068, -0.03584319353103638, -0.027301862835884094, -0.013667899183928967, -0.004200733732432127, -0.0001957584754563868, 0.000566263624932617, 0.0003116970765404403, 6.836453394498676e-05, 4.154052476224024e-06]


x0 = np.sin(0.02*np.pi*np.arange(N), dtype=np.float32)
dec = decimate(taps, 4, B)

print( os.getpid())
input()

y = []
for b in range(N/B):
    Ny,y0 = dec.process(x0[b*B:(b+1)*B],4*B, 1.77)
    y += y0[0:Ny].tolist()
                        

interp = resample(taps, 4, B)
y1 = []
for b in range(N/B):
    Ny,y0 = interp.process(x0[b*B:(b+1)*B],4*B, 1.77)
    y1 += y0[0:Ny].tolist()


    
#plot.plot(np.array(y) - np.array(y1))
#plot.show()

