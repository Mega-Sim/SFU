#!/usr/bin/env python3
# -*- coding:utf-8 -*-

import pandas as pd
from matplotlib import pyplot as plt

oht_log_file_name = 'oht_sig_log.csv'

df = pd.read_csv(oht_log_file_name, index_col='time')

columns = ["shutter_front_open_sen",
            "shutter_front_close_sen",
            "shutter_front_inposition",
            "shutter_front_alarm",
            "shutter_rear_open_sen",
            "shutter_rear_close_sen",
            "shutter_rear_inposition"]

df[columns].plot(subplots=True, grid=True, ylim=(0,1))
plt.show()
