#!/usr/bin/env python

import os, sys
import pandas as pd
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
    print( 'Usage> %s <datafile>' % sys.argv[0])
    sys.exit(-1)

try:
    df = pd.read_csv(sys.argv[1], sep=' ');
    df.columns = ['id', 't', 'pr', 'vr', 'p', 'v', 'trq', 'sp', 'sv', 'st'] # s-slave    
except:
    print('Fail to read', sys.argv[1])
    sys.exit(-1)

#print df

#df[['t', 'pr','vr', 'p', 'v', 'sp','sv']].plot(x='t')
df[['t', 'pr', 'p', 'trq', 'st']].plot(x='t')
plt.grid()
plt.show()
