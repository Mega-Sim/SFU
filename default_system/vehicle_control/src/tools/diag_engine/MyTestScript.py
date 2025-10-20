# -*- coding: utf-8 -*-
"""
Created on Wed Jan 22 13:24:26 2020

@author: taein78.kim
"""
import sys
import time
import os
import glob
import numpy as np
import pandas as pd
import pickle
import datetime as dt
import matplotlib.pyplot as plt
import matplotlib.font_manager as font_manager
import matplotlib.dates
from matplotlib.dates import WEEKLY,MONTHLY, DateFormatter, rrulewrapper, RRuleLocator 
import numpy as np
from numpy import nan as NaN
import seaborn as sns

sys.path.append('C:/MCCDiag/python_package')

import MyGantt as myg
import diag_functions as dfn

###################################################################################
# 수동 Log 파일에서 Dataframe 생성
# filepath
f = r'D:\PERSONAL_SPACE\## MCC\0. 주간 작업 내용\2020-02-12\Osc lookdown test log\MCC2020021110.log'
# Log Loading
df = pd.read_csv(f, header = None)
# Header Define
df = df.rename(columns={0:'Time', 1:'MachineID', 2:'Type', 3:'Event ID'})

# TRAS Type Filter
dfTrans = df.loc[(df['Type']=='TRANS')]

strDate = f[-14:-10] + '-' + f[-10:-8] + '-' + f[-8:-6] #yyyyMMdd
dfBuf =dfTrans['time']

lst = []
for strTime in dfBuf:
    strDateTime = strDate + ' ' + strTime[1:13]
    lst.append(strDateTime)   

dfTrans['time'] = lst  # 경고 발생하나.. 일단 무시

"""
dfTransUnload - TRANS Type 중 Unload Event Filter
"""
# ULD Event Filter : EventID 중 'UL'로 시작하는 이벤트만 추출
l = []
for d in dfTrans['EventID']:
    l.append(d[:2] == 'UL')

dfTransUnload = dfTrans.loc[l]

###################################################################################
"""
Best Practice 찾자..
"""
def fileFilterTransOscillation(file):
    """
    Trans Oscillation Event 만 추출
    """
    f = file
    print(f)
    # Log Loading
    #df = pd.read_csv(f, header = None)
    df = pd.read_csv (f, names = ['Time','Machine ID','Type', 'Event ID', 'StartEnd', 'Material', 'Station ID', 'Port Type', 'Node ID', 'Offset',\
                                  'Key1','Value1','Key2','Value2','Key3','Value3','Key4','Value4','Key5','Value5',\
                                  'Key6','Value6','Key7','Value7','Key8','Value8','Key9','Value9','Key10','Value 10'])   
    # 임시
    #df = df.rename(columns={'Time':'time', 'OHT':'MachineID', 'Status':'Dummy1', 'Box type':'Dummy2', 'Station ID':'EventID'})  
    
    # TRANS Type Filter
    dfTrans = df.loc[(df['Type']=='TRANS')]
    
    # File Name에서 연/월/일 추출하여 time과 병합
    strDate = f[-14:-10] + '-' + f[-10:-8] + '-' + f[-8:-6] #yyyyMMdd
    dfBuf =dfTrans['Time']
    
    lst = []
    for strTime in dfBuf:
        strDateTime = strDate + ' ' + strTime[1:13]
        lst.append(strDateTime)   
    
    dfTrans['Time'] = lst  # 경고 발생하나.. 일단 무시
    
    # ULD Event Filter : EventID 중 'OSCI'로 시작하는 이벤트만 추출
    l = []
    for d in dfTrans['Box type']:
        l.append(d[:2] == 'OSCI')
    
    return dfTrans.loc[l]


def fileFilterTransUnload(file):
    """
    Trans Unload Event 만 추출
    """
    f = file
    print(f)
    # Log Loading
    df = pd.read_csv(f, header = None)
    df = df.rename(columns = {0:'Time',1:'Machine ID',2:'Type',3:'Event ID',4:'StartEnd',5:'Material',\
                              6:'Station ID',7:'Port Type',8:'Node ID',9:'Offset',\
                              10:'Key1',11:'Value1',12:'Key2',13:'Value2',14:'Key3',15:'Value3',16:'Key4',17:'Value4',18:'Key5',19:'Value5',\
                              20:'Key6',21:'Value6',22:'Key7',23:'Value7',24:'Key8',25:'Value8',26:'Key9',27:'Value9',28:'Key10',29:'Value10',\
                              30:'Key11',31:'Value11',32:'Key12',33:'Value12',34:'Key13',35:'Value13',36:'Key14',37:'Value14',38:'Key15',39:'Value15',\
                              40:'Key16',41:'Value16',42:'Key17',43:'Value17',44:'Key18',45:'Value18',46:'Key19',47:'Value19',48:'Key20',49:'Value20',\
                              50:'Key21',51:'Value21'})
    
    # TRANS Type Filter
    dfTrans = df.loc[(df['Type']=='TRANS')]
    
    # File Name에서 연/월/일 추출하여 time과 병합
    strDate = f[-14:-10] + '-' + f[-10:-8] + '-' + f[-8:-6] #yyyyMMdd
    dfBuf =dfTrans['Time']
    
    lst = []
    for strTime in dfBuf:
        strDateTime = strDate + ' ' + strTime[1:13]
        lst.append(strDateTime)   
    
    dfTrans['Time'] = lst  # 경고 발생하나.. 일단 무시
    
    # ULD Event Filter : EventID 중 'UL'로 시작하는 이벤트만 추출
    l = []
    for d in dfTrans['Event ID']:
        l.append(d[:2] == 'UL')
    
    return dfTrans.loc[l]


def fileFilterTrans(file):
    """
    Trans Event 만 추출
    """
    f = file
    print(f)
    # Log Loading
    #df = pd.read_csv(f, header = None)
    df = pd.read_csv (f, names = ['Time','OHT','Type', 'Event ID', 'Status', 'Box type', 'Station ID', 'EQ', 'Node ID', 'Offset', 'Data Key 1','Data Value 1','Data Key 2','Data Value 2','Data Key 3','Data Value 3','Data Key 4','Data Value 4','Data Key 5','Data Value 5','Data Key 6','Data Value 6','Data Key 7','Data Value 7','Data Key 8','Data Value 8','Data Key 9','Data Value 9','Data Key 10','Data Value 10' \
                                  , 'Data Key 11','Data Value 11', 'c34', 'c35', 'c36', 'c37', 'c38', 'c39','c40','c41','c42','c43','c44','c45','c46','c47','c48','c49','c50','c51','c52','c53'])   
    # TRANS Type Filter
    dfTrans = df.loc[(df['Type']=='TRANS')]
    
    # File Name에서 연/월/일 추출하여 time과 병합
    strDate = f[-14:-10] + '-' + f[-10:-8] + '-' + f[-8:-6] #yyyyMMdd
    dfBuf =dfTrans['Time']
    
    lst = []
    for strTime in dfBuf:
        strDateTime = strDate + ' ' + strTime[1:13]
        lst.append(strDateTime)   
    
    dfTrans['Time'] = lst  # 경고 발생하나.. 일단 무시
    
    return dfTrans

# Log->Pickle 만들기..
def fileMake():
    fp = r'D:/LogMCCRaw/SEMES/V*/MCC*.log'
    files = glob.glob(fp)
    nCnt = 0
    for f in files:
        df = pd.DataFrame()    
        df = pd.read_csv(f, names=['C1','C2','C3','C4','C5','C6','C7','C8','C9','C10'\
                                   'C11','C12','C13','C14','C15','C16','C17','C18','C19','C20'\
                                   'C21','C22','C23','C24','C25','C26','C27','C28','C29','C30'\
                                   'C31','C32','C33','C34','C35','C36','C37','C38','C39','C40'\
                                   'C41','C42','C43','C44','C45','C46','C47','C48','C49','C50'\
                                   'C51','C52','C53','C54','C55','C56','C57','C58','C59','C60'])
        
        # 개별 pik 파일로 저장하고 싶을 때..
        f = f.replace('LogMCCRaw', 'DiagMCCData')
        f = f.replace('\\V', '/Data/V')
        f = f.replace('.log', '.pklz')
        
        # 폴더 검색/생성
        if not os.path.isdir(f[:-18]):
            os.mkdir(f[:-18])
        
        dfn.DumpPickleFile(df, f, True)
        nCnt += 1
        print(str(nCnt) + r' / ' + str(len(files)))


# Event Type 별 Log 분리
def fileFilterByType():
    fp = r'D:/DiagMCCData/SEMES/Data/V*/MCC*.pklz'
    files = glob.glob(fp)
    nCnt = 0
    for f in files:
        df = dfn.LoadPickleFile(f, True)
        dfDrv = df[df['C3'] == 'DRV']
        dfTrans = df[df['C3'] == 'TRANS']
        dfStatus = df[df['C3'] == 'STS']
        
        ff = f[:-4] + '_Drive.pklz'
        dfn.DumpPickleFile(dfDrv, ff, True)
        ff = f[:-4] + '_Trans.pklz'
        dfn.DumpPickleFile(dfTrans, ff, True)
        ff = f[:-4] + '_Status.pklz'
        dfn.DumpPickleFile(dfStatus, ff, True)
        nCnt += 1
        print(str(nCnt) + r' / ' + str(len(files)))




# Log->Pickle 만들기..
def fileMakeCompress():
    fp = r'D:/DiagMCCData/SEMES/Data/V*/MCC*.pkl'
    files = glob.glob(fp)
    nCnt = 0
    for f in files:
        df = dfn.LoadPickleFile(f, False)
        f= f + 'z'
        dfn.DumpPickleFile(df, f, True)
        nCnt += 1
        print(str(nCnt) + r' / ' + str(len(files)))


# Log Dump 만들기..
def fileMake(strWhat):
    fp = 'D:/LogMCCRaw/P1L_7F/V*/MCC*.log'    
    files = glob.glob(fp)
    dfTrans = pd.DataFrame()    
    
    for f in files:    
        if(strWhat == 'Trans'):
            df = fileFilterTrans(f)
        if(strWhat == 'TransUnload'):
            df = fileFilterTransUnload(f)
        if(strWhat == 'Oscillation'):
            df = fileFilterTransOscillation(f)
        
        # 개별 pik 파일로 저장하고 싶을 때..
        #f = f.replace('LogMCCRaw', 'DiagMCCData')
        #f = f.replace('\\V', '/Data/V')
        #f = f.replace('.log', '.pkl')
        #DumpPickleFile(df, f, False)
        dfTrans = dfTrans.append(df, ignore_index=True)
        
    strDate = dt.datetime.now().strftime('%Y%m%d%H%M')        
    dfn.DumpPickleFile(dfTrans, r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + strWhat + '.pkl', False)
    
# MachineID 별로 Pikcle 생성 - 한번에 너무 많으면 Time-out
def fileMakeMachineID(strWhat, MachineID):
    fp = 'D:/LogMCCRaw/P1L_7F/V*/MCC*.log'        
    files = glob.glob(fp)
    dfTrans = pd.DataFrame()    
    
    bufMachineID = files[0][20:26]
    for f in files:    
        if(strWhat == 'Trans'):
            df = fileFilterTrans(f)
        if(strWhat == 'TransUnload'):
            df = fileFilterTransUnload(f)
        if(strWhat == 'Oscillation'):
            df = fileFilterTransOscillation(f)        
         
        # MachineID 별 pik 파일로 저장
        curMachineID = f[20:26]
        if(bufMachineID != curMachineID):
            dfTrans = pd.DataFrame()  # 호기 변경 시 초기화
            bufMachineID = curMachineID
            f = f.replace('LogMCCRaw', 'DiagMCCData')
            f = f.replace('\\V', '/Data/V')
            strDate = dt.datetime.now().strftime('%Y%m%d%H%M')        
            f = f.replace('.log', '_'+ strDate + '.pkl')
            dfn.DumpPickleFile(df, f, False) 
            print(f)
            
        dfTrans = dfTrans.append(df, ignore_index=True)
        
    # 마지막 파일/호기 저장
    f = f.replace('LogMCCRaw', 'DiagMCCData')
    f = f.replace('\\V', '/Data/V')
    strDate = dt.datetime.now().strftime('%Y%m%d%H%M')        
    f = f.replace('.log', '_'+ strDate + '.pkl')
    dfn.DumpPickleFile(df, f, False) 


# 하나 고르자        
strWhatPickle = 'Trans'
strWhatPickle = 'TransUnload'
strWhatPickle = 'Oscillation'

start = time.perf_counter()
fileMake()
print(time.perf_counter() - start)


"""
dfUnloadMCC - Unload 연관 이벤트 시간 추출하여 Table로 만든것
'ULD READY'
'ULD MOVE SHIFTROTATE'
'ULD MOVE DOWN HOIST FAST'
'ULD MOVE DOWN HOIST FAST SLOWDOWN'  < 2.1 Add
'ULD MOVE DOWN HOIST SLOW'
'ULD CHECK ARRIVE TEACHING POINT'
'ULD GRIP'
'ULD MOVE UP HOIST SLOW'
'ULD MOVE UP HOIST FAST'
'ULD MOVE UP HOIST FAST SLOWDOWN' <- 2.1 Add
'ULD MOVE UP HOIST HOME'
'ULD MOVE SHIFTROTATE HOME'
'ULD DONE' <- 2.1 Add

- V1.0 EQ
ULD READY
ULD MOVE SHIFTROTATE
ULD MOVE DOWN HOIST FAST
ULD MOVE DOWN HOIST SLOW
ULD CHECK ARRIVE TEACHING POINT
ULD GRIP
ULD MOVE UP HOIST SLOW
ULD MOVE UP HOIST FAST
ULD MOVE UP HOIST HOME  <-- EQ 만?
ULD MOVE SHIFTROTATE HOME

- V1.0 STB
ULD READY
ULD MOVE SHIFTROTATE
ULD MOVE DOWN HOIST FAST
ULD MOVE DOWN HOIST SLOW
ULD CHECK ARRIVE TEACHING POINT
ULD GRIP
ULD MOVE UP HOIST SLOW
ULD MOVE UP HOIST FAST
ULD MOVE SHIFTROTATE HOME


"""

###################################################################################
# dfUnloadMCC : Gantt 만들기 위해 시간만 이벤트 별도 MCC 용 Datafame 생성
# 추출 방식..
# dfTransUnload는 호기별 소팅되어 있음
# 호기별로 별도 이벤트를 추려야함 -> 정상인 동작만 분석하자..
# 처음(Ready 부터 시작 안함), 끝(종료 이벤트 아님-버전별 다름), 중간(이벤트 끊김)
# 


# 현재 버전별 이벤트 개수가 다르니.. 변환 전 개수부터 맞추고 가야함..!!
refEventAmount = {r'V2.1 EQ/STK':26, r'V1 EQ/STK':20, r'V2.1 STB':24, r'V1 STB':18} # Event ID개수 x 2(Start/End)
# 왜? column 이 순서대로 안만들어 지는지 모르겠음. --; 그래서 재정렬...
dfUnloadMCC = pd.DataFrame(columns={'E1S','E1E', 'E2S','E2E','E3S','E3E','E4S','E4E','E5S','E5E','E6S','E6E','E7S','E7E','E8S','E8E','E9S','E9E','E10S', 'E10E', 'E11S', 'E11E', 'E12S', 'E12E', 'E13S', 'E13E'})
dfUnloadMCC.columns = ['E1S','E1E', 'E2S','E2E','E3S','E3E','E4S','E4E','E5S','E5E','E6S','E6E','E7S','E7E','E8S','E8E','E9S','E9E','E10S', 'E10E', 'E11S', 'E11E', 'E12S', 'E12E', 'E13S', 'E13E']
#dfUnloadMCC = pd.DataFrame(columns={'E1S','E1E', 'E2S','E2E','E3S','E3E','E4S','E4E','E5S','E5E','E6S','E6E','E7S','E7E','E8S','E8E','E9S','E9E','E10S', 'E10E'})
#dfUnloadMCC.columns = ['E1S','E1E', 'E2S','E2E','E3S','E3E','E4S','E4E','E5S','E5E','E6S','E6E','E7S','E7E','E8S','E8E','E9S','E9E','E10S', 'E10E']


isFirst = False
ifrom = 0
ito=0
idx = 0 #새로운 dfUnloadMCC 에 gantt(MCC) Dataframe으로 만들기 위한 Row Index
endEventID = 'ULD DONE'
curMachineID = dfTransUnload.iloc[0]['Machine ID'] 
eventAmount = refEventAmount[r'V2.1 EQ/STK']
dfLen = len(dfTransUnload)
 # Method#1 - ULD READY Event 기준 잘라 넣기
for n in range(dfLen): # 다 뒤져보자.. ㅠㅠ
    if(dfTransUnload.iloc[n]['Event ID'] == 'ULD READY' and dfTransUnload.iloc[n]['StartEnd'] == 'Start'): # 시작으로 구분 Ready Start ~ 전까지
        if(isFirst == False): # 처음만남
            isFirst = True
            ifrom = n
        else:            
            if(dfTransUnload.iloc[n-1]['Event ID'] == endEventID and dfTransUnload.iloc[n-1]['StartEnd'] == 'End'):
                if((n-ifrom) == 26): # 정상적인 개수 Event 개수만 필터 - 중간에 끊긴것 제외
                    ito = n
                    lst = dfTransUnload['Time'][ifrom:ito].tolist()
                    dfUnloadMCC.loc[idx] = lst
                    idx += 1
                    print(str(n) + '/' + str(idx) + ': ' + str(round(n/dfLen*100,2)) + '%' )
                
            ifrom = n

          
# Method#2 - 고정 값으로 잘라내기 : Filter 된 DataFrame에 사용
eventAmount = 26 # EventID 26개 13*2
eventAmount = 20 # EventID 26개 13*2
loopcnt = len(dfTransUnload) // eventAmount # index 개수 (몫구하기)
for n in range(loopcnt):
    ifrom = n*eventAmount
    ito = ifrom + eventAmount
    lst = dfTransUnload['Time'][ifrom:ito].tolist()
    dfUnloadMCC.loc[n] = lst
    print(str(n) + '/' + str(loopcnt) + ': ' + str(round(n/loopcnt*100,2)) + '%')


############################################################################################
# dfPlot : Gantt 그리기 - datetime to matplottime
ftest = lambda x:matplotlib.dates.date2num(dt.datetime.fromisoformat(x))
dfPlot = dfUnloadMCC.applymap(ftest)

# V2.1 - 26개
eventLabel = ['ULD READY', 'ULD MOVE SHIFTROTATE', 'ULD MOVE DOWN HOIST FAST', 'ULD MOVE DOWN HOIST FAST SLOWDOWN', \
              'ULD MOVE DOWN HOIST SLOW','ULD CHECK ARRIVE TEACHING POINT', 'ULD GRIP',\
              'ULD MOVE UP HOIST SLOW', 'ULD MOVE UP HOIST FAST','ULD MOVE UP HOIST FAST SLOWDOWN',\
              'ULD MOVE UP HOIST HOME', 'ULD MOVE SHIFTROTATE HOME','ULD DONE']
# V1.0 EQ - 20개
eventLabel = ['ULD READY', 'ULD MOVE SHIFTROTATE', 'ULD MOVE DOWN HOIST FAST', 'ULD MOVE DOWN HOIST SLOW', \
              'ULD CHECK ARRIVE TEACHING POINT', 'ULD GRIP',\
              'ULD MOVE UP HOIST SLOW', 'ULD MOVE UP HOIST FAST', 'ULD MOVE UP HOIST HOME', 'ULD MOVE SHIFTROTATE HOME']

############################################################################################
#개별 Unload Gantt 그리기
myg.MyGanttChart(dfPlot.loc[14196], eventLabel) # V70803 STK S.192414 H:474mm S: -21mm
myg.MyGanttChart(dfPlot.loc[0], eventLabel)     # V70801 

# ULD READY Abnormal - 모두 처음 시작 시간 날짜가 잘못 변환되어 문제됨. Log 파일 처음에 이전 시간 Stamp 有
myg.MyGanttChart(dfPlot.loc[1161], eventLabel)
myg.MyGanttChart(dfPlot.loc[8148], eventLabel)
myg.MyGanttChart(dfPlot.loc[8678], eventLabel)

# ULD READY Abnormal - 시간이 길음
myg.MyGanttChart(dfPlot.loc[31], eventLabel)    # 3000>
myg.MyGanttChart(dfPlot.loc[2566], eventLabel)  # 0

# ULD MOVE SHIFTROTATE Abnormal - 시간이 길음
myg.MyGanttChart(dfPlot.loc[12228], eventLabel) # 250
myg.MyGanttChart(dfPlot.loc[3], eventLabel) # 250

# ULD CHECK ARRIVE TEACHING POINT Abnormal - 시간이 길음
myg.MyGanttChart(dfPlot.loc[39105], eventLabel) # 
myg.MyGanttChart(dfPlot.loc[3], eventLabel) # 250


# SEMES Unload 
myg.MyGanttChart(dfPlot.loc[0], eventLabel)
myg.MyGanttChart(dfPlot.loc[1], eventLabel)
myg.MyGanttChart(dfPlot.loc[2], eventLabel)
myg.MyGanttChart(dfPlot.loc[3], eventLabel)
myg.MyGanttChart(dfPlot.loc[4], eventLabel)
myg.MyGanttChart(dfPlot.loc[5], eventLabel)
myg.MyGanttChart(dfPlot.loc[6], eventLabel)

myg.MyGanttChart(dfPlot.loc[7], eventLabel)
myg.MyGanttChart(dfPlot.loc[12], eventLabel)
myg.MyGanttChart(dfPlot.loc[13], eventLabel)


'''
# 일단 수정 합시다.
datebuf = dfUnloadMCC.loc[1161, 'E1S']
dfUnloadMCC.loc[1161, 'E1S'] = datebuf.replace('-28','-27')
datebuf = dfUnloadMCC.loc[8148, 'E1S']
dfUnloadMCC.loc[8148, 'E1S'] = datebuf.replace('11-01','10-31')
datebuf = dfUnloadMCC.loc[8678, 'E1S']
dfUnloadMCC.loc[8678, 'E1S'] = datebuf.replace('-03','-02')
'''
#################################################################################################
# 각 Step 시간 추출
dfUnloadStepT = pd.DataFrame(columns=eventLabel)

for idx in range(len(eventLabel)):
    dfUnloadStepT[eventLabel[idx]] = dfUnloadMCC.apply(lambda x:(dt.datetime.strptime(x[2*idx+1], "%Y-%m-%d %H:%M:%S.%f") - dt.datetime.strptime(x[2*idx], "%Y-%m-%d %H:%M:%S.%f")), axis=1)

dfUnloadStepT = dfUnloadStepT.applymap(lambda x:round(x.total_seconds() * 1000))

# Abnormal Filter
dfStepT = dfUnloadStepT
dfStepTAbnormal = dfStepT[dfStepT['ULD READY'] < 0]
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD READY'] > (10 * 60 *1000)]) #10분
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE SHIFTROTATE'] < 0])
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE SHIFTROTATE'] > (10 * 60 *1000)]) #10분
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE DOWN HOIST FAST'] < 0])
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE DOWN HOIST FAST'] > (10 * 60 *1000)]) #10분
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE DOWN HOIST SLOW'] < 0])
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE DOWN HOIST SLOW'] > (10 * 60 *1000)]) #10분
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD CHECK ARRIVE TEACHING POINT'] < 0])
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD CHECK ARRIVE TEACHING POINT'] > (10 * 60 *1000)]) #10분
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD GRIP'] < 0])
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD GRIP'] > (10 * 60 *1000)]) #10분
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE UP HOIST SLOW'] < 0])
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE UP HOIST SLOW'] > (10 * 60 *1000)]) #10분
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE UP HOIST FAST'] < 0])
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE UP HOIST FAST'] > (10 * 60 *1000)]) #10분
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE UP HOIST HOME'] < 0])
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE UP HOIST HOME'] > (10 * 60 *1000)]) #10분
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE SHIFTROTATE HOME'] < 0])
dfStepTAbnormal = dfStepTAbnormal.append(dfStepT[dfStepT['ULD MOVE SHIFTROTATE HOME'] > (10 * 60 *1000)]) #10분
dfStepT = dfStepT.drop(dfStepTAbnormal.index)
dfUnloadStepT = dfStepT


# 정규분포 그려보기
# bufColumn 수동 변경해서 그려봄..
strDate = dt.datetime.now().strftime('%Y%m%d%H%M')
from scipy.stats import norm
for i in range(len(eventLabel)):
    bufColumn = eventLabel[i]
    if(i != 5): # Data가 모두 0이면 에러. 나중에 Validation 추가 필요
        ax = sns.distplot(dfUnloadStepT[bufColumn], fit=norm, kde=False)
        
        fname = r'D:/DiagMCCData/Figure/fig_' + strDate + '(' + bufColumn + ').png'
        figure = ax.get_figure()
        #figure.savefig(fname)
        plt.show()

def myDistPlot(df):
    sns.distplot(df, fit=norm, kde=False)

ax = sns.distplot(dfPlot, fit=norm, kde=False)

## 데이터 분석  ##########################################################
'''
ULD READY
ULD MOVE SHIFTROTATE
ULD MOVE DOWN HOIST FAST
ULD MOVE DOWN HOIST SLOW
ULD CHECK ARRIVE TEACHING POINT
ULD GRIP
ULD MOVE UP HOIST SLOW
ULD MOVE UP HOIST FAST
ULD MOVE UP HOIST HOME  <-- EQ 만?
ULD MOVE SHIFTROTATE HOME'''
# 'ULD MOVE UP HOIST FAST' Time 분포 확인
bufSTK = dfTrans.loc[376217:(376217+19)]
bufEQ = dfTrans.loc[0:19]

# 'ULD READY' Time 분포 확인
bufReady = dfUnloadStepT['ULD READY']
bufReadyAbnormal = bufReady[bufReady < 0]
bufReadyAbnormal = bufReady[bufReady > 3000]
bufReadyAbnormal0 = bufReady[bufReady == 0]
bufReady = bufReady.drop(index = bufReadyAbnormal.index) # 이상 데이터 정리
bufReadyFix = bufReady.drop(bufReadyAbnormal.index)

# 'ULD READY' Time 분포 확인
bufShiftRotate = dfUnloadStepT['ULD MOVE SHIFTROTATE']
bufShiftRotate.describe()
bufShiftRotateAbnormal = bufShiftRotate[bufShiftRotate > 100]
bufShiftRotateAbnormal250 = bufShiftRotate[bufShiftRotate == 250]
bufShiftRotateAbnormal0 = bufShiftRotate[bufShiftRotate == 15]

# 'ULD READY' 늦는 현상과 EQ PIO Time 연관 분석
bufPIOTime = dfTransUnload[dfTransUnload['StartEnd'] == 'End']
bufPIOTime = bufPIOTime[bufPIOTime['Event ID'] == 'ULD READY']
bufPIOTime.reset_index() #index 초기화
bufPIOTime = bufPIOTime.astype('int64')

# ULD CHECK ARRIVE TEACHING POINT
bufArrivedPos = dfUnloadStepT['ULD CHECK ARRIVE TEACHING POINT']
bufArrivedPosAbnormal = bufArrivedPos[bufArrivedPos > 0]
bufArrivedPosAbnormal1 = bufArrivedPos[bufArrivedPos == 1516]
bufArrivedPosAbnormal2 = bufArrivedPos[bufArrivedPos < 0]
bufArrivedPosFix = bufArrivedPos.drop(bufArrivedPosAbnormal.index)

# 호기별 분리해 보자 V70801~V70810
dfTransULD_V70801 = dfTransUnload[dfTransUnload['Machine ID'] == 'V70801']
dfTransULD_V70802 = dfTransUnload[dfTransUnload['Machine ID'] == 'V70802']
dfTransULD_V70803 = dfTransUnload[dfTransUnload['Machine ID'] == 'V70803']
dfTransULD_V70804 = dfTransUnload[dfTransUnload['Machine ID'] == 'V70804']
dfTransULD_V70805 = dfTransUnload[dfTransUnload['Machine ID'] == 'V70805']
dfTransULD_V70806 = dfTransUnload[dfTransUnload['Machine ID'] == 'V70806']
dfTransULD_V70807 = dfTransUnload[dfTransUnload['Machine ID'] == 'V70807']
dfTransULD_V70808 = dfTransUnload[dfTransUnload['Machine ID'] == 'V70808']
dfTransULD_V70809 = dfTransUnload[dfTransUnload['Machine ID'] == 'V70809']
dfTransULD_V70810 = dfTransUnload[dfTransUnload['Machine ID'] == 'V70810']

dfStepT_V70801 = dfUnloadStepT.loc[dfTransULD_V70801.index]


##############################################################################
##                      Key Value 뽑기~~~
def findKeyValue(df1, eventID1, startEnd1, KeyName1, dataType1):
    df1 = df1[df1['Event ID'] == eventID1]
    df1 = df1[df1['StartEnd'] == startEnd1]
    df1 = df1[KeyName1]
    
    df1 = df1.astype(dataType1)
    
    return df1

# Slide 위치 오차 0~0.1 or -0.1~0.1
dfSlideErr = findKeyValue(dfTransUnload, 'ULD MOVE SHIFTROTATE', 'End', 'Data Value 3', 'float')
dfSlideErr2 = findKeyValue(dfTransUnload, 'ULD MOVE SHIFTROTATE HOME', 'End', 'Data Value 3', 'float')

# Hoist Push Retry Count -> 0~1mm
dfPushRetry = findKeyValue(dfTransUnload, 'ULD CHECK ARRIVE TEACHING POINT', 'End', 'Data Value 3', 'int')

# Push On Pos - 모두 0 ??
#dfPushOnPos1 = findKeyValue(dfTransUnload, 'ULD MOVE DOWN HOIST SLOW', 'End', 'Value1', 'float')
dfPushOnPos2 = findKeyValue(dfTransUnload, 'ULD CHECK ARRIVE TEACHING POINT', 'End', 'Value3', 'float')
dfPushOnPos3 = findKeyValue(dfTransUnloadEQ, 'ULD CHECK ARRIVE TEACHING POINT', 'End', 'Value3', 'float')
dfPushOnPos4 = findKeyValue(dfTransUnloadSTK, 'ULD CHECK ARRIVE TEACHING POINT', 'End', 'Value3', 'float')
dfPushOnPos5 = findKeyValue(dfTransUnloadLSTB, 'ULD CHECK ARRIVE TEACHING POINT', 'End', 'Value3', 'float')
dfPushOnPos6 = findKeyValue(dfTransUnloadRSTB, 'ULD CHECK ARRIVE TEACHING POINT', 'End', 'Value3', 'float')

dfHoistTCPos = findKeyValue(dfTransUnloadEQ, 'ULD MOVE DOWN HOIST SLOW', 'Start', 'Value5', 'float')

#Hoist Position
dfHoistPosGrip = findKeyValue(dfTransUnloadEQ, 'ULD GRIP', 'End', 'Value1', 'float')



#############################################################################################################
# 정상적으로 전체 이벤트 로그 안남은 항목들 정리하자..
# 원본에서 Drop 하는 방식
# df - 대상 Dataframe
# refEventNameStart - 시작 Event Name
# refEventNameEnd   - 종료 Event Name
# cnt               - 정상 이벤트 개수 
def FilterByEventNameAndCnt(df, startEventID, endEventID, cnt):
#    dfRet = pd.DataFrame()
#    dfRet = dfRet.append(df.columns.to_series(), ignore_index=True)
#    dfRet = dfRet.drop([0])
    dfRet = df
    isFirst = False
    ifrom = 0
    dfLen = len(df)
    lst = df.index.tolist()
     
    for n in range(dfLen): # 다 뒤져보자.. ㅠㅠ
        if(df.iloc[n]['Event ID'] == startEventID and df.iloc[n]['StartEnd'] == 'Start'): # 시작으로 구분 Ready Start ~ 전까지
            if(isFirst == False): # 처음만남                                
                if(n != 0): #처음 불필요 이벤트 삭제
                    dfRet = dfRet.drop(lst[ifrom:n])
                isFirst = True
                ifrom = n
            else:            
                if(df.iloc[n-1]['Event ID'] == endEventID and df.iloc[n-1]['StartEnd'] == 'End'):
                    if((n-ifrom) == cnt): # 정상
                        iform = n #Dummy
                        #print('OK ' + str(ifrom) + r'~' + str(n) + ': ' + str(round(n/dfLen*100,1)) + '%' )
                    else: # 개수 이상
                        dfRet = dfRet.drop(lst[ifrom:n])
                        print('NG Cnt' + str(ifrom) + r'~' + str(n) + ': ' + str(round(n/dfLen*100,1)) + '%' )
                else: # Event ID 이상
                    dfRet = dfRet.drop(lst[ifrom:n])                        
                    print('NG Event' + df.iloc[n-1]['Event ID'] + ' ' + df.iloc[n-1]['StartEnd'] + \
                          str(ifrom) + r'~' + str(n) + ': ' + str(round(n/dfLen*100,1)) + '%' )                    
                ifrom = n
        
        if(n%10000 == 0):
            print('Progress ' + str(n) + r'/' + str(dfLen) + ': ' + str(round(n/dfLen*100,1)) + '%' )
            
    return dfRet

dfTransUnloadEQ_Filtered = FilterByEventNameAndCnt(dfTransUnloadEQ, 'ULD READY', 'ULD MOVE SHIFTROTATE HOME', 20)
dfTransUnloadSTK_Filtered = FilterByEventNameAndCnt(dfTransUnloadSTK, 'ULD READY', 'ULD MOVE SHIFTROTATE HOME', 20)
dfTransUnloadSTB_Filtered = FilterByEventNameAndCnt(dfTransUnloadSTB, 'ULD READY', 'ULD MOVE SHIFTROTATE HOME', 18)
dfTransUnload_Filtered = dfTransUnloadEQ_Filtered
dfTransUnload_Filtered = dfTransUnload_Filtered.append(dfTransUnloadSTK_Filtered)
dfTransUnload_Filtered = dfTransUnload_Filtered.append(dfTransUnloadSTB_Filtered)

dfTransUnloadEQ_Abnormal = dfTransUnloadEQ.drop(dfTransUnloadEQ_Filtered.index)
dfTransUnloadSTK_Abnormal = dfTransUnloadSTK.drop(dfTransUnloadSTK_Filtered.index)
dfTransUnloadSTB_Abnormal = dfTransUnloadSTB.drop(dfTransUnloadSTB_Filtered.index)

##############################################################################
##                      Pickle Load

# Pickle Load..!!
f = r'D:/DiagMCCData/P1L_7F/Data/'
dfTransV70801 = dfn.LoadPickleFile(f + '202001301432_V70801_Trans.pklz', True)
dfTransV70802 = dfn.LoadPickleFile(f + '202001301444_V70802_Trans.pklz', True)
dfTransV70803 = dfn.LoadPickleFile(f + '202001301452_V70803_Trans.pklz', True)
dfTransV70804 = dfn.LoadPickleFile(f + '202001301503_V70804_Trans.pklz', True)
dfTransV70805 = dfn.LoadPickleFile(f + '202001301513_V70805_Trans.pklz', True)
dfTransV70806 = dfn.LoadPickleFile(f + '202001301520_V70806_Trans.pklz', True)
dfTransV70807 = dfn.LoadPickleFile(f + '202001301528_V70807_Trans.pklz', True)
dfTransV70808 = dfn.LoadPickleFile(f + '202001301533_V70808_Trans.pklz', True)
dfTransV70809 = dfn.LoadPickleFile(f + '202001301543_V70809_Trans.pklz', True)
dfTransV70810 = dfn.LoadPickleFile(f + '202001301552_V70810_Trans.pklz', True)

### 호기 Trans 데이터 프레임 정리 - 컬럼 이름 및 불필요 컬럼 삭제
df = dfTransV70810
lst = df.columns.to_list()
df = df.drop(lst[34:], axis=1) #불필요 컬럼 삭제
df = df.rename(columns={'Event ID':'Dummy1', 'Status':'Dummy2'})
df = df.rename(columns={'OHT':'Machine ID', 'Box type':'Event ID', 'Station ID':'StartEnd'})
df = df.rename(columns={'EQ':'Material', 'Node ID':'Station ID', 'Offset':'Port Type'})
df = df.rename(columns={'Data Key 1':'Node ID', 'Data Value 1':'Offset'})
df = df.rename(columns={'Data Key 2':'Key1', 'Data Value 2':'Value1',\
                        'Data Key 3':'Key2', 'Data Value 3':'Value2',\
                        'Data Key 4':'Key3', 'Data Value 4':'Value3',\
                        'Data Key 5':'Key4', 'Data Value 5':'Value4',\
                        'Data Key 6':'Key5', 'Data Value 6':'Value5',\
                        'Data Key 7':'Key6', 'Data Value 7':'Value6',\
                        'Data Key 8':'Key7', 'Data Value 8':'Value7',\
                        'Data Key 9':'Key8', 'Data Value 9':'Value8',\
                        'Data Key 10':'Key9', 'Data Value 10':'Value9',\
                        'Data Key 11':'Key10', 'Data Value 11':'Value10'})
df = df.rename(columns={'c34':'Key11', 'c35':'Value11'})
dfTransV70810 = df.drop(['Dummy1', 'Dummy2'], axis=1)

## Column Name 정리된 Trans Pickle
f = r'D:/DiagMCCData/P1L_7F/'
dfTransV70801 = dfn.LoadPickleFile(f + '202002032149_dfTransV70801.pkl', False)
# StartEnd 중 Start or End 가 아닌것 제거
df = dfTransV70801[dfTransV70801['StartEnd'] != 'Start']
df = df[df['StartEnd'] != 'End']
dfTransV70801 = dfTransV70801.drop(df.index)

dfTransV70801_HandOpen = dfTransV70801[dfTransV70801['Event ID'] == 'HAND SEQUENCE OPEN']

dfTransV70801_HandClose = dfTransV70801[dfTransV70801['Event ID'] == 'HAND SEQUENCE CLOSE']
dfTransV70802_HandClose = dfTransV70802[dfTransV70802['Event ID'] == 'HAND SEQUENCE CLOSE']
dfTransV70803_HandClose = dfTransV70803[dfTransV70803['Event ID'] == 'HAND SEQUENCE CLOSE']
dfTransV70804_HandClose = dfTransV70804[dfTransV70804['Event ID'] == 'HAND SEQUENCE CLOSE']
dfTransV70805_HandClose = dfTransV70805[dfTransV70805['Event ID'] == 'HAND SEQUENCE CLOSE']
dfTransV70806_HandClose = dfTransV70806[dfTransV70806['Event ID'] == 'HAND SEQUENCE CLOSE']
dfTransV70807_HandClose = dfTransV70807[dfTransV70807['Event ID'] == 'HAND SEQUENCE CLOSE']
dfTransV70808_HandClose = dfTransV70808[dfTransV70808['Event ID'] == 'HAND SEQUENCE CLOSE']
dfTransV70809_HandClose = dfTransV70809[dfTransV70809['Event ID'] == 'HAND SEQUENCE CLOSE']
dfTransV70810_HandClose = dfTransV70810[dfTransV70810['Event ID'] == 'HAND SEQUENCE CLOSE']

dfTransHandClose = dfTransV70801_HandClose.append(dfTransV70802_HandClose)
dfTransHandClose = dfTransHandClose.append(dfTransV70803_HandClose)
dfTransHandClose = dfTransHandClose.append(dfTransV70804_HandClose)
dfTransHandClose = dfTransHandClose.append(dfTransV70805_HandClose)
dfTransHandClose = dfTransHandClose.append(dfTransV70806_HandClose)
dfTransHandClose = dfTransHandClose.append(dfTransV70807_HandClose)
dfTransHandClose = dfTransHandClose.append(dfTransV70808_HandClose)
dfTransHandClose = dfTransHandClose.append(dfTransV70809_HandClose)
dfTransHandClose = dfTransHandClose.append(dfTransV70810_HandClose)
dfTransHandClose['Value11'] = dfTransHandClose['Value11'].astype('float')
dfTransHandClose['Station ID'] = dfTransHandClose['Station ID'].astype('int')

dfTransHandCloseAbnormal = dfTransHandClose[dfTransHandClose['Value11'] > 1]

f = r'D:/DiagMCCData/P1L_7F/Data/'
dfDriveV70801 = dfn.LoadPickleFile(f + '202001311002_V70801_Drive.pklz', True)
dfDriveV70802 = dfn.LoadPickleFile(f + '202001311053_V70802_Drive.pklz', True)
dfDriveV70803 = dfn.LoadPickleFile(f + '202001311130_V70803_Drive.pklz', True)
dfDriveV70804 = dfn.LoadPickleFile(f + '202001311230_V70804_Drive.pklz', True)
dfDriveV70805 = dfn.LoadPickleFile(f + '202001311317_V70805_Drive.pklz', True)
dfDriveV70806 = dfn.LoadPickleFile(f + '202001311350_V70806_Drive.pklz', True)
dfDriveV70807 = dfn.LoadPickleFile(f + '202001311425_V70807_Drive.pklz', True)
dfDriveV70808 = dfn.LoadPickleFile(f + '202001311445_V70808_Drive.pklz', True)
dfDriveV70809 = dfn.LoadPickleFile(f + '202001311534_V70809_Drive.pklz', True)
dfDriveV70810 = dfn.LoadPickleFile(f + '202001311617_V70810_Drive.pklz', True)

dfDriveV70801NodeDetect = dfDriveV70801[dfDriveV70801['Event ID'] == 'NODE DETECT']
dfDriveV70802NodeDetect = dfDriveV70802[dfDriveV70802['Event ID'] == 'NODE DETECT']
dfDriveV70803NodeDetect = dfDriveV70803[dfDriveV70803['Event ID'] == 'NODE DETECT']
dfDriveV70804NodeDetect = dfDriveV70804[dfDriveV70804['Event ID'] == 'NODE DETECT']
dfDriveV70805NodeDetect = dfDriveV70805[dfDriveV70805['Event ID'] == 'NODE DETECT']
dfDriveV70806NodeDetect = dfDriveV70806[dfDriveV70806['Event ID'] == 'NODE DETECT']
dfDriveV70807NodeDetect = dfDriveV70807[dfDriveV70807['Event ID'] == 'NODE DETECT']
dfDriveV70808NodeDetect = dfDriveV70808[dfDriveV70808['Event ID'] == 'NODE DETECT']
dfDriveV70809NodeDetect = dfDriveV70809[dfDriveV70809['Event ID'] == 'NODE DETECT']
dfDriveV70810NodeDetect = dfDriveV70810[dfDriveV70810['Event ID'] == 'NODE DETECT']

dfDriveV70801DetectNStop = dfDriveV70801[dfDriveV70801['Event ID'] == 'OHT DETECT AND STOP']
dfDriveV70802DetectNStop = dfDriveV70802[dfDriveV70802['Event ID'] == 'OHT DETECT AND STOP']
dfDriveV70803DetectNStop = dfDriveV70803[dfDriveV70803['Event ID'] == 'OHT DETECT AND STOP']
dfDriveV70804DetectNStop = dfDriveV70804[dfDriveV70804['Event ID'] == 'OHT DETECT AND STOP']
dfDriveV70805DetectNStop = dfDriveV70805[dfDriveV70805['Event ID'] == 'OHT DETECT AND STOP']
dfDriveV70806DetectNStop = dfDriveV70806[dfDriveV70806['Event ID'] == 'OHT DETECT AND STOP']
dfDriveV70807DetectNStop = dfDriveV70807[dfDriveV70807['Event ID'] == 'OHT DETECT AND STOP']
dfDriveV70808DetectNStop = dfDriveV70808[dfDriveV70808['Event ID'] == 'OHT DETECT AND STOP']
dfDriveV70809DetectNStop = dfDriveV70809[dfDriveV70809['Event ID'] == 'OHT DETECT AND STOP']
dfDriveV70810DetectNStop = dfDriveV70810[dfDriveV70810['Event ID'] == 'OHT DETECT AND STOP']

dfDriveNodeDetect = dfDriveV70801NodeDetect
dfDriveNodeDetect = dfDriveNodeDetect.append(dfDriveV70802NodeDetect)
dfDriveNodeDetect = dfDriveNodeDetect.append(dfDriveV70803NodeDetect)
dfDriveNodeDetect = dfDriveNodeDetect.append(dfDriveV70804NodeDetect)
dfDriveNodeDetect = dfDriveNodeDetect.append(dfDriveV70805NodeDetect)
dfDriveNodeDetect = dfDriveNodeDetect.append(dfDriveV70806NodeDetect)
dfDriveNodeDetect = dfDriveNodeDetect.append(dfDriveV70807NodeDetect)
dfDriveNodeDetect = dfDriveNodeDetect.append(dfDriveV70808NodeDetect)
dfDriveNodeDetect = dfDriveNodeDetect.append(dfDriveV70809NodeDetect)
dfDriveNodeDetect = dfDriveNodeDetect.append(dfDriveV70810NodeDetect)

dfDriveNodeDetect = dfDriveNodeDetect.reset_index() # append 후 Index 초기화(고유 Index 갖기 위해)
dfDriveNodeDetectAbnormal = dfDriveNodeDetect[dfDriveNodeDetect['Key4'] != 'Compensation']
dfDriveNodeDetectFiltered = dfDriveNodeDetect.drop(dfDriveNodeDetectAbnormal.index)
dfDriveNodeDetectFiltered['Value4'] = dfDriveNodeDetectFiltered['Value4'].astype('float')


dfDriveDetectNStop = dfDriveV70801DetectNStop
dfDriveDetectNStop = dfDriveDetectNStop.append(dfDriveV70802DetectNStop)
dfDriveDetectNStop = dfDriveDetectNStop.append(dfDriveV70803DetectNStop)
dfDriveDetectNStop = dfDriveDetectNStop.append(dfDriveV70804DetectNStop)
dfDriveDetectNStop = dfDriveDetectNStop.append(dfDriveV70805DetectNStop)
dfDriveDetectNStop = dfDriveDetectNStop.append(dfDriveV70806DetectNStop)
dfDriveDetectNStop = dfDriveDetectNStop.append(dfDriveV70807DetectNStop)
dfDriveDetectNStop = dfDriveDetectNStop.append(dfDriveV70808DetectNStop)
dfDriveDetectNStop = dfDriveDetectNStop.append(dfDriveV70809DetectNStop)
dfDriveDetectNStop = dfDriveDetectNStop.append(dfDriveV70810DetectNStop)

dfDriveDetectNStop = dfDriveDetectNStop.reset_index() # append 후 Index 초기화(고유 Index 갖기 위해)
df = dfDriveDetectNStop
df = dfDriveDetectNStop[dfDriveDetectNStop['StartEnd'] == 'Start']
dfDriveDetectNStopAbnormal = df[df['Key2'] != 'OBS Detect Status']

df = dfDriveDetectNStop[dfDriveDetectNStop['StartEnd'] == 'End']
dfDriveDetectNStopAbnormal = df[df['Key1'] != 'Detect Stop Time']

dfDriveDetectNStopFiltered = dfDriveDetectNStop.drop(dfDriveDetectNStopAbnormal.index)
dfDriveDetectNStopFiltered['Value4'] = dfDriveDetectNStopFiltered['Value4'].astype('float')


df = dfDriveDetectNStop
lst = df.columns.to_list()
df = df.drop(lst[15:], axis=1) #불필요 컬럼 삭제

df = df.rename(columns={'Node ID':'Station'})
df = df.rename(columns={'OHT':'Machine ID', 'Status':'StartEnd', 'Box type':'Material',\
                        'Station ID':'Node ID', 'EQ':'Target',\
                        'Data Key 1':'Velocity', 'Data Value 1':'Key1',\
                        'Data Key 2':'Value1', 'Data Value 2':'Key2', 'Data Key 3':'Value2'})

    
strDate = dt.datetime.now().strftime('%Y%m%d%H%M')
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfDriveDetectNStop.pkl'
dfn.DumpPickleFile(dfDriveDetectNStop, f , False)

f = r'D:/DiagMCCData/P1L_7F/'
dfDriveNodeDetect = dfn.LoadPickleFile(f + '202002121442_dfDriveDetectNStop.pkl', False)




dfStatusV70801 = dfn.LoadPickleFile(f + '202001311831_V70801_Status.pklz', True)
dfStatusV70802 = dfn.LoadPickleFile(f + '202001311855_V70802_Status.pklz', True)
dfStatusV70803 = dfn.LoadPickleFile(f + '202001311914_V70803_Status.pklz', True)
dfStatusV70804 = dfn.LoadPickleFile(f + '202001311944_V70804_Status.pklz', True)
dfStatusV70805 = dfn.LoadPickleFile(f + '202001312007_V70805_Status.pklz', True)
dfStatusV70806 = dfn.LoadPickleFile(f + '202001312024_V70806_Status.pklz', True)
dfStatusV70807 = dfn.LoadPickleFile(f + '202001312041_V70807_Status.pklz', True)
dfStatusV70808 = dfn.LoadPickleFile(f + '202001312052_V70808_Status.pklz', True)
dfStatusV70809 = dfn.LoadPickleFile(f + '202001312117_V70809_Status.pklz', True)
dfStatusV70810 = dfn.LoadPickleFile(f + '202001312139_V70810_Status.pklz', True)


f = r'D:/DiagMCCData/P1L_7F/'
dfTransUnload = dfn.LoadPickleFile(f + '202002011114_dfTransUnload.pkl', False)
dfTransUnloadEQ = dfTransUnload[dfTransUnload['Port Type'] == 1]
dfTransUnloadSTK = dfTransUnload[dfTransUnload['Port Type'] == 3]
dfTransUnloadLSTB = dfTransUnload[dfTransUnload['Port Type'] == 5]
dfTransUnloadRSTB = dfTransUnload[dfTransUnload['Port Type'] == 6]
dfTransUnloadSTB = dfTransUnload[dfTransUnload['Port Type'] > 4] # 5 and 6
dfTransUnloadAbnormal = dfTransUnload.drop(dfTransUnloadEQ.index)
dfTransUnloadAbnormal = dfTransUnloadAbnormal.drop(dfTransUnloadSTK.index)
dfTransUnloadAbnormal = dfTransUnloadAbnormal.drop(dfTransUnloadSTB.index)
dfTransUnload = dfTransUnload.drop(dfTransUnloadAbnormal.index)
len(dfTransUnloadEQ) + len(dfTransUnloadSTK) + len(dfTransUnloadLSTB) + len(dfTransUnloadRSTB) + len(dfTransUnloadAbnormal)
# ==========> 이미 만든거 Load
f = r'D:/DiagMCCData/P1L_7F/'
dfTransUnload_Filtered = dfn.LoadPickleFile(f + '202002011719dfTransUnload_Filtered.pkl', False)
dfTransUnloadEQ_Filtered = dfn.LoadPickleFile(f + '202002011719dfTransUnloadEQ_Filtered.pkl', False)
dfTransUnloadSTK_Filtered = dfn.LoadPickleFile(f + '202002011719dfTransUnloadSTB_Filtered.pkl', False)
dfTransUnloadSTB_Filtered = dfn.LoadPickleFile(f + '202002011719dfTransUnloadSTK_Filtered.pkl', False)

dfTransUnloadEQ_Abnormal = dfn.LoadPickleFile(f + '202002011719dfTransUnloadEQ_Abnormal.pkl', False)
dfTransUnloadSTK_Abnormal = dfn.LoadPickleFile(f + '202002011719dfTransUnloadSTB_Abnormal.pkl', False)
dfTransUnloadSTB_Abnormal = dfn.LoadPickleFile(f + '202002011719dfTransUnloadSTK_Abnormal.pkl', False)

dfUnloadMCC = dfn.LoadPickleFile(f + '202002031103_dfUnloadMCC.pkl', False)
dfUnloadMCC_EQ = dfn.LoadPickleFile(f + '202002032042_dfUnloadMCC_EQ.pkl', False)
dfUnloadMCC_STK = dfn.LoadPickleFile(f + '202002032039_dfUnloadMCC_STK.pkl', False)
dfUnloadMCC_STB = dfn.LoadPickleFile(f + '202002031103_dfUnloadMCC.pkl', False)

dfPlot = dfn.LoadPickleFile(f + '202002031103_dfPlot.pkl', False)
dfUnloadStepT = dfn.LoadPickleFile(f + '202001292150_dfUnloadStepT_20_Fix.pkl', False)



# 20개 열만 추출
dfTransUnload = dfTransUnload.iloc[:,0:20]
dfTransUnload = dfTransUnload.rename(columns={'Data Key 2':'Key1', 'Data Value 2':'Value1',\
                                              'Data Key 3':'Key2', 'Data Value 3':'Value2',\
                                              'Data Key 4':'Key3', 'Data Value 4':'Value3',\
                                              'Data Key 5':'Key4', 'Data Value 5':'Value4',\
                                              'Data Key 6':'Key5', 'Data Value 6':'Value5'})
dfTransUnload = dfTransUnload.rename(columns={'time':'Time', 'MachineID':'Machine ID', 'PortType':'Port Type'})
dfTrans = dfTrans.rename(columns={'Event ID':'Dummy1', 'Status':'Dummy2'})
dfTrans = dfTrans.rename(columns={'OHT':'MachineID', 'Time':'time', 'Box type':'Event ID'})
dfTrans = dfTrans.drop(['Dummy1', 'Dummy2'], axis=1) # 불필요 열 삭제

##############################################################################
#                       Pickle Dump
# dfTrans
strDate = dt.datetime.now().strftime('%Y%m%d%H%M')
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfTransV70801.pkl'
dfn.DumpPickleFile(dfTransV70801, f, False) # 정리된 pickle

# dfTransUnload
strDate = dt.datetime.now().strftime('%Y%m%d%H%M')
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_dfTransUnload.pkl'
dfn.DumpPickleFile(dfTransUnload, f, False) # 정리된 pickle

# dfTransUnload에서 Filter(PortType & Event 개수) Dataframe들..
strDate = dt.datetime.now().strftime('%Y%m%d%H%M')
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfTransUnloadEQ_Filtered.pkl'
dfn.DumpPickleFile(dfTransUnloadEQ_Filtered, f, False) 
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfTransUnloadSTK_Filtered.pkl'
dfn.DumpPickleFile(dfTransUnloadSTK_Filtered, f, False)
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfTransUnloadSTB_Filtered.pkl'
dfn.DumpPickleFile(dfTransUnloadSTB_Filtered, f, False)
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfTransUnload_Filtered.pkl'
dfn.DumpPickleFile(dfTransUnload_Filtered, f, False)

strDate = dt.datetime.now().strftime('%Y%m%d%H%M')
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfTransUnloadEQ_Abnormal.pkl'
dfn.DumpPickleFile(dfTransUnloadEQ_Abnormal, f, False) 
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfTransUnloadSTK_Abnormal.pkl'
dfn.DumpPickleFile(dfTransUnloadSTK_Abnormal, f, False)
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfTransUnloadSTB_Abnormal.pkl'
dfn.DumpPickleFile(dfTransUnloadSTB_Abnormal, f, False)

# Hand Close
strDate = dt.datetime.now().strftime('%Y%m%d%H%M')
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfTransHandClose.pkl'
dfn.DumpPickleFile(dfTransHandClose, f, False) 
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfTransHandCloseFilteredEnd.pkl'
dfn.DumpPickleFile(dfTransHandCloseFilteredEnd, f, False) 
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfTransHandCloseAbnormal.pkl'
dfn.DumpPickleFile(dfTransHandCloseAbnormal, f, False) 


# dfUnloadMCC - EQ/STK Event 모음
strDate = dt.datetime.now().strftime('%Y%m%d%H%M')
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_dfUnloadMCC.pkl'
dfn.DumpPickleFile(dfUnloadMCC, f, False)
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_dfUnloadMCC_EQ.pkl'
dfn.DumpPickleFile(dfUnloadMCC_EQ, f, False)
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_dfUnloadMCC_STK.pkl'
dfn.DumpPickleFile(dfUnloadMCC_STK, f, False)
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_dfUnloadMCC_STB.pkl'
dfn.DumpPickleFile(dfUnloadMCC_STB, f, False)

# dfPlot - Gantt 차트
strDate = dt.datetime.now().strftime('%Y%m%d%H%M')
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_dfPlot.pkl'
dfn.DumpPickleFile(dfPlot, f, False)

# dfUnloadStepT - 정규분포 
strDate = dt.datetime.now().strftime('%Y%m%d%H%M')
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_dfUnloadStepT.pkl'
dfn.DumpPickleFile(dfUnloadStepT, f, False)

# dfDriveNodeDetect
strDate = dt.datetime.now().strftime('%Y%m%d%H%M')
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfDriveNodeDetect.pkl'
dfn.DumpPickleFile(dfDriveNodeDetect, f, False)

# dfDriveNodeDetectFiltered
strDate = dt.datetime.now().strftime('%Y%m%d%H%M')
f = r'D:/DiagMCCData/P1L_7F/' + strDate + '_' + 'dfDriveNodeDetectFiltered.pkl'
dfn.DumpPickleFile(dfDriveNodeDetectFiltered, f, False)

