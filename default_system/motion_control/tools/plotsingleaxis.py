#!/usr/bin/env python

import os, sys
import pandas as pd
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
    print ('Usage> %s <datafile>' % sys.argv[0])
    sys.exit(-1)

try:
    df = pd.read_csv(sys.argv[1], sep=' ');
    df.columns = ['id', 't', 'pr', 'vr', 'p', 'v', 'trq', 'pe', 'ts']
    #print df.columns
except:
    print ('Fail to read', sys.argv[1])
    sys.exit(-1)

#print df

#df[['t','pr', 'p', 'vr']].plot(x='t')
df['id100'] = df['id']*100 
df[['ts','vr','v']].plot(x='ts')
plt.grid()
plt.show()
