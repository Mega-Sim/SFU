# -*- coding: utf-8 -*-

import pandas as pd
import datetime as dt
import matplotlib.pyplot as plt
import matplotlib.font_manager as font_manager
import matplotlib.dates
from matplotlib.dates import WEEKLY,MONTHLY, DateFormatter, rrulewrapper, RRuleLocator 
import numpy as np
  
 
def _create_date(datetxt):
    """Creates the date"""
    day,month,year=datetxt.split('-')
    date = dt.datetime(int(year), int(month), int(day))
    mdate = matplotlib.dates.date2num(date) 
    return mdate
 
def _create_datetime(strdatetime):
    """Creates the date"""
    date = dt.datetime.fromisoformat('2020-01-15 15:40:00.218')
    mdate = matplotlib.dates.date2num(date) 
    return mdate    

def MyGanttChart(dfRow, ylabels):
    """
        Create gantt charts with matplotlib
        Give eventDataFrame & yLabels.
    """            
    ilen=len(ylabels)
    pos = np.arange(0.5,ilen*0.5+0.5,0.5)
    task_dates = {}
    for i, task in enumerate(ylabels):
        task_dates[task] = [dfRow[i*2], dfRow[i*2+1]]
        
    fig = plt.figure(figsize=(20,8)) # figsize inch
    ax = fig.add_subplot(111) # 전체 창을 가로 1칸, 세로 1칸으로 나누고 이중 1번째에 axes 생성
    for i in range(len(ylabels)):
         start_date,end_date = task_dates[ylabels[i]]
         ax.barh((i*0.5)+0.5, end_date - start_date, left=start_date, height=0.3, align='center', edgecolor='lightgreen', color='orange', alpha = 0.8)
         dtText = matplotlib.dates.num2date((end_date - start_date)+1) # 1보다 작으면 날짜가 0라서 에러.. 최소 1 필요         
         ax.text(end_date, (i*0.5)+0.5, dtText.strftime("%S.%f")[:-3], rotation=0, ha='left', va='center')
    
    locsy, labelsy = plt.yticks(pos,ylabels)
    plt.setp(labelsy, fontsize = 14)

    ax.set_ylim(ymin = -0.1, ymax = ilen*0.5+0.5)
    ax.grid(color = 'g', linestyle = ':')
    
    ax.xaxis_date()
    labelsx = ax.get_xticklabels()
    plt.setp(labelsx, rotation=30, fontsize=10)
 
    ax.invert_yaxis()
    fig.autofmt_xdate()
    plt.savefig('gantt1.svg', format='svg') #.svg - Scalable Vector Graphics
    plt.show()

if __name__ == '__main__': #직접 실행시 name이 '__main__' -> Module Import 시 실행하고 싶지 않을 경우 구문
    df = pd.DataFrame({'C1':[0,100,200], 'C2':[10,120,230], 'C3':[30,140,250], 'C4':[60, 150, 280]})    
    MyGanttChart(df, ['EVT1', 'EVT2'])
