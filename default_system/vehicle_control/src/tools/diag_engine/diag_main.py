import sys
import time
import os
import glob
import datetime as dt
import numpy as np
import pandas as pd
import pickle
import argparse
import logging
from xlsxwriter import Workbook

sys.path.append(r'C:/MCCDiag/python_package')
import diag_functions as dfn
import diag_dbms as ddb
import diag_plot as dplot


def set_logger():
    '''Set logger parameter    

    Returns
    -------
    None.

    '''
    # Check & Create Site Folder
    fdir='C:\MCCDiag\python_package\log'
    if not os.path.isdir(fdir):    
        os.mkdir(fdir)
    
    # Remove logger handle 
    handlers = logger.handlers[:]
    for handler in handlers:        
        handler.close()
        logger.removeHandler(handler)
    
    # create logger with module name    
    logger.setLevel(logging.INFO)
    # create file handler which logs even debug messages
    fh = logging.FileHandler(r'C:/MCCDiag/python_package/log/'+analysis_time_log+'_diag_mcc.log', mode='w')
    fh.setLevel(logging.INFO)
    # create console handler with a higher log level
    ch = logging.StreamHandler()
    ch.setLevel(logging.INFO)
    # create formatter and add it to the handlers
    formatter = logging.Formatter('%(asctime)s [%(levelname)s][%(name)s] %(message)s')
    fh.setFormatter(formatter)
    ch.setFormatter(formatter)
    # add the handlers to the logger
    logger.addHandler(fh)
    logger.addHandler(ch)


def set_local_args():  
    ret=[]    
    ret.append('diag_main.py')  # No meaning
    ret.append('P1L_3F')         # site
    ret.append('P1L_3F_Sample')   # site group
    ret.append('2020072807')    # from
    ret.append('2020072807')    # to
    ret.append('ALL') # ALL or VHL List    

    ret=[]    
    ret.append('diag_main.py')  # No meaning
    ret.append('P1L_7F')         # site
    ret.append('P1L_7F_Sample')   # site group
    ret.append('2020080400')    # from
    ret.append('2020083123')    # to
    ret.append('ALL') # ALL or VHL List    

    ret=[]    
    ret.append('diag_main.py')  # No meaning
    ret.append('V1L_10F')       # site
    ret.append('V1L_10F')       # site group
    ret.append('2020091100')    # from
    ret.append('2020091102')    # to
    ret.append('ALL') # ALL or VHL List    

    ret=[]    
    ret.append('diag_main.py')  # No meaning
    ret.append('SEMES')         # site
    ret.append('SEMES_2F')      # site group
    ret.append('2020091910')    # from
    ret.append('2020091910')    # to
    ret.append('ALL') # ALL or VHL List

    for a, i in zip(ret, range(len(ret))):
        logger.info('test arg[%d] %s', i, a)

    return ret

def check_args():    
    '''Check validations of arguments
       if there is no argument, set testing argument

    Returns
    -------
    lst : TYPE list
        DESCRIPTION.
        Proven arguments
    '''
    lst_args=[]
    if(len(sys.argv) > 1):    
        # 인자 개수 체크
        for a, i in zip(sys.argv, range(len(sys.argv))):
            lst_args.append(a)
            logger.info('arg[%d] %s', i, a)
        
        # python  %module% %site% %group% %from_datetime% %to_datetime% %lst_vhl%
        if(len(sys.argv) != 6):
            #time.sleep(1)
            logger.error('Args count(6) missmatch Error: %d', len(sys.argv))
            sys.exit()
        
        # arg[3], arg[4]
        if(sys.argv[3].isdecimal() == False or sys.argv[4].isdecimal() == False):
            logger.error('datetime format is invalid..!! check format[yyyymmddhh]')
            sys.exit()
        
        if(len(sys.argv[3]) != 10 or len(sys.argv[4]) != 10):
            logger.error('datetime length is not 10. check format[yyyymmddhh]')
            sys.exit()
    else:#Testing - no argv
        lst_args = set_local_args()
        
    return lst_args

def set_option():
    '''Set options
    raw data save - True or False
    plot filter - True or False
    dump pickle - True or False    

    Returns
    -------
    None.

    '''
    dic_opt={'rawData save':False,
             'plot filter':True,
             'plot use':True,
             'MCC Ver':2.0,
             'dump pickle':True,
             'Ref Refresh':False, # Reference data refresh
             'NotUse':0}
    
    if(ArgSite=='SEMES'):
        dic_opt['plot filter']=False
        dic_opt['rawData save']=True
    
    logger.info('option =========')
    for k,v in dic_opt.items():
        logger.info('%s - %s', k, v)
        
    return dic_opt


def log_summary():
    dfTransEQ=dfRawT[(dfRawT['PortType']==1)]
    dfTransSTK=dfRawT[(dfRawT['PortType']==3)]
    dfTransLSTB=dfRawT[(dfRawT['PortType']==5)]
    dfTransRSTB=dfRawT[(dfRawT['PortType']==6)]
        
    logger.info('dfRawD events: %d', len(dfRawD))
    logger.info('dfRawT events: %d', len(dfRawT))
    logger.info('dfTransEQ events: %d , trans cnt: %d', len(dfTransEQ),\
                len(dfTransEQ[(dfTransEQ['EventID']=='LD DONE') & (dfTransEQ['StartEnd']=='End')]))
    logger.info('dfTransSTK events: %d , trans cnt: %d', len(dfTransSTK),\
                len(dfTransSTK[(dfTransSTK['EventID']=='LD DONE') & (dfTransSTK['StartEnd']=='End')]))
    logger.info('dfTransLSTB events: %d , trans cnt: %d', len(dfTransLSTB),\
                len(dfTransLSTB[(dfTransLSTB['EventID']=='LD DONE') & (dfTransLSTB['StartEnd']=='End')]))
    logger.info('dfTransRSTB events: %d , trans cnt: %d', len(dfTransRSTB),\
                len(dfTransRSTB[(dfTransRSTB['EventID']=='LD DONE') & (dfTransRSTB['StartEnd']=='End')]))
    logger.info('dfStatus1s events: %d', len(dfStatus1s))
    logger.info('dfStatus5s events: %d', len(dfStatus5m))
    logger.info('dfStatus5m events: %d', len(dfRawD))
    logger.info('Machine amount: %d', len(dfStatus5m['MachineID'].unique()))
    logger.info('Node amount: %d', len(dfRawD['NodeID'].unique()))
    logger.info('Station amount: %d', len(dfRawT['StationID'].unique()))


def exec_log2pickle():
    '''Convet log files(.log or .zip) to pickle files    
    

    Returns
    -------
    None.

    '''
    logger.info('Start dump log to pickle: %s ~ %s ', from_date, to_date)
    
    start = time.perf_counter()
    fromtime=dt.datetime.strptime(from_date,'%Y%m%d%H')
    totime=dt.datetime.strptime(to_date,'%Y%m%d%H')
    dtt=fromtime
    while dtt<=totime:
        lst=str(dtt).split('-')
        ArgDate=lst[0]+lst[1]+lst[2].split(' ')[0]+lst[2].split(' ')[1][:2]
        logger.info(ArgDate)
        # 1. Log -> Pickle 
        if(len(lst_vhl)>0):
            if(lst_vhl[0] == 'ALL'):            
                logger.info('ALL log->pickle from log')
                dfn.log2dataframe(fpMCCLog, ArgSite, ArgDate, fpDiagData) # .log
                logger.info('ALL log->pickle from zip')
                dfn.log2dataframeEx(fpMCCLog, ArgSite, ArgDate, fpDiagData) # .log.zip
            else: # lst_vhl
                logger.info('VHL log->pickle from log')
                dfn.log2dataframeByVHL(fpMCCLog, ArgSite, ArgDate, lst_vhl, fpDiagData) # .log
                logger.info('VHL log->pickle from zip')
                dfn.log2dataframeByVHLEx(fpMCCLog, ArgSite, ArgDate, lst_vhl, fpDiagData) # .log.zip
        else:
            logger.error("VHL List is invalid..!!")
            sys.exit()
            
        dtt=dtt+dt.timedelta(hours=1)
        
    logger.info('Dump pickles: Elispe Time - %.2f sec', time.perf_counter() - start)


def exec_load_pickles():
    '''combine each pickles
    dfRawD - driving events
    dfRawT - trans events
    dfRawS - status events    

    Returns
    -------
    list
        DESCRIPTION.

    '''
    logger.info('Start load pickles: %s ~ %s ', from_date, to_date)
    dfRawD=dfRawS=dfRawT=pd.DataFrame()
    start = time.perf_counter()
    fromtime=dt.datetime.strptime(from_date,'%Y%m%d%H')
    totime=dt.datetime.strptime(to_date,'%Y%m%d%H')
    dtt=fromtime
    while dtt<=totime:        
        lst=str(dtt).split('-')
        dtt=dtt+dt.timedelta(hours=1)    
        ArgDate=lst[0]+lst[1]+lst[2].split(' ')[0]+lst[2].split(' ')[1][:2]
        logger.info('Load pickle: %s', ArgDate)
        start = time.perf_counter()
        if(len(lst_vhl)>0):
            if(lst_vhl[0] == 'ALL'):
                dfD = dfn.loadPickleDriveByDataTime(fpDiagData, ArgSite, ArgDate)
                dfT = dfn.loadPickleTransByDataTime(fpDiagData, ArgSite, ArgDate)
                dfS = dfn.loadPickleStatusByDataTime(fpDiagData, ArgSite, ArgDate)            
            else: # lst_vhl
                dfD = dfn.loadPickleDriveByDataTimenVHL(fpDiagData, ArgSite, ArgDate, lst_vhl)
                dfT = dfn.loadPickleTransByDataTimenVHL(fpDiagData, ArgSite, ArgDate, lst_vhl)
                dfS = dfn.loadPickleStatusByDataTimenVHL(fpDiagData, ArgSite, ArgDate, lst_vhl)        
        else:
            logger.error("VHL List is invalid..!!")
            sys.exit()            
           
        dfRawD = dfRawD.append(dfD, ignore_index=True, sort=False)
        dfRawT = dfRawT.append(dfT, ignore_index=True, sort=False)
        dfRawS = dfRawS.append(dfS, ignore_index=True, sort=False)
        
    logger.info('Load pickles: Elispe Time - %.2f sec', time.perf_counter() - start)
    return [dfRawD, dfRawT, dfRawS]

def get_df_byType(event_type):
    '''Return dataframe by event type
    -----------------------------
    |  event_type  |  dataframe |
    -----------------------------
    |   DRV        |   dfRawD   |
    |   TRANS      |   dfRawT   |
    |   STS        |   dfRawS   |
    |   others     |   none     |
    -----------------------------
    
    Parameters
    ----------
    event_type : TYPE(string)
        DESCRIPTION.

    Returns
    -------
    dfRet : TYPE(dataframe)
        DESCRIPTION.

    '''
    dfRet=pd.DataFrame()
    
    if event_type == 'DRV':
        dfRet = dfRawD
    elif event_type == 'TRANS':
        dfRet = dfRawT
    elif event_type == 'STS':
        dfRet = dfRawS
    else:
        logger.warning("Invalid event type : %s", event_type)
    
    return dfRet
        
def get_station_id(event_type):    
    if event_type == 'DRV':
        return 'Target'
    elif event_type == 'TRANS':
        return 'StationID'
    else:
        logger.warning("Invalid event type : %s", event_type)


def exec_preprocess(dfRawD, dfRawT, dfRawS):
    if(len(dfRawD) > 0):
        start = time.perf_counter()
        lst=['Source','Target','NodeID','Offset']
        dfRawD.loc[:,lst]=dfRawD.loc[:,lst].apply(pd.to_numeric,errors = 'coerce', downcast='integer')
        lst=['Velocity']
        dfRawD.loc[:,lst]=dfRawD.loc[:,lst].apply(pd.to_numeric,errors = 'coerce')
        logger.info('Drive to numeric Time: %.2f sec', time.perf_counter()-start)
        
        # 1. Manual : Node ID '0'
        df_abnormal = dfRawD[(dfRawD['NodeID'] == 0) | (dfRawD['Source'] == 0) | (dfRawD['Target'] == 0)]
        df_filtered = dfRawD.drop(df_abnormal.index.to_list())
        dfRawD=df_filtered
        logger.info('Abnormal filter Drive(Manual - Node/Source/Target 0): %d events', len(df_abnormal))
        
        # 2. MARK 중 작업 변경으로 완료 되지 못한 Event 제거 - 우선 Start는 놔두고 End만
        df_abnormal = dfRawD[(dfRawD['EventID']=='NODE MARK') & (dfRawD['StartEnd']=='End')].copy()
        lst=['Value1','Value2','Value3']
        df_abnormal.loc[:,lst]=df_abnormal.loc[:,lst].apply(pd.to_numeric,errors = 'coerce')
        df_abnormal = df_abnormal[(df_abnormal['Value1']==0) & (df_abnormal['Value2']==0) & (df_abnormal['Value3']==0)]
        df_filtered = dfRawD.drop(df_abnormal.index.to_list())
        dfRawD=df_filtered
        logger.info('Abnormal filter Drive(Node Mark Job-change): %d events', len(df_abnormal))
        
        # 3. Node 이접한 Station Tag 중 Node 정지 후 -> Station Mark 상황 제거
        df_abnormal = dfRawD[(dfRawD['EventID']=='STATION MARK') & (dfRawD['StartEnd']=='End')].copy()
        lst=['Value4','Value5','Value6']
        df_abnormal.loc[:,lst]=df_abnormal.loc[:,lst].apply(pd.to_numeric,errors = 'coerce')
        df_abnormal = df_abnormal[(df_abnormal['Value4']==0) & (df_abnormal['Value5']==0) & (df_abnormal['Value6']==0)]
        df_filtered = dfRawD.drop(df_abnormal.index.to_list())
        dfRawD=df_filtered
        logger.info('Abnormal filter Drive(Station Mark Job-change): %d events', len(df_abnormal))
        
        # 4. Node 이접한 QR Tag 중 Node 정지 후 -> Station Mark 상황 제거
        df_abnormal = dfRawD[(dfRawD['EventID']=='QR MARK') & (dfRawD['StartEnd']=='End')].copy()
        lst=['Value4','Value5','Value6']
        df_abnormal.loc[:,lst]=df_abnormal.loc[:,lst].apply(pd.to_numeric,errors = 'coerce')
        df_abnormal = df_abnormal[(df_abnormal['Value4']==0) & (df_abnormal['Value5']==0) & (df_abnormal['Value6']==0)]
        df_filtered = dfRawD.drop(df_abnormal.index.to_list())
        dfRawD=df_filtered
        logger.info('Abnormal filter Drive(QR Mark Job-change): %d events', len(df_abnormal))
        
        # CID LINK Event 삭제 - MCC Log 이상 
        if(dic_opt['MCC Ver']==2.1):
            df_abnormal = dfRawD[(dfRawD['EventID']=='CID LINK')]
            df_filtered = dfRawD.drop(df_abnormal.index.to_list())
            dfRawD=df_filtered
            logger.info('Abnormal filter Drive(remove CID LINK): %d events', len(df_abnormal))
    else:
        logger.warning('dfRawD is empty..!!')
    
    if(len(dfRawT) > 0):
        start = time.perf_counter()
        lst=['StationID','PortType','NodeID','Offset']
        dfRawT.loc[:,lst]=dfRawT.loc[:,lst].apply(pd.to_numeric,errors = 'coerce', downcast='integer')
        logger.info('Trans to numeric Time: %.2f sec', time.perf_counter()-start)
    else:
        logger.warning('dfRawT is empty..!!')

    if(len(dfRawS) > 0):
        start = time.perf_counter()
        lst=['NodeID','Offset']
        dfRawS.loc[:,lst]=dfRawS.loc[:,lst].apply(pd.to_numeric,errors = 'coerce', downcast='integer')
        lst=['Velocity']
        dfRawS.loc[:,lst]=dfRawS.loc[:,lst].apply(pd.to_numeric,errors = 'coerce')
        logger.info('Status to numeric Time: %.2f sec', time.perf_counter()-start)
    
        # 1. Manual : Node ID '0'
        dfRawS['NodeID']=dfRawS['NodeID'].apply(pd.to_numeric,errors = 'coerce')
        
        df_abnormal = dfRawS[dfRawS['NodeID'] == 0]
        df_filtered = dfRawS.drop(df_abnormal.index.to_list())    
        dfRawS=df_filtered
        logger.info('Abnormal filter Status(Node 0): %d events', len(df_abnormal))
        
        # Status seperate #############################################################
        dfStatus1s = dfRawS[(dfRawS['Type']=='STS') & (dfRawS['EventID']=='PERIODIC 1s')]
        dfStatus5s = dfRawS[(dfRawS['Type']=='STS') & (dfRawS['EventID']=='PERIODIC 5s')]
        dfStatus5m = dfRawS[(dfRawS['Type']=='STS') & (dfRawS['EventID']=='PERIODIC 5M')]
                
        df = dfStatus5s
        lst = df.columns.to_list()
        df = df.drop(lst[30:], axis=1)
        dfStatus5s = df
            
        df = dfStatus5m
        lst = df.columns.to_list()
        df = df.drop(lst[16:], axis=1)
        dfStatus5m = df
    else:
        logger.warning('dfRawS is empty..!!')
    
    return [dfRawD, dfRawT, dfStatus1s, dfStatus5s, dfStatus5m]

#진단 시작  ####################################################################
# main의 기준 등록
def refValue():
    global Need
    global Recommand
    Need=50.0         # 즉시 점검 필요, Health Index 구간: 0-50점
    Recommand=70.0    # 점검 추천, Health Index 구간: 51-70점 
refValue()

def check_dump_pickles():
    if(dic_opt['dump pickle']==True):
        logger.info('Start dump pickle')
        fp = fpPklDump + 'dfRawD_'+from_date+'_'+to_date+'.pklz'
        dfn.DumpPickleFile(dfRawD, fp, True)
        fp = fpPklDump + 'dfRawT_'+from_date+'_'+to_date+'.pklz'
        dfn.DumpPickleFile(dfRawT, fp, True)
        fp = fpPklDump + 'dfRawS_'+from_date+'_'+to_date+'.pklz'
        dfn.DumpPickleFile(dfRawS, fp, True)
        fp = fpPklDump + 'dfStatus1s_'+from_date+'_'+to_date+'.pklz'
        dfn.DumpPickleFile(dfStatus1s, fp, True)
        fp = fpPklDump + 'dfStatus5s_'+from_date+'_'+to_date+'.pklz'
        dfn.DumpPickleFile(dfStatus5s, fp, True)
        fp = fpPklDump + 'dfStatus5m_'+from_date+'_'+to_date+'.pklz'
        dfn.DumpPickleFile(dfStatus5m, fp, True)    
        logger.info('Complete dump pickle')

def check_ref_refresh(lst_df, ref):
    if(dic_opt['Ref Refresh']==True):
        logger.info('Start reference refresh')
        machine_id='V30201' # user pre-define
        dfD=lst_df[0]; dfT=lst_df[1]; dfS=lst_df[2]
        dfD=dfD[dfD['MachineID']==machine_id]
        dfT=dfT[dfT['MachineID']==machine_id]
        dfS=dfS[dfS['MachineID']==machine_id]        
        
        for i in range(len(ref)):
            key_name=ref.iloc[i]['key_name']
            event_type=ref.iloc[i]['event_type']
            event_id=ref.iloc[i]['event_id']
            start_end=ref.iloc[i]['start_end']
            value_no='Value'+str(ref.iloc[i]['value_no'])
            
            if(event_type=='DRV'):
                df=dfD
            elif(event_type=='TRANS'):
                df=dfT
            elif(event_type=='STS'):
                df=dfS
            else:
                logger.info(f'Invalid({event_type}) event type..!!')
                return ref
                
            if((start_end=='Start') | (start_end=='End')):
                df=df[(df['EventID']==event_id) & (df['StartEnd']==start_end)]
            else: # no start/end
                df=df[(df['EventID']==event_id)]
                
            df=df[['MachineID', value_no]]
            df[value_no]=df[value_no].apply(pd.to_numeric,errors = 'coerce')
            
            mean=df[value_no].mean()
            std=df[value_no].std()
            LSL=round(mean - (6*std), 5)
            USL=round(mean + (6*std), 5)
            
            #decimal point - 3 fix
            ref.loc[i,'mean']=round(mean,3)
            ref.loc[i,'std']=round(std,3)
            ref.loc[i,'min']=round(df[value_no].min(),3)
            ref.loc[i,'max']=round(df[value_no].max(),3)
            ref.loc[i,'LSL']=round(LSL,3)
            ref.loc[i,'USL']=round(USL,3)
            
            logger.info(f"{event_type},{event_id},{start_end},{value_no},{key_name},{LSL:.3f},{USL:.3f}")
            
        logger.info("db table update")
        ddb.update_df2table(ddb.db_name, ref, 'cpk_ref_data')
 
        logger.info('Complete refresh pickle')
        
    return ref


def get_fixed_col_list(event_type):    
    lst=[]
    if(event_type=='STS'):
        lst=['Time','MachineID','NodeID','Material','Velocity']
    elif(event_type=='DRV'):
        lst=['Time','MachineID','NodeID','Target','Material','Velocity']
    elif(event_type=='TRANS'):
        lst=['Time','MachineID','StationID','Material','Velocity']
    else:
        logger.warning("Invalid event type : %s", event_type)
        
    return lst

def calc_index(filter_event, col_list, dic_Ref, ref_key):
    '''Calculate health index by items

    Parameters
    ----------
    filter_event : TYPE(dictionaly)
        DESCRIPTION. filter key items
    col_list : TYPE(list)
        DESCRIPTION. Include key items
    dic_Ref : TYPE(dictionaly)
        DESCRIPTION. reference items
    ref_key : TYPE(str)
        DESCRIPTION. reference key(MachineID, NodeID, StationID)

    Returns
    -------
    dfHi : TYPE(dataframe)
        DESCRIPTION. health index

    '''
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio(ref_key, df)
    else:
        dfHi=pd.DataFrame(index=lst_machine)
        dfHi['total']=100.0
        
    return dfHi

def db_ivalue_update(group_name, dfHi, hi_item, rename_columns):
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, dfHi, group_name, table_name, rename_columns, analysis_time_db)

def get_analysis_match_key(analysis_key):
    ret_key='Unknown'
    if(analysis_key == 'machine'):
        ret_key='MachineID'
    elif(analysis_key == 'node'):
        ret_key='NodeID'
    elif(analysis_key == 'station'):
        ret_key='StationID'
    
    return ret_key


def exec_indexing(dfRawD, DfRawT, dfStatus1s, dfStatus5s, dfStatus5m, ref, file_path):
    lst_machine=dfStatus5m['MachineID'].unique().tolist()
    lst_node=dfStatus1s['NodeID'].unique().tolist()
    lst_station=dfRawT['StationID'].unique().tolist()
    
    dfHiOht=pd.DataFrame(index=lst_machine)
    dfHiNode=pd.DataFrame(index=lst_node)
    dfHiStation=pd.DataFrame(index=lst_station)
    #########################################################################################################
    # Node Mark
    hi_item='mark_node'
    # Set
    filter_event={'Type':'DRV', 'EventID':'NODE MARK', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID','Velocity',
              'Value2','Value3','Value4']
    dic_Ref={'Node Mark Distance':['GLT', 160, 200, 100, 1],
             'Node Mark Sensor Distance':['GLT', 290, 300, 270, 1],
             'Node Mark Start Velocity':['GT', 1.0, 1.4, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['total']=100.0
        dfHiN=pd.DataFrame(index=lst_node)
        dfHiN['total']=100.0
        
    # Summary
    rename_columns={'NodeMarkDistGT':'node_mark_distanceGT','NodeMarkDistLT':'node_mark_distanceLT',
                    'NodeSensorDistGT':'node_sensor_distanceGT','NodeSensorDistLT':'node_sensor_distanceLT',
                    'NodeMarkStartSpeedGT':'node_mark_enter_velocityGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    #########################################################################################################
    # QR Mark
    hi_item='mark_qr'
    # Set
    filter_event={'Type':'DRV', 'EventID':'QR MARK', 'StartEnd':'End'}
    col_list=['Time','MachineID','Target',
              'Value2','Value3','Value4','Value7']
    dic_Ref={'QR Mark Distance':['GLT', 460, 500, 300, 1],
             'QR Mark Offset':['GLT', 25, 50, 10, 1],
             'QR Mark Start Velocity':['GT', 1.0, 1.4, 0, 1],
             'QR Parent Node Diff':['GLT', 0, 15, -15, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiS = DiagType.calcHealthIndexQuickRatio('Target', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['total']=100.0
        dfHiS=pd.DataFrame(index=lst_station)
        dfHiS['total']=100.0
    
    # Summary
    rename_columns={'QRMarkDistGT':'QR_mark_distanceGT','QRMarkDistLT':'QR_mark_distanceLT',
                    'QROffsetGT':'QR_offsetGT','QROffsetLT':'QR_offsetLT',
                    'QRMarkStartSpeedGT':'QR_mark_enter_velocityGT',
                    'ParentNodeOffsetDifferenceGT':'QR_node_offset_diffGT',
                    'ParentNodeOffsetDifferenceLT':'QR_node_offset_diffLT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='station'
    df=dfHiS
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiStation=dfn.addHealthIndex(hi_item, dfHiStation, dfHiS)
    
    ##########################################################################################################
    # Station Mark
    hi_item='mark_station'
    # Set
    filter_event={'Type':'DRV', 'EventID':'STATION MARK', 'StartEnd':'End'}
    col_list=['Time','MachineID','Target',
              'Value2','Value3','Value4','Value7','Value8']
    dic_Ref={'ST Mark Distance':['GLT', 290, 400, 100, 1],
             'ST Mark Sensor Distance':['GLT', 150, 200, 125, 1],
             'ST Mark Start Velocity':['GT', 1.0, 1.4, 0, 1],
             'ST Mark Front Sensor Hunting':['GT', 0, 0, 0, 1],         
             'ST Parent Node Diff':['GLT', 0, 15, -15, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiS = DiagType.calcHealthIndexQuickRatio('Target', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['total']=100.0
        dfHiS=pd.DataFrame(index=lst_station)
        dfHiS['total']=100.0
    
    # Summary
    rename_columns={'StationMarkDistGT':'station_mark_distanceGT','StationMarkDistLT':'station_mark_distanceLT',
                    'StationSensorDistGT':'station_sensor_distanceGT',
                    'StationSensorDistLT':'station_sensor_distanceLT',
                    'StationMarkStartSpeedGT':'station_mark_enter_velocityGT',
                    'FrontSensorHuntingCountGT':'station_sensor_haunt_countGT',
                    'ParentNodeOffsetDifferenceGT':'station_node_offset_diffGT',
                    'ParentNodeOffsetDifferenceLT':'station_node_offset_diffLT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='station'
    df=dfHiS
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiStation=dfn.addHealthIndex(hi_item, dfHiStation, dfHiS)
    
    ##########################################################################################################
    # Node Compensation
    hi_item='tag_node'
    # Set
    filter_event={'Type':'DRV', 'EventID':'NODE DETECT'}
    col_list=['Time','MachineID','NodeID',
              'Value4']
    dic_Ref={'Node Compensation':['GLT', 0, 100, -100, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['total']=100.0
        dfHiN=pd.DataFrame(index=lst_node)
        dfHiN['total']=100.0
    
    # Summary
    rename_columns={'CompensationGT':'node_compensationGT','CompensationLT':'node_compensationLT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # BCR Driving
    hi_item='bcr_drive'
    # Set
    filter_event={'Type':'DRV', 'EventID':'NODE MARK', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value7', 'Value8']
    dic_Ref={r'Driving BCR Read Fail':['GT', 0, 1, 0, 1],
             r'Driving BCR Trigger Count':['GT', 150, 180, 100, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['total']=100.0
        dfHiN=pd.DataFrame(index=lst_node)
        dfHiN['total']=100.0
    
    # Summary
    rename_columns={'DrivingBCRReadFailGT':'BCR_drive_readfail_countGT',
                    'DrivingBCRTriggerCountGT':'BCR_drive_trigger_countGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
            
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN) 
        
    ##########################################################################################################
    # BCR Trans
    hi_item='bcr_trans'
    # Set
    filter_event={'Type':'DRV', 'EventID':'STATION MARK', 'StartEnd':'End'}
    col_list=['Time','MachineID','Target',
              'Value9','Value10']
    dic_Ref={r'Trans BCR Read Fail Count':['GT', 0, 1, 0, 1],
             r'Trans BCR Trigger Count':['GT', 150, 180, 100, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiS = DiagType.calcHealthIndexQuickRatio('Target', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['total']=100.0
        dfHiS=pd.DataFrame(index=lst_station)
        dfHiS['total']=100.0
    
    # Summary
    rename_columns={'TransBCRReadFailGT':'BCR_trans_readfail_countGT',
                    'TransBCRTriggerCountGT':'BCR_trans_trigger_countGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='station'
    df=dfHiS
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiStation=dfn.addHealthIndex(hi_item, dfHiStation, dfHiS) 
    
    ##########################################################################################################
    # CID-R Monitor
    hi_item='cidr_monitor'
    # Set
    filter_event={'Type':'DRV', 'EventID':'CID MONITOR', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value1','Value2','Value3','Value4','Value5',
              'Value6','Value7']
    dic_Ref={'CID R First Link Level':['LT', 240, 300, 170, 1],
             'CID R FiberError':['GT', 0, 5, 0, 1],
             'CID R RFError':['GT', 0, 50, 0, 1],
             'CID R Max Current':['GT', 5, 25, 0, 1],
             'CID R Max Voltage':['GT', 240, 300, 100, 1],
             'CID R Max Temp':['GT', 42, 55, 20, 1],
             'CID R Noise':['GT', 2, 5, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)    
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiN=pd.DataFrame(index=lst_node)
        dfHiM['total']=100.0    
        dfHiN['total']=100.0    
        lst_col=['CIDR_first_linklevelLT','CIDR_fiber_errorGT','CIDR_RF_errorGT','CIDR_max_currentGT',\
                 'CIDR_max_voltageGT','CIDR_max_temperatureGT','CIDR_noiseGT']
        for l in lst_col:
            dfHiM[l]=100.0
            dfHiN[l]=100.0
    
    # Summary   
    rename_columns={'CIDRFirstLinkLevelLT':'CIDR_first_linklevelLT',
                    'CIDRFiberErrorGT':'CIDR_fiber_errorGT',
                    'CIDRRFErrorGT':'CIDR_RF_errorGT',
                    'CIDRMaxCurrentGT':'CIDR_max_currentGT',
                    'CIDRMaxVoltageGT':'CIDR_max_voltageGT',
                    'CIDRMaxTempGT':'CIDR_max_temperatureGT',
                    'CIDRNoiseGT':'CIDR_noiseGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)            
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # CID-O Monitor
    hi_item='cido_monitor'
    # Set
    filter_event={'Type':'DRV', 'EventID':'CID MONITOR', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value8','Value9','Value10']
    dic_Ref={'CID O First Link Level':['LT', 240, 300, 170, 1],
             'CID O FiberError':['GT', 0, 1, 0, 1],
             'CID O RFError':['GT', 0, 1, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['total']=100.0
        dfHiN=pd.DataFrame(index=lst_node)
        dfHiN['total']=100.0    
        lst_col=['CIDO_first_linklevelLT','CIDO_fiber_errorGT','CIDO_RF_errorGT']
        for l in lst_col:
            dfHiM[l]=100.0
            dfHiN[l]=100.0
    
    # Summary
    rename_columns={'CIDOFirstLinkLevelLT':'CIDO_first_linklevelLT',
                    'CIDOFiberErrorGT':'CIDO_fiber_errorGT',
                    'CIDORFErrorGT':'CIDO_RF_errorGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
           
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN) 
    
    ##########################################################################################################
    # CID Link
    hi_item='cid_link'
    # Set
    filter_event={'Type':'DRV', 'EventID':'CID LINK', 'StartEnd':'Start'}
    col_list=['Time','MachineID','NodeID', 'Offset','Velocity',
              'Value1','Value2','Value3']
    
    dic_Ref={'CID Link Time':['GT', 0, 100, 20, 1],
             'CID Link Map Offset':['GT', 0, 1800, 0, 1],
             'CID Link Distance':['GT', 0, 100, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    
    if(len(df)>0):
        # Original 외 가공 데이터 참조 시 별도 분석(calcHealthIndexQuickRatioEx) 진행... 
        lst_diag_col=['CID_link_distance_diff']
        df['CID_link_distance_diff']=df['CIDLinkDistance']-df['MapCIDLinkOffset']
        dic_RefEx={'CID_link_distance_diff':['GLT', 0, 100, 20, 1]}
        dfRefEx=pd.DataFrame(data=dic_RefEx)
        dfRefEx.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
        #filter - Time의 경우 Offset 0 기준하여 남기고 있어.. 그 기준으로 감..
        df = df[df['MapCIDLinkOffset'] == 0]
        dfHiM = DiagType.calcHealthIndexQuickRatioEx('MachineID', df, lst_diag_col, dfRefEx)
        dfHiN = DiagType.calcHealthIndexQuickRatioEx('NodeID', df, lst_diag_col, dfRefEx)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['total']=100.0
        dfHiN=pd.DataFrame(index=lst_node)
        dfHiN['total']=100.0
        lst_col=['CID_link_distance_diffGT','CID_link_distance_diffLT']
        for l in lst_col:
            dfHiM[l]=100.0
            dfHiN[l]=100.0
    
    # Summary
    rename_columns={'CID_link_distance_diffGT':'CID_link_distance_diffGT',
                    'CID_link_distance_diffLT':'CID_link_distance_diffLT',}
        
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # Slide Home Monitor
    hi_item='slide_home_off'
    # Set
    filter_event={'Type':'DRV', 'EventID':'DRIVING', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value5']
    dic_Ref={'Slide Home Off':['GT', 0, 0, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['total']=100.0
        dfHiN=pd.DataFrame(index=lst_node)
        dfHiN['total']=100.0
    
    # Summary
    rename_columns={'ShiftHomeSensorOffGT':'slide_home_sensor_offGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # FOUP Detect Sensor
    hi_item='inner_foup_detect'
    # Set
    filter_event={'Type':'DRV', 'EventID':'DRIVING', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value3','Value4']
    dic_Ref={'FOUP Abnormal Detect':['GT', 0, 0, 0, 1],
             'FOUP Abnormal Not-Detect':['GT', 0, 0, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['total']=100.0
    
    # Summary
    rename_columns={'InnerFOUPDetectGT':'foup_abnormal_detectGT',
                    'InnerFOUPNotDetectGT':'foup_abnormal_undetectedGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    
    ##########################################################################################################
    # Abnormal Stop
    hi_item='abnormal_stop'
    # Set
    filter_event={'Type':'DRV', 'EventID':'ABNORMAL STOP', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value1']
    dic_Ref={'Drive Abnormal Stop':['GT', 0, 10000, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['AbnormalStopTimeGT']=100.0
        dfHiM['total']=100.0
    
    # Summary
    rename_columns={'AbnormalStopTimeGT':'abnormal_stop_timeGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    
    ##########################################################################################################
    hi_item='shutter_time'
    # Shutter Front Open Time
    # Set
    filter_event={'Type':'DRV', 'EventID':'SHUTTER FRONT OPEN', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value2']
    dic_Ref={'Shutter Front Open Time':['GLT', 1000, 1500, 500, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiOhtShutterFrontOpen = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        
        key_name='shutter_front_open_movetime'
        df=df.rename(columns={'ShutterFrontOpenTime':key_name})
        dfCpkOhtShutterFrontOpen = dfn.calc_cpk(df, key_name, ref, analysis_time_db)
        dfn.my_plot_cpk(key_name, df, {'x':'MachineID', 'y':key_name},(16,10),1,\
                        {'min':600, 'max':1200}, {'min':800, 'max':1000, 'ref':880}, True,\
                        f'{file_path}{analysis_time_file}_{key_name}.png', ref, dfCpkOhtShutterFrontOpen)        
    else:
        dfHiOhtShutterFrontOpen=pd.DataFrame(index=lst_machine)
        dfHiOhtShutterFrontOpen['total']=100.0
    
    # Shutter Front Close Time
    # Set
    filter_event={'Type':'DRV', 'EventID':'SHUTTER FRONT CLOSE', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value2']
    dic_Ref={'Shutter Front Close Time':['GLT', 1000, 1500, 500, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiOhtShutterFrontClose = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        
        key_name='shutter_front_close_movetime'
        df=df.rename(columns={'ShutterFrontCloseTime':key_name})
        dfCpkOhtShutterFrontClose = dfn.calc_cpk(df, key_name, ref, analysis_time_db)
        dfn.my_plot_cpk(key_name, df, {'x':'MachineID', 'y':key_name},(16,10),1,\
                        {'min':600, 'max':1200}, {'min':800, 'max':1000, 'ref':880}, True,\
                        f'{file_path}{analysis_time_file}_{key_name}.png', ref, dfCpkOhtShutterFrontClose)
    else:
        dfHiOhtShutterFrontClose=pd.DataFrame(index=lst_machine)
        dfHiOhtShutterFrontClose['total']=100.0
    
    # Shutter Rear Open Time
    # Set
    filter_event={'Type':'DRV', 'EventID':'SHUTTER REAR OPEN', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value2']
    dic_Ref={'Shutter Rear Open Time':['GLT', 1000, 1500, 500, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiOhtShutterRearOpen = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    else:
        dfHiOhtShutterRearOpen=pd.DataFrame(index=lst_machine)
        dfHiOhtShutterRearOpen['total']=100.0
    
    # Shutter Rear Close Time
    # Set
    filter_event={'Type':'DRV', 'EventID':'SHUTTER REAR CLOSE', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value2']
    dic_Ref={'Shutter Rear Close Time':['GLT', 1000, 1500, 500, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiOhtShutterRearClose = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    else:
        dfHiOhtShutterRearClose=pd.DataFrame(index=lst_machine)
        dfHiOhtShutterRearClose['total']=100.0
    
    # Summary
    rename_columns={'ShutterFrontOpenTimeGT':'shutter_front_open_movetimeGT',
                    'ShutterFrontOpenTimeLT':'shutter_front_open_movetimeLT',
                    'ShutterFrontCloseTimeGT':'shutter_front_close_movetimeGT',
                    'ShutterFrontCloseTimeLT':'shutter_front_close_movetimeLT',
                    'ShutterRearOpenTimeGT':'shutter_rear_open_movetimeGT',
                    'ShutterRearOpenTimeLT':'shutter_rear_open_movetimeLT',
                    'ShutterRearCloseTimeGT':'shutter_rear_close_movetimeGT',
                    'ShutterRearCloseTimeLT':'shutter_rear_close_movetimeLT'}
    
    frames=[dfHiOhtShutterFrontOpen, dfHiOhtShutterFrontClose, dfHiOhtShutterRearOpen, dfHiOhtShutterRearClose]
    dfHiOhtShutter=pd.concat(frames, axis=1)
    dfHiOhtShutter=dfHiOhtShutter.drop(columns=['total'])
    dfHiOhtShutter=dfHiOhtShutter.fillna(100.0)
    dfHiOhtShutter['total']=round(dfHiOhtShutter.sum(axis=1)/len(rename_columns),0)
    
    
    group_name='machine'
    df=dfHiOhtShutter
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    table_name='cpk_machine'
    df=dfCpkOhtShutterFrontOpen
    ddb.insert_df2table(ddb.db_name, df, table_name)
        
    df=dfCpkOhtShutterFrontClose
    ddb.insert_df2table(ddb.db_name, df, table_name)    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiOhtShutter)
    
    ##########################################################################################################
    hi_item='steer_sensor'
    # Steer Front Left Sensing Count
    # Set
    filter_event={'Type':'DRV', 'EventID':'STEER FRONT LEFT', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value3']
    dic_Ref={'Steer FLeft Sensing Count':['GT', 1, 1, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiOhtSteerFLeftSensingCnt = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiNodeSteerFLeftSensingCnt = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    else:
        dfHiOhtSteerFLeftSensingCnt=pd.DataFrame(index=lst_machine)
        dfHiOhtSteerFLeftSensingCnt['total']=100.0
        dfHiNodeSteerFLeftSensingCnt=pd.DataFrame(index=lst_node)
        dfHiNodeSteerFLeftSensingCnt['total']=100.0
    
    # Steer Front Right Sensing Count
    # Set
    filter_event={'Type':'DRV', 'EventID':'STEER FRONT RIGHT', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value3']
    dic_Ref={'Steer FRight Sensing Count':['GT', 1, 1, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiOhtSteerFRightSensingCnt = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiNodeSteerFRightSensingCnt = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    else:
        dfHiOhtSteerFRightSensingCnt=pd.DataFrame(index=lst_machine)
        dfHiOhtSteerFRightSensingCnt['total']=100.0
        dfHiNodeSteerFRightSensingCnt=pd.DataFrame(index=lst_node)
        dfHiNodeSteerFRightSensingCnt['total']=100.0
    
    # Steer Rear Left Sensing Count
    # Set
    filter_event={'Type':'DRV', 'EventID':'STEER REAR LEFT', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value3']
    dic_Ref={'Steer RLeft Sensing Count':['GT', 1, 1, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiOhtSteerRLeftSensingCnt = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiNodeSteerRLeftSensingCnt = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    else:
        dfHiOhtSteerRLeftSensingCnt=pd.DataFrame(index=lst_machine)
        dfHiOhtSteerRLeftSensingCnt['total']=100.0
        dfHiNodeSteerRLeftSensingCnt=pd.DataFrame(index=lst_node)
        dfHiNodeSteerRLeftSensingCnt['total']=100.0
    
    # Steer Rear Right Sensing Count
    # Set
    filter_event={'Type':'DRV', 'EventID':'STEER REAR RIGHT', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID',
              'Value3']
    dic_Ref={'Steer RRight Sensing Count':['GT', 1, 1, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    # 임시.. 로그가 잘못 찍히고 있음.. 
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHi=dfHi.rename(columns={'SteerRearLeftHuntingCountGT':'SteerRearRightHuntingCountGT'})
        dfHiOhtSteerRRightSensingCnt = dfHi
        dfHi = DiagType.calcHealthIndexQuickRatio('NodeID', df)
        dfHi=dfHi.rename(columns={'SteerRearLeftHuntingCountGT':'SteerRearRightHuntingCountGT'})
        dfHiNodeSteerRRightSensingCnt = dfHi
    else:
        dfHiOhtSteerRRightSensingCnt=pd.DataFrame(index=lst_machine)
        dfHiOhtSteerRRightSensingCnt['total']=100.0
        dfHiNodeSteerRRightSensingCnt=pd.DataFrame(index=lst_node)
        dfHiNodeSteerRRightSensingCnt['total']=100.0
    
    # Summary
    rename_columns={'SteerFrontLeftHuntingCountGT':'steer_front_left_sensor_countGT',
                    'SteerFrontRightHuntingCountGT':'steer_front_right_sensor_countGT',
                    'SteerRearLeftHuntingCountGT':'steer_rear_left_sensor_countGT',
                    'SteerRearRightHuntingCountGT':'steer_rear_right_sensor_countGT'}
    
    frames=[dfHiOhtSteerFLeftSensingCnt, dfHiOhtSteerFRightSensingCnt,
            dfHiOhtSteerRLeftSensingCnt, dfHiOhtSteerRRightSensingCnt]
    dfHiOhtSteerSensor=pd.concat(frames, axis=1)
    dfHiOhtSteerSensor=dfHiOhtSteerSensor.drop(columns=['total'])
    dfHiOhtSteerSensor['total']=round(dfHiOhtSteerSensor.sum(axis=1)/len(rename_columns),0)
    dfHiOhtSteerSensor=dfHiOhtSteerSensor.astype({'SteerFrontLeftHuntingCountGT':'int',
                                                   'SteerFrontRightHuntingCountGT':'int',
                                                   'SteerRearLeftHuntingCountGT':'int',
                                                   'SteerRearRightHuntingCountGT':'int',
                                                   'total':'int'})
    
    group_name='machine'
    df=dfHiOhtSteerSensor
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    frames=[dfHiNodeSteerFLeftSensingCnt, dfHiNodeSteerFRightSensingCnt,
            dfHiNodeSteerRLeftSensingCnt, dfHiNodeSteerRRightSensingCnt]
    dfHiNodeSteerSensor=pd.concat(frames, axis=1)
    dfHiNodeSteerSensor=dfHiNodeSteerSensor.drop(columns=['total'])
    dfHiNodeSteerSensor=dfHiNodeSteerSensor.fillna(100.0)
    dfHiNodeSteerSensor['total']=round(dfHiNodeSteerSensor.sum(axis=1)/len(rename_columns),0)
    dfHiNodeSteerSensor.astype({'SteerFrontLeftHuntingCountGT':'int',
                                'SteerFrontRightHuntingCountGT':'int',
                                'SteerRearLeftHuntingCountGT':'int',
                                'SteerRearRightHuntingCountGT':'int',
                                'total':'int'})
    
    group_name='node'
    df=dfHiOhtSteerSensor
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiOhtSteerSensor)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiNodeSteerSensor)
    
    ##########################################################################################################
    hi_item='steer_time'
    # Steer Front Left Time
    # Set
    filter_event={'Type':'DRV', 'EventID':'STEER FRONT LEFT', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID','Velocity',
              'Value2']
    dic_Ref={'Steer FLeft Time':['GLT', 500, 900, 300, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        # datafilter
        # Velocity 0.0 제외 - 장애물감지 상태에서 시간 측정 오류
        df['Velocity']=df['Velocity'].apply(pd.to_numeric,errors = 'coerce')
        df=df[df['Velocity']>0]
        
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiOhtSteerFLeftTime = dfHi
        dfHi = DiagType.calcHealthIndexQuickRatio('NodeID', df)
        dfHiNodeSteerFLeftTime = dfHi
    else:
        dfHiOhtSteerFLeftTime=pd.DataFrame(index=lst_machine)
        dfHiOhtSteerFLeftTime['total']=100.0
        dfHiNodeSteerFLeftTime=pd.DataFrame(index=lst_node)
        dfHiNodeSteerFLeftTime['total']=100.0
    
    # Steer Front Right Time
    # Set
    filter_event={'Type':'DRV', 'EventID':'STEER FRONT RIGHT', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID','Velocity',
              'Value2']
    dic_Ref={'Steer FRight Time':['GLT', 500, 900, 300, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        # datafilter
        # Velocity 0.0 제외 - 장애물감지 상태에서 시간 측정 오류
        df['Velocity']=df['Velocity'].apply(pd.to_numeric,errors = 'coerce')
        df=df[df['Velocity']>0]
        
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiOhtSteerFRightTime = dfHi
        dfHi = DiagType.calcHealthIndexQuickRatio('NodeID', df)
        dfHiNodeSteerFRightTime = dfHi
    else:
        dfHiOhtSteerFRightTime=pd.DataFrame(index=lst_machine)
        dfHiOhtSteerFRightTime['total']=100.0
        dfHiNodeSteerFRightTime=pd.DataFrameindex=lst_node()
        dfHiNodeSteerFRightTime['total']=100.0
    
    # Steer Rear Left Time
    # Set
    filter_event={'Type':'DRV', 'EventID':'STEER REAR LEFT', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID','Velocity',
              'Value2']
    dic_Ref={'Steer RLeft Time':['GLT', 500, 900, 300, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        # datafilter
        # Velocity 0.0 제외 - 장애물감지 상태에서 시간 측정 오류
        df['Velocity']=df['Velocity'].apply(pd.to_numeric,errors = 'coerce')
        df=df[df['Velocity']>0]
        
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiOhtSteerRLeftTime = dfHi
        dfHi = DiagType.calcHealthIndexQuickRatio('NodeID', df)
        dfHiNodeSteerRLeftTime = dfHi
    else:
        dfHiOhtSteerRLeftTime=pd.DataFrame(index=lst_machine)
        dfHiOhtSteerRLeftTime['total']=100.0
        dfHiNodeSteerRLeftTime=pd.DataFrame(index=lst_node)
        dfHiNodeSteerRLeftTime['total']=100.0
    
    # Steer Rear Right Time
    # Set
    filter_event={'Type':'DRV', 'EventID':'STEER REAR RIGHT', 'StartEnd':'End'}
    col_list=['Time','MachineID','NodeID','Velocity',
              'Value2']
    dic_Ref={'Steer RRight Time':['GLT', 500, 900, 300, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        # datafilter
        # Velocity 0.0 제외 - 장애물감지 상태에서 시간 측정 오류
        df['Velocity']=df['Velocity'].apply(pd.to_numeric,errors = 'coerce')
        df=df[df['Velocity']>0]
        
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiOhtSteerRRightTime = dfHi
        dfHi = DiagType.calcHealthIndexQuickRatio('NodeID', df)
        dfHiNodeSteerRRightTime = dfHi
    else:
        dfHiOhtSteerRRightTime=pd.DataFrame(index=lst_machine)
        dfHiOhtSteerRRightTime['total']=100.0
        dfHiNodeSteerRRightTime=pd.DataFrame(index=lst_node)
        dfHiNodeSteerRRightTime['total']=100.0
    
    # Summary
    rename_columns={'SteerFrontLeftTimeGT':'steer_front_left_movetimeGT',
                    'SteerFrontLeftTimeLT':'steer_front_left_movetimeLT',
                    'SteerFrontRightTimeGT':'steer_front_right_movetimeGT',
                    'SteerFrontRightTimeLT':'steer_front_right_movetimeLT',
                    'SteerRearLeftTimeGT':'steer_rear_left_movetimeGT',
                    'SteerRearLeftTimeLT':'steer_rear_left_movetimeLT',
                    'SteerRearRightTimeGT':'steer_rear_right_movetimeGT',
                    'SteerRearRightTimeLT':'steer_rear_right_movetimeLT'}
    
    frames=[dfHiOhtSteerFLeftTime, dfHiOhtSteerFRightTime, dfHiOhtSteerRLeftTime, dfHiOhtSteerRRightTime]
    dfHiOhtSteerTime=pd.concat(frames, axis=1)
    dfHiOhtSteerTime=dfHiOhtSteerTime.drop(columns=['total'])
    dfHiOhtSteerTime=dfHiOhtSteerTime.fillna(100.0)
    dfHiOhtSteerTime['total']=round(dfHiOhtSteerTime.sum(axis=1)/len(rename_columns),0)
    
    
    group_name='machine'
    df=dfHiOhtSteerTime
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    frames=[dfHiNodeSteerFLeftTime, dfHiNodeSteerFRightTime, dfHiNodeSteerRLeftTime, dfHiNodeSteerRRightTime]
    dfHiNodeSteerTime=pd.concat(frames, axis=1)
    dfHiNodeSteerTime=dfHiNodeSteerTime.drop(columns=['total'])
    dfHiNodeSteerTime=dfHiNodeSteerTime.fillna(100.0)
    dfHiNodeSteerTime['total']=round(dfHiNodeSteerTime.sum(axis=1)/len(rename_columns),0)
    
    
    group_name='node'
    df=dfHiNodeSteerTime
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiOhtSteerTime)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiNodeSteerTime)
    
    ##########################################################################################################
    # FOUP Detect Position
    # LSTB(PortType 5)만 우선 봅시다.
    hi_item='inner_foup_detect_positon'
    # Set
    filter_event={'Type':'TRANS', 'EventID':'LD MOVE SHIFTROTATE', 'StartEnd':'End', 'PortType':5}
    col_list=['Time','MachineID','StationID',
              'Value5']
    dic_Ref={'FOUP Abnormal Off Detect':['GLT', -300, -250, -350, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiOhtFoupDetectOffPos = dfHi
    else:
        dfHiOhtFoupDetectOffPos=pd.DataFrame(index=lst_machine)
        dfHiOhtFoupDetectOffPos['foup_detect_off_positionGT']=100.0
        dfHiOhtFoupDetectOffPos['foup_detect_off_positionLT']=100.0
        dfHiOhtFoupDetectOffPos['total']=100.0
    
    # LSTB(PortType 5)만 우선 봅시다.
    # Set
    filter_event={'Type':'TRANS', 'EventID':'ULD MOVE SHIFTROTATE HOME', 'StartEnd':'End', 'PortType':5}
    col_list=['Time','MachineID','StationID',
              'Value5']
    dic_Ref={'FOUP Abnormal On Detect':['GLT', -300, -250, -350, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiOhtFoupDetectOnPos = dfHi
    else:
        dfHiOhtFoupDetectOnPos=pd.DataFrame(index=lst_machine)
        dfHiOhtFoupDetectOnPos['foup_detect_on_positionGT']=100.0
        dfHiOhtFoupDetectOnPos['foup_detect_on_positionLT']=100.0
        dfHiOhtFoupDetectOnPos['total']=100.0
    
    # Summary
    rename_columns={'FOUPDetectOnPosGT':'foup_detect_on_positionGT',
                    'FOUPDetectOnPosLT':'foup_detect_on_positionLT',
                    'FOUPDetectOffPosGT':'foup_detect_off_positionGT',
                    'FOUPDetectOffPosLT':'foup_detect_off_positionLT'}
    
    frames=[dfHiOhtFoupDetectOffPos, dfHiOhtFoupDetectOnPos]
    dfHiOhtFoupDetectPos=pd.concat(frames, axis=1)
    dfHiOhtFoupDetectPos=dfHiOhtFoupDetectPos.drop(columns=['total'])
    dfHiOhtFoupDetectPos=dfHiOhtFoupDetectPos.fillna(100.0)
    dfHiOhtFoupDetectPos['total']=round(dfHiOhtFoupDetectPos.sum(axis=1)/len(rename_columns),0)
    
    
    group_name='machine'
    df=dfHiOhtFoupDetectPos
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiOhtFoupDetectPos)
    
    ##########################################################################################################
    # FOUP Cover Detect
    hi_item='foup_cover_detect_position'
    # Set
    filter_event={'Type':'TRANS', 'EventID':'LD MOVE SHIFTROTATE', 'StartEnd':'End','PortType':5}
    col_list=['Time','MachineID','NodeID',
              'Value4']
    dic_Ref={'Foup Cover Detect Position':['GT', -8, -5, -13, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['foup_cover_detect_on_positionGT']=100.0
        dfHiM['total']=100.0
    
    # Summary
    rename_columns={'FOUPCoverOnPosGT':'foup_cover_detect_on_positionGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    
    
    ##########################################################################################################
    hi_item='hoist_home'
    # Hoist Home Off Postion(load)
    # FOUP 들었을때만 체크 - STB가 안정적
    # Set
    filter_event={'Type':'TRANS', 'EventID':'LD MOVE DOWN HOIST FAST', 'StartEnd':'End', 'PortType':5}
    col_list=['Time','MachineID','StationID',
              'Value5']
    dic_Ref={'Hoist Home Off Pos(load)':['GLT', 6, 8, 3, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        # 동일 Key-name으로 임시 변환 처리
        dfHi = dfHi.rename(columns={'HoistHomeOffPosGT':'LdHoistHomeOffPosGT',
                                    'HoistHomeOffPosLT':'LdHoistHomeOffPosLT'})
        dfHiOhtHoistHomeOffPosLd = dfHi
    else:
        dfHiOhtHoistHomeOffPosLd=pd.DataFrame(index=lst_machine)
        dfHiOhtHoistHomeOffPosLd['LdHoistHomeOffPosGT']=100.0
        dfHiOhtHoistHomeOffPosLd['LdHoistHomeOffPosLT']=100.0
        dfHiOhtHoistHomeOffPosLd['total']=100.0
        
    # Hoist Home On Postion(Load)
    # FOUP 들었을때만 체크 - EQ가 안정적
    # Set
    filter_event={'Type':'TRANS', 'EventID':'LD DONE', 'StartEnd':'End', 'PortType':1}
    col_list=['Time','MachineID','StationID',
              'Value3']
    dic_Ref={'Hoist Home on Pos(load)':['GLT', 6, 8, 3, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        # 동일 Key-name으로 임시 변환 처리
        dfHi = dfHi.rename(columns={'HoistHomeOnPosGT':'LdHoistHomeOnPosGT',
                                    'HoistHomeOnPosLT':'LdHoistHomeOnPosLT'})
        dfHiOhtHoistHomeOnPosLd = dfHi
    else:
        dfHiOhtHoistHomeOnPosLd=pd.DataFrame(index=lst_machine)
        dfHiOhtHoistHomeOnPosLd['LdHoistHomeOnPosGT']=100.0
        dfHiOhtHoistHomeOnPosLd['LdHoistHomeOnPosLT']=100.0
        dfHiOhtHoistHomeOnPosLd['total']=100.0
    
    # Hoist Home Off Postion(Unload)
    # FOUP 들었을때만 체크 - STB가 안정적
    # Set
    filter_event={'Type':'TRANS', 'EventID':'ULD MOVE DOWN HOIST FAST', 'StartEnd':'End', 'PortType':5}
    col_list=['Time','MachineID','StationID',
              'Value5']
    dic_Ref={'Hoist Home Off Pos(unload)':['GLT', 6, 8, 3, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        # 동일 Key-name으로 임시 변환 처리
        dfHi = dfHi.rename(columns={'HoistHomeOffPosGT':'UldHoistHomeOffPosGT',
                                    'HoistHomeOffPosLT':'UldHoistHomeOffPosLT'})
        dfHiOhtHoistHomeOffPosUld = dfHi
    else:
        dfHiOhtHoistHomeOffPosUld=pd.DataFrame(index=lst_machine)
        dfHiOhtHoistHomeOffPosUld['UldHoistHomeOffPosGT']=100.0
        dfHiOhtHoistHomeOffPosUld['UldHoistHomeOffPosLT']=100.0
        dfHiOhtHoistHomeOffPosUld['total']=100.0
        
    # Hoist Home On Postion(Unload)
    # FOUP 들었을때만 체크 - EQ가 안정적
    # Set
    filter_event={'Type':'TRANS', 'EventID':'ULD DONE', 'StartEnd':'End', 'PortType':1}
    col_list=['Time','MachineID','StationID',
              'Value3']
    dic_Ref={'Hoist Home on Pos(unload)':['GLT', 6, 8, 3, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        # 동일 Key-name으로 임시 변환 처리
        dfHi = dfHi.rename(columns={'HoistHomeOnPosGT':'UldHoistHomeOnPosGT',
                                    'HoistHomeOnPosLT':'UldHoistHomeOnPosLT'})
        dfHiOhtHoistHomeOnPosUld = dfHi
    else:
        dfHiOhtHoistHomeOnPosUld=pd.DataFrame(index=lst_machine)
        dfHiOhtHoistHomeOnPosUld['UldHoistHomeOnPosGT']=100.0
        dfHiOhtHoistHomeOnPosUld['UldHoistHomeOnPosLT']=100.0
        dfHiOhtHoistHomeOnPosUld['total']=100.0
    
    # Summary
    rename_columns={'LdHoistHomeOffPosGT':'hoist_home_off_position_ldGT',
                    'LdHoistHomeOffPosLT':'hoist_home_off_position_ldLT',
                    'LdHoistHomeOnPosGT':'hoist_home_on_position_ldGT',
                    'LdHoistHomeOnPosLT':'hoist_home_on_position_ldLT',
                    'UldHoistHomeOffPosGT':'hoist_home_off_position_uldGT',
                    'UldHoistHomeOffPosLT':'hoist_home_off_position_uldLT',
                    'UldHoistHomeOnPosGT':'hoist_home_on_position_uldGT',
                    'UldHoistHomeOnPosLT':'hoist_home_on_position_uldLT'}
    
    frames=[dfHiOhtHoistHomeOffPosLd, dfHiOhtHoistHomeOnPosLd,
            dfHiOhtHoistHomeOffPosUld, dfHiOhtHoistHomeOnPosUld]
    dfHiOhtHoistHomePos=pd.concat(frames, axis=1)
    dfHiOhtHoistHomePos=dfHiOhtHoistHomePos.drop(columns=['total'])
    dfHiOhtHoistHomePos=dfHiOhtHoistHomePos.fillna(100.0)
    dfHiOhtHoistHomePos['total']=round(dfHiOhtHoistHomePos.sum(axis=1)/len(rename_columns),0)
    
    group_name='machine'
    df=dfHiOhtHoistHomePos
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiOhtHoistHomePos)
    
    ##########################################################################################################
    # Lookdown Detect
    # 시간만 진단 항목 포함, 위치는 향후 상세 분석 시 활용
    hi_item='lookdown'
    # Set
    filter_event={'Type':'TRANS', 'EventID':'LOOKDOWN DETECT', 'StartEnd':'End'}
    col_list=['Time','MachineID','StationID',
              'Value3']
    dic_Ref={'Lookdown Detect Time':['GT', 0, 0, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiS = DiagType.calcHealthIndexQuickRatio('StationID', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['LookDownStopTimeGT']=100.0
        dfHiM['total']=100.0
        dfHiS=pd.DataFrame(index=lst_station)
        dfHiS['LookDownStopTimeGT']=100.0
        dfHiS['total']=100.0
    
    # Summary
    rename_columns={'LookDownStopTimeGT':'lookdown_detect_timeGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='station'
    df=dfHiS
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiStation=dfn.addHealthIndex(hi_item, dfHiStation, dfHiS)
    
    ##########################################################################################################
    hi_item='oscillation'
    # Oscillation Count
    # 이력이 많은 Unload만 우선 포함
    # Set
    filter_event={'Type':'TRANS', 'EventID':'ULD DONE', 'StartEnd':'End'}
    col_list=['Time','MachineID','StationID',
              'Value2']
    dic_Ref={'Oscillation Count':['GT', 0, 0, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiOhtOscilCnt = dfHi
        dfHi = DiagType.calcHealthIndexQuickRatio('StationID', df)
        dfHiStationOscilCnt = dfHi
    else:
        dfHiOhtOscilCnt=pd.DataFrame(index=lst_machine)
        dfHiOhtOscilCnt['OscillationCountGT']=100.0
        dfHiOhtOscilCnt['total']=100.0
        dfHiStationOscilCnt=pd.DataFrame(index=lst_station)
        dfHiStationOscilCnt['OscillationCountGT']=100.0
        dfHiStationOscilCnt['total']=100.0
    
    # Oscillation Pause Time
    # 이력이 많은 Unload만 우선 포함
    # Set
    filter_event={'Type':'TRANS', 'EventID':'OSCILLATION PAUSE', 'StartEnd':'End'}
    col_list=['Time','MachineID','StationID',
              'Value1']
    dic_Ref={'Oscillation Pause Time':['GT', 0, 0, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHiOhtOscilPauseTime = dfHi
        dfHi = DiagType.calcHealthIndexQuickRatio('StationID', df)
        dfHiStationOscilPauseTime = dfHi
    else:
        dfHiOhtOscilPauseTime=pd.DataFrame(index=lst_machine)
        dfHiOhtOscilPauseTime['OscillationPauseTimeGT']=100.0
        dfHiOhtOscilPauseTime['total']=100.0
        dfHiStationOscilPauseTime=pd.DataFrame(index=lst_station)
        dfHiStationOscilPauseTime['OscillationPauseTimeGT']=100.0
        dfHiStationOscilPauseTime['total']=100.0
    
    # Summary
    rename_columns={'OscillationCountGT':'oscillation_countGT',
                    'OscillationPauseTimeGT':'oscillation_pause_timeGT'}
    
    frames=[dfHiOhtOscilCnt, dfHiOhtOscilPauseTime]
    dfHiOhtOscillation=pd.concat(frames, axis=1)
    dfHiOhtOscillation=dfHiOhtOscillation.drop(columns=['total'])
    dfHiOhtOscillation=dfHiOhtOscillation.fillna(100.0)
    dfHiOhtOscillation['total']=round(dfHiOhtOscillation.sum(axis=1)/len(rename_columns),0)
    
    
    group_name='machine'
    df=dfHiOhtOscillation
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
    frames=[dfHiStationOscilCnt, dfHiStationOscilPauseTime]
    dfHiStationOscillation=pd.concat(frames, axis=1)
    dfHiStationOscillation=dfHiStationOscillation.drop(columns=['total'])
    dfHiStationOscillation=dfHiStationOscillation.fillna(100.0)
    dfHiStationOscillation['total']=round(dfHiStationOscillation.sum(axis=1)/len(rename_columns),0)
    
    group_name='station'
    df=dfHiStationOscillation
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiOhtOscillation)
    dfHiStation=dfn.addHealthIndex(hi_item, dfHiStation, dfHiStationOscillation)
    
    ##########################################################################################################
    # Rotate Sensor
    hi_item='rotate_sensor'
    # Set
    filter_event={'Type':'TRANS', 'EventID':'ROTATE', 'StartEnd':'End'}
    col_list=['Time','MachineID','StationID',
              'Value1']
    dic_Ref={'Rotate0 On Degree':['GT', 0, 2, 0, 1],
             'Rotate0 Off Degree':['GT', 0, 2, 0, 1],
             'Rotate180 On Degree':['LT', 180, 180, 178, 1],
             'Rotate180 Off Degree':['LT', 180, 180, 178, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    else:
        dfHiM=pd.DataFrame(index=lst_machine)
        dfHiM['Rotate0OnDegreeGT']=100.0
        dfHiM['Rotate0OffDegreeGT']=100.0
        dfHiM['Rotate180OnDegreeLT']=100.0
        dfHiM['Rotate180OffDegreeLT']=100.0    
        dfHiM['total']=100.0
    
    # Summary
    rename_columns={'Rotate0OnDegreeGT':'rotate_sensor_0_on_degreeGT',
                    'Rotate0OffDegreeGT':'rotate_sensor_0_off_degreeGT',
                    'Rotate180OnDegreeLT':'rotate_sensor_180_on_degreeLT',
                    'Rotate180OffDegreeLT':'rotate_sensor_180_off_degreeLT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    
    ##########################################################################################################
    # Slide Unload Position Error
    hi_item='slide_position'
    filter_event={'Type':'TRANS', 'EventID':'ULD MOVE SHIFTROTATE HOME', 'StartEnd':'End'}
    col_list=['Time','MachineID','StationID',
              'Value2']
    dic_Ref={'Slide Unload Position Error':['GLT', 0, 0.2, -0.2, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHi = dfHi.rename(columns={'ShiftPosErrGT':'SlideUnloadPositionErrorGT',
                                    'ShiftPosErrLT':'SlideUnloadPositionErrorLT'})
        dfHiOhtSlideUldPosErr = dfHi
    else:
        dfHiOhtSlideUldPosErr=pd.DataFrame(index=lst_machine)
        dfHiOhtSlideUldPosErr['total']=100.0
        lst_col=['SlideUnloadPositionErrorGT','SlideUnloadPositionErrorLT']
        for l in lst_col:
            dfHiOhtSlideUldPosErr[l]=100.0
    
    # Slide Load Position Error
    filter_event={'Type':'TRANS', 'EventID':'LD MOVE SHIFTROTATE', 'StartEnd':'End'}
    col_list=['Time','MachineID','StationID',
              'Value2']
    dic_Ref={'Slide Load Position Error':['GLT', 0, 0.2, -0.2, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHi = dfHi.rename(columns={'ShiftPosErrGT':'SlideLoadPositionErrorGT',
                                    'ShiftPosErrLT':'SlideLoadPositionErrorLT'})
        dfHiOhtSlideLdPosErr = dfHi
    else:
        dfHiOhtSlideLdPosErr=pd.DataFrame(index=lst_machine)
        dfHiOhtSlideLdPosErr['total']=100.0
        lst_col=['SlideLoadPositionErrorGT','SlideLoadPositionErrorLT']
        for l in lst_col:
            dfHiOhtSlideLdPosErr[l]=100.0    
        
    # Summary
    rename_columns={'SlideUnloadPositionErrorGT':'slide_uld_position_errorGT',
                    'SlideUnloadPositionErrorLT':'slide_uld_position_errorLT',
                    'SlideLoadPositionErrorGT':'slide_ld_position_errorGT',
                    'SlideLoadPositionErrorLT':'slide_ld_position_errorLT'}
    
    frames=[dfHiOhtSlideUldPosErr, dfHiOhtSlideLdPosErr]
    dfHiOhtSlidePosErr=pd.concat(frames, axis=1)
    dfHiOhtSlidePosErr=dfHiOhtSlidePosErr.drop(columns=['total'])
    dfHiOhtSlidePosErr=dfHiOhtSlidePosErr.fillna(100.0)
    dfHiOhtSlidePosErr['total']=round(dfHiOhtSlidePosErr.sum(axis=1)/len(rename_columns),0)
    
    group_name='machine'
    df=dfHiOhtSlidePosErr
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiOhtSlidePosErr)
    
    ##########################################################################################################
    hi_item='slide_home'
    # Slide Home Off Position - Unload LSTB
    filter_event={'Type':'TRANS', 'EventID':'LD MOVE SHIFTROTATE', 'StartEnd':'End', 'PortType':5}
    col_list=['Time','MachineID','StationID',
              'Value3']
    dic_Ref={'Slide Home Off Position L':['GLT', -5.5, -4.5, -6.5, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHi = dfHi.rename(columns={'ShiftHomeOffPosGT':'LstbShiftHomeOffPosGT',
                                    'ShiftHomeOffPosLT':'LstbShiftHomeOffPosLT'})
        dfHiOhtSlideHomeOffPosLSTB = dfHi    
    else:
        dfHiOhtSlideHomeOffPosLSTB=pd.DataFrame(index=lst_machine)
        dfHiOhtSlideHomeOffPosLSTB['LstbShiftHomeOffPosGT']=100.0
        dfHiOhtSlideHomeOffPosLSTB['LstbShiftHomeOffPosLT']=100.0
        dfHiOhtSlideHomeOffPosLSTB['total']=100.0
        
    # Slide Home Off Position - Unload RSTB
    filter_event={'Type':'TRANS', 'EventID':'LD MOVE SHIFTROTATE', 'StartEnd':'End', 'PortType':6}
    col_list=['Time','MachineID','StationID',
              'Value3']
    dic_Ref={'Slide Home Off Position R':['GLT', 5.5, 6.5, 4.5, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHi = dfHi.rename(columns={'ShiftHomeOffPosGT':'RstbShiftHomeOffPosGT',
                                    'ShiftHomeOffPosLT':'RstbShiftHomeOffPosLT'})
        dfHiOhtSlideHomeOffPosRSTB = dfHi
    else:
        dfHiOhtSlideHomeOffPosRSTB=pd.DataFrame(index=lst_machine)
        dfHiOhtSlideHomeOffPosRSTB['RstbShiftHomeOffPosGT']=100.0
        dfHiOhtSlideHomeOffPosRSTB['RstbShiftHomeOffPosLT']=100.0
        dfHiOhtSlideHomeOffPosRSTB['total']=100.0
    
    # Slide Home On Position - Unload LSTB
    filter_event={'Type':'TRANS', 'EventID':'ULD MOVE SHIFTROTATE HOME', 'StartEnd':'End', 'PortType':5}
    col_list=['Time','MachineID','StationID',
              'Value3']
    dic_Ref={'Slide Home On Position L':['GLT', -4.5, -3.5, -5.5, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHi = dfHi.rename(columns={'ShiftHomeOnPosGT':'LstbShiftHomeOnPosGT',
                                    'ShiftHomeOnPosLT':'LstbShiftHomeOnPosLT'})
        dfHiOhtSlideHomeOnPosLSTB = dfHi
    else:
        dfHiOhtSlideHomeOnPosLSTB=pd.DataFrame(index=lst_machine)
        dfHiOhtSlideHomeOnPosLSTB['LstbShiftHomeOnPosGT']=100.0
        dfHiOhtSlideHomeOnPosLSTB['LstbShiftHomeOnPosLT']=100.0
        dfHiOhtSlideHomeOnPosLSTB['total']=100.0
    
    # Slide Home On Position - Unload RSTB
    filter_event={'Type':'TRANS', 'EventID':'ULD MOVE SHIFTROTATE HOME', 'StartEnd':'End', 'PortType':6}
    col_list=['Time','MachineID','StationID',
              'Value3']
    dic_Ref={'Slide Home On Position R':['GLT', 4.5, 5.5, 3.5, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHi = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        dfHi = dfHi.rename(columns={'ShiftHomeOnPosGT':'RstbShiftHomeOnPosGT',
                                    'ShiftHomeOnPosLT':'RstbShiftHomeOnPosLT'})
        dfHiOhtSlideHomeOnPosRSTB = dfHi
    else:
        dfHiOhtSlideHomeOnPosRSTB=pd.DataFrame(index=lst_machine)
        dfHiOhtSlideHomeOnPosRSTB['RstbShiftHomeOnPosGT']=100.0
        dfHiOhtSlideHomeOnPosRSTB['RstbShiftHomeOnPosLT']=100.0
        dfHiOhtSlideHomeOnPosRSTB['total']=100.0
    
    # Summary
    rename_columns={'LstbShiftHomeOffPosGT':'slide_home_off_position_lstbGT',
                    'LstbShiftHomeOffPosLT':'slide_home_off_position_lstbLT',
                    'RstbShiftHomeOffPosGT':'slide_home_off_position_rstbGT',
                    'RstbShiftHomeOffPosLT':'slide_home_off_position_rstbLT',
                    'LstbShiftHomeOnPosGT':'slide_home_on_position_lstbGT',
                    'LstbShiftHomeOnPosLT':'slide_home_on_position_lstbLT',
                    'RstbShiftHomeOnPosGT':'slide_home_on_position_rstbGT',
                    'RstbShiftHomeOnPosLT':'slide_home_on_position_rstbLT'}
    
    frames=[dfHiOhtSlideHomeOffPosLSTB, dfHiOhtSlideHomeOffPosRSTB,
            dfHiOhtSlideHomeOnPosLSTB, dfHiOhtSlideHomeOnPosRSTB]
    dfHiOhtSlideHomePos=pd.concat(frames, axis=1)
    dfHiOhtSlideHomePos=dfHiOhtSlideHomePos.drop(columns=['total'])
    dfHiOhtSlideHomePos=dfHiOhtSlideHomePos.fillna(100.0)
    dfHiOhtSlideHomePos['total']=round(dfHiOhtSlideHomePos.sum(axis=1)/len(rename_columns),0)
    
    group_name='machine'
    df=dfHiOhtSlideHomePos
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
        
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiOhtSlideHomePos)
    
    ##########################################################################################################
    # AMC Voltage
    hi_item='amc_voltage'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 1s'}
    col_list=['Time','MachineID','NodeID',
              'Value3','Value4','Value5','Value6']
    dic_Ref={'3V':['GLT', 327, 350, 310, 1],
             '5V':['GLT', 515, 525, 500, 1],
             '+12V':['GLT', 1199, 1201, 1199, 1],
             '-12V':['GLT', 1199, 1201, 1199, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    
    # Summary
    rename_columns={'3VGT':'amc_3VGT','3VLT':'amc_3VLT',
                    '5VGT':'amc_5VGT','5VLT':'amc_5VLT',
                    '+12VGT':'amc_12VpGT','+12VLT':'amc_12VpLT',
                    '-12VGT':'amc_12VmGT','-12VLT':'amc_12VmLT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # IPC State
    hi_item='ipc_monitor'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 1s'}
    col_list=['Time','MachineID','NodeID',
              'Value17','Value18']
    dic_Ref={'IPC Temp':['GT', 40.0, 50.0, 0, 1],
             'IPC Voltage':['GLT', 5.2, 6.0, 5.0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    
    # Summary
    rename_columns={'IPCTempGT':'ipc_temperatureGT',
                    'IPCVoltageGT':'ipc_voltageGT','IPCVoltageLT':'ipc_voltageLT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    
    ##########################################################################################################
    # Drive Following
    hi_item='drive_follow'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 1s'}
    col_list=['Time','MachineID','NodeID',
              'Value19']
    dic_Ref={'Drv Follow Err':['GT', 0, 10, -10, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    
    # Summary
    rename_columns={'DrivingFollowingErrGT':'drive_follow_errorGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # Drive Toque
    hi_item='drive_toque'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 1s'}
    col_list=['Time','MachineID','NodeID',
              'Value1','Value2']
    dic_Ref={'Front Toque':['GLT', 0, 300, -3000, 1],
             'Rear Toque':['GLT', 0, 300, -300, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    
    # Summary
    rename_columns={'DrivingFrontTorqueGT':'drive_toque_frontGT',
                    'DrivingFrontTorqueLT':'drive_toque_frontLT',
                    'DrivingRearTorqueGT':'drive_toque_rearGT',
                    'DrivingRearTorqueLT':'drive_toque_rearLT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # Ragulator Voltage
    hi_item='regulator_voltage'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 1s'}
    col_list=['Time','MachineID','NodeID',
              'Value7','Value8','Value9']
    dic_Ref={'Edc Voltage':['GLT', 300, 355, 150, 1],
             'Ecap Voltage':['GLT', 300, 355, 150, 1],
             'Eout Voltage':['GLT', 300, 355, 150, 1],
             'EdcEout Diff Voltage':['GLT', 0, 15, -15, 1],
             'EdcEcap Diff Voltage':['GLT', 0, 15, -15, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    # Original 외 가공 데이터 참조 시 별도 분석(calcHealthIndexQuickRatioEx) 진행... 
    lst_diag_col=['Edc', 'Ecap','Eout',
                  'regulator_voltage_diff_EdcEout','regulator_voltage_diff_EdcEcap']
    df['regulator_voltage_diff_EdcEout']=df['Edc']-df['Eout']
    df['regulator_voltage_diff_EdcEcap']=df['Edc']-df['Ecap']
    dic_RefEx={'Edc':['GLT', 300, 355, 150, 1],
               'Ecap':['GLT', 300, 355, 150, 1],
               'Eout':['GLT', 300, 355, 150, 1],
               'regulator_voltage_diff_EdcEout':['GLT', 0, 15, -15, 1],
               'regulator_voltage_diff_EdcEcap':['GLT', 0, 15, -15, 1]}
    dfRefEx=pd.DataFrame(data=dic_RefEx)
    dfRefEx.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    dfHiM = DiagType.calcHealthIndexQuickRatioEx('MachineID', df, lst_diag_col, dfRefEx)
    dfHiN = DiagType.calcHealthIndexQuickRatioEx('NodeID', df, lst_diag_col, dfRefEx)
    
    # Summary
    rename_columns={'EdcGT':'regulator_voltage_EdcGT','EdcLT':'regulator_voltage_EdcLT',
                    'EcapGT':'regulator_voltage_EcapGT','EcapLT':'regulator_voltage_EcapLT',
                    'EoutGT':'regulator_voltage_EoutGT','EoutLT':'regulator_voltage_EoutLT',
                    'regulator_voltage_diff_EdcEoutGT':'regulator_voltage_diff_EdcEoutGT',
                    'regulator_voltage_diff_EdcEoutLT':'regulator_voltage_diff_EdcEoutLT',
                    'regulator_voltage_diff_EdcEcapGT':'regulator_voltage_diff_EdcEcapGT',
                    'regulator_voltage_diff_EdcEcapLT':'regulator_voltage_diff_EdcEcapLT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # Ragulator Current
    hi_item='regulator_current'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 1s'}
    col_list=['Time','MachineID','NodeID',
              'Value10','Value11']
    dic_Ref={'Iout Ampere':['GLT', 10, 90, 0, 1], # 진선 기준
             'Iac Ampere':['GLT', 45, 65, 0, 1]} # 직선 기준
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    
    # Summary
    rename_columns={'IoutGT':'regulator_current_IoutGT','IoutLT':'regulator_current_IoutLT',
                    'IacGT':'regulator_current_IacGT','IacLT':'regulator_current_IacLT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # Ragulator
    hi_item='regulator_temperature'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 1s'}
    col_list=['Time','MachineID','NodeID',
              'Value12','Value13','Value14','Value15','Value16']
    dic_Ref={'Pickup Temp':['GT', 45, 50, 0, 1],
             'IGB Temp':['GT', 45, 60, 0, 1],
             'Ecap Temp':['GT', 45, 50, 0, 1],
             'Ecap2 Temp':['GT', 45, 50, 0, 1],
             'EcapRound Temp':['GT', 45, 50, 0, 1],
             'Ecap Diff Temp':['GT', 0, 8, -8, 1],
             'Ecap2 Diff Temp':['GT', 0, 8, -8, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    # Original 외 가공 데이터 참조 시 별도 분석(calcHealthIndexQuickRatioEx) 진행... 
    lst_diag_col=['PickUpTemp', 'IGBTemp','ECapTemp','Ecap2Temp','EcapRoundTemp',
                  'regulator_temperature_diff_Ecap','regulator_temperature_diff_Ecap2']
    df['regulator_temperature_diff_Ecap']=df['ECapTemp']-df['EcapRoundTemp']
    df['regulator_temperature_diff_Ecap2']=df['Ecap2Temp']-df['EcapRoundTemp']
    dic_RefEx={'Pickup Temp':['GT', 45, 50, 0, 1],
               'IGB Temp':['GT', 45, 60, 0, 1],
               'Ecap Temp':['GT', 45, 50, 0, 1],
               'Ecap2 Temp':['GT', 45, 50, 0, 1],
               'EcapRound Temp':['GT', 45, 50, 0, 1],           
               'regulator_temperature_diff_Ecap':['GT', 0, 8, -8, 1],
               'regulator_temperature_diff_Ecap2':['GT', 0, 8, -8, 1]}
    dfRefEx=pd.DataFrame(data=dic_RefEx)
    dfRefEx.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    dfHiM = DiagType.calcHealthIndexQuickRatioEx('MachineID', df, lst_diag_col, dfRefEx)
    dfHiN = DiagType.calcHealthIndexQuickRatioEx('NodeID', df, lst_diag_col, dfRefEx)
    
    # Summary
    rename_columns={'PickUpTempGT':'regulator_temperature_pickupGT','IGBTempGT':'regulator_temperature_IGBGT',
                    'ECapTempGT':'regulator_temperature_EcapGT','Ecap2TempGT':'regulator_temperature_Ecap2GT',
                    'EcapRoundTempGT':'regulator_temperature_EcapRoundGT',
                    'regulator_temperature_diff_EcapGT':'regulator_temperature_diff_EcapGT',
                    'regulator_temperature_diff_Ecap2GT':'regulator_temperature_diff_Ecap2GT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    # Summary
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # Hoist Following
    hi_item='hoist_follow'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 1s'}
    col_list=['Time','MachineID','NodeID',
              'Value20']
    dic_Ref={'Hoist Follow Err':['GLT', 0, 10, -10, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    
    # Summary
    rename_columns={'HoistFollowingErrGT':'hoist_follow_errorGT',
                    'HoistFollowingErrLT':'hoist_follow_errorLT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # Slide Following
    hi_item='slide_follow'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 1s'}
    col_list=['Time','MachineID','NodeID',
              'Value21']
    dic_Ref={'Slide Follow Error':['GLT', 0, 0.5, -0.5, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    
    
    # Summary
    rename_columns={'ShiftFollowingErrGT':'slide_follow_errorGT',
                    'ShiftFollowingErrLT':'slide_follow_errorLT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # Wireless RSSI
    hi_item='rssi'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 1s'}
    col_list=['Time','MachineID','NodeID',
              'Value22']
    dic_Ref={'RSSI':['GT', -55, -40, -100, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    
    # Summary
    rename_columns={'RSSIGT':'rssi_levelGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # AMC
    hi_item='amc_comm'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 5s'}
    col_list=['Time','MachineID','NodeID',
              'Value10']
    dic_Ref={r'AMC commfail Count':['GT', 0, 1, 0, 1]}
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    
    # Summary
    rename_columns={'AMCcommfailGT':'amc_comm_failGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    
    ##########################################################################################################
    # Fan Fail
    hi_item='fan_fail'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 5s'}
    col_list=['Time','MachineID','NodeID',
              'Value3']
    dic_Ref={'Fan Fail':['GT', 0, 0, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    
    # Summary
    rename_columns={'FanFailGT':'fan_fail_countGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    
    ##########################################################################################################
    # Hand Comm. Fail
    hi_item='hand_comm'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 5s'}
    col_list=['Time','MachineID','NodeID',
              'Value11']
    dic_Ref={'Hand Comm Fail':['GT', 0, 0, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    dfHiN = DiagType.calcHealthIndexQuickRatio('NodeID', df)
    
    # Summary
    rename_columns={'HandPIOCommfailCountGT':'hand_comm_failGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    group_name='node'
    df=dfHiN
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    dfHiNode=dfn.addHealthIndex(hi_item, dfHiNode, dfHiN)
    
    ##########################################################################################################
    # Memory & CPU Load
    hi_item='ipc_performance'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 5s'}
    col_list=['Time','MachineID','NodeID',
              'Value1','Value2']
    dic_Ref={'Memory Load':['GT', 20, 80, 0, 1],
             'CPU Load':['GT', 10, 50, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    
    # Summary
    rename_columns={'CPUGT':'ipc_cpu_usageGT','MemoryGT':'ipc_memory_usageGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)
    
    ##########################################################################################################
    # Thread Cycle
    # 주요한 Cycle만 우선 포함
    hi_item='thread_cycle'
    # Set
    filter_event={'Type':'STS', 'EventID':'PERIODIC 5s'}
    col_list=['Time','MachineID','NodeID',
              'Value4','Value5','Value6','Value7','Value8','Value9']
    dic_Ref={'Log Cycle':['GT', 1000, 10000, 0, 1],
             'Task Cycle':['GT', 16, 80, 0, 1],
             'Status Cycle':['GT', 16, 80, 0, 1],
             'Excute Cycle':['GT', 16, 80, 0, 1],
             'ExtraJob Cycle':['GT', 16, 80, 0, 1],
             'AMC Cycle':['GT', 16, 30, 0, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    dfHiM = DiagType.calcHealthIndexQuickRatio('MachineID', df)
    
    # Summary
    rename_columns={'LogThreadGT':'thread_cycle_logGT',
                    'TaskControlThreadGT':'thread_cycle_taskGT',
                    'StatusCheckThreadGT':'thread_cycle_statusGT',               
                    'ExcuteThreadGT':'thread_cycle_excuteGT',                
                    'ExtraJobThreadGT':'thread_cycle_extrajobGT',
                    'AMCDLLlibThreadGT':'thread_cycle_amcGT'}
    
    group_name='machine'
    df=dfHiM
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiM)

    ##########################################################################################################
    hi_item='hand_movetime'
    # Hand Grip Time
    # Set
    filter_event={'Type':'TRANS', 'EventID':'ULD GRIP', 'StartEnd':'End'}
    col_list=['Time','MachineID','StationID',
              'Value2']
    dic_Ref={'hand_grip_movetime':['GLT', 1000, 1100, 900, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiOhtHandGripTime = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        
        key_name='hand_grip_movetime'
        df=df.rename(columns={'GripTime':key_name})
        dfCpkOhtHandGripTime = dfn.calc_cpk(df, key_name, ref, analysis_time_db)
        dfn.my_plot_cpk(key_name, df, {'x':'MachineID', 'y':key_name},(16,10),1,\
                        {'min':0, 'max':1200}, {'min':800, 'max':1000, 'ref':900}, True,\
                        f'{file_path}{analysis_time_file}_{key_name}.png', ref, dfCpkOhtHandGripTime)
    else:
        dfHiOhtHandGripTime=pd.DataFrame(index=lst_machine)
        dfHiOhtHandGripTime['total']=100.0
    
    # Hand Release Time
    # Set
    filter_event={'Type':'TRANS', 'EventID':'LD RELEASE', 'StartEnd':'End'}
    col_list=['Time','MachineID','StationID',
              'Value2']
    dic_Ref={'hand_release_movetime':['GLT', 1000, 1100, 900, 1]}
    
    dfRef=pd.DataFrame(data=dic_Ref)
    dfRef.index=['Logic', 'Ref', 'Max', 'Min', 'Weight']
    
    # Logic
    dfRaw=get_df_byType(filter_event['Type'])
    DiagType=dfn.DiagType1(filter_event, col_list, dfRef)
    df = DiagType.filterRaw(dfRaw)
    if(len(df)>0):
        dfHiOhtHandReleaseTime = DiagType.calcHealthIndexQuickRatio('MachineID', df)
        
        key_name='hand_release_movetime'
        df=df.rename(columns={'ReleaseTime':key_name})
        dfCpkOhtHandReleaseTime = dfn.calc_cpk(df, key_name, ref, analysis_time_db)
        dfn.my_plot_cpk(key_name, df, {'x':'MachineID', 'y':key_name},(16,10),1,\
                        {'min':0, 'max':1200}, {'min':800, 'max':1000, 'ref':900}, True,\
                        f'{file_path}{analysis_time_file}_{key_name}.png', ref, dfCpkOhtHandReleaseTime)
    else:
        dfHiOhtHandReleaseTime=pd.DataFrame(index=lst_machine)
        dfHiOhtHandReleaseTime['total']=100.0   
   
    # Summary
    rename_columns={'GripTimeGT':'hand_grip_movetimeGT',
                    'GripTimeLT':'hand_grip_movetimeLT',
                    'ReleaseTimeGT':'hand_release_movetimeGT',
                    'ReleaseTimeLT':'hand_release_movetimeLT'}
    
    frames=[dfHiOhtHandGripTime, dfHiOhtHandReleaseTime]
    dfHiOhtHandMove=pd.concat(frames, axis=1)
    dfHiOhtHandMove=dfHiOhtHandMove.drop(columns=['total'])
    dfHiOhtHandMove=dfHiOhtHandMove.fillna(100.0)
    dfHiOhtHandMove['total']=round(dfHiOhtHandMove.sum(axis=1)/len(rename_columns),0)
    
    group_name='machine'
    df=dfHiOhtHandMove
    table_name='ivalue_'+group_name+'_'+hi_item
    ddb.insert_df2grouptable(ddb.db_name, df, group_name, table_name, rename_columns, analysis_time_db)    
    
    table_name='cpk_machine'
    df=dfCpkOhtHandGripTime
    ddb.insert_df2table(ddb.db_name, df, table_name)
        
    df=dfCpkOhtHandReleaseTime
    ddb.insert_df2table(ddb.db_name, df, table_name)
    
    dfHiOht=dfn.addHealthIndex(hi_item, dfHiOht, dfHiOhtShutter)
    ##########################################################################################################
    
    
    return [dfHiOht, dfHiNode, dfHiStation]

        
###############################################################################
#Health Index 기반 ResultOHT Level 부여
def ResultLevel(df_HI):
    if df_HI['Health Index']<=Need:
        return 'Need'
    elif df_HI['Health Index']<=Recommand:
        return 'Recommand'
    else: 
        return 'Normal'


def calc_total_indexing(dfHiOht, dfHiNode, dfHiStation):
    ###############################################################################
    # Machine ID 기반 통합 Health Index 도출
    if(len(dfHiOht)==0):
        logger.warning('Health Index by MachineID has no item..!!')
    else:
        dfHiOht['Health Index']=round(dfHiOht.sum(axis=1)/len(dfHiOht.columns),1)
        dfHiOht.sort_values(by='Health Index',ascending=True)
        
        dfHiOht['Result']=dfHiOht.apply(ResultLevel,axis=1)
        dfHiOht=dfHiOht.sort_values(by='Health Index',ascending=True)
    
    ###############################################################################
    # Node ID 기반 통합 Health Index 도출
    if(len(dfHiNode)==0):
        logger.warning('Health Index by NodeID has no item..!!')
    else:
        dfHiNode=dfHiNode.fillna(100.0)
        dfHiNode['Health Index']=round(dfHiNode.sum(axis=1)/len(dfHiNode.columns),1)
        dfHiNode.sort_values(by='Health Index',ascending=True)
        
        dfHiNode['Result']=dfHiNode.apply(ResultLevel,axis=1)
        dfHiNode=dfHiNode.sort_values(by='Health Index',ascending=True)
    
    ###############################################################################
    # Station ID 기반 통합 Health Index 도출
    if(len(dfHiStation)==0):
        logger.warning('Health Index by StationID has no item..!!')
    else:
        dfHiStation=dfHiStation.fillna(100.0)
        dfHiStation['Health Index']=round(dfHiStation.sum(axis=1)/len(dfHiStation.columns),1)
        dfHiStation.sort_values(by='Health Index',ascending=True)
    
        dfHiStation['Result']=dfHiStation.apply(ResultLevel,axis=1)
        dfHiStation=dfHiStation.sort_values(by='Health Index',ascending=True)
        
    return [dfHiOht, dfHiNode, dfHiStation]


def exec_db_update(dfHiOht, dfHiNode, dfHiStation, dfRawT):
    ###############################################################################
    # dbms save
    table_name='history'
    logger.info('start update table of db: %s', table_name)
    append_lst=[]
    append_lst.append(analysis_time_db)
    append_lst.append(ArgSite)
    append_lst.append(ArgGroup)
    append_lst.append(from_date)
    append_lst.append(to_date)
    append_lst.append(len(dfRawT[(dfRawT['PortType']==1) &
                                 (dfRawT['EventID']=='LD DONE') &                             
                                 (dfRawT['StartEnd']=='End')]))
    append_lst.append(len(dfRawT[(dfRawT['PortType']==3) &
                                 (dfRawT['EventID']=='LD DONE') &                             
                                 (dfRawT['StartEnd']=='End')]))
    append_lst.append(len(dfRawT[(dfRawT['PortType']==5) &
                                 (dfRawT['EventID']=='LD DONE') &                             
                                 (dfRawT['StartEnd']=='End')]))
    append_lst.append(len(dfRawT[(dfRawT['PortType']==6) &
                                 (dfRawT['EventID']=='LD DONE') &                             
                                 (dfRawT['StartEnd']=='End')]))
    append_lst.append(len(dfStatus5m['MachineID'].unique()))
    append_lst.append(len(dfHiOht[dfHiOht['Health Index']<Need]))
    append_lst.append(len(dfRawD['NodeID'].unique()))
    append_lst.append(len(dfHiNode[dfHiNode['Health Index']<Need]))
    append_lst.append(len(dfRawT['StationID'].unique()))
    append_lst.append(len(dfHiStation[dfHiStation['Health Index']<Need]))
    append_lst.append('db Testing')
    
    df_db=ddb.get_table_columns(ddb.db_name, table_name)
    if(len(df_db.columns) == len(append_lst)):
        dic={}
        for d, i in zip(df_db.columns.to_list(),range(len(df_db.columns.to_list()))):
            dic[d]=append_lst[i]
        
        df_db=df_db.append(dic, ignore_index=True)    
        ddb.insert_history(ddb.db_name, df_db)
        logger.info('complete')
    else:
        logger.warning('missmatch table[%s] columns count..!! - Set(%d)/Real(%d)',\
                       table_name, len(df_db.columns), len(append_lst))
    
    ###############################################################################
    table_name='index_machine'
    logger.info('start update table of db: %s', table_name)
    #rename - column 정리 , 나중에 Table로 관리.. 현재는 기본 Key Name으로 하고 있음.. 
    df=ddb.get_table_columns(ddb.db_name, table_name)
    df_db=dfHiOht.sort_index(axis=1)
    df_db=df_db.drop(columns={'Result'})
    df_db=df_db.rename(columns={'Health Index':'total'})
    df_db['analysis_time']=analysis_time_db
    df_db=df_db.assign(id_machine=df_db.index.to_list())
    df_db=df_db.fillna(100.0)
    
    df=ddb.get_table_columns(ddb.db_name, table_name)
    if(len(df.columns) == len(df_db.columns)):
        ddb.insert_df2table(ddb.db_name, df_db, table_name)
        logger.info('complete')
    else:
        logger.warning('missmatch table[%s] columns count..!! - Set(%d)/Real(%d)',\
                       table_name, len(df_db.columns), len(df.columns))
    
    
    ###############################################################################
    table_name='index_node'
    logger.info('start update table of db: %s', table_name)
    #rename - column 정리 , 나중에 Table로 관리.. 현재는 기본 Key Name으로 하고 있음.. 
    df=ddb.get_table_columns(ddb.db_name, table_name)
    df_db=dfHiNode.sort_index(axis=1)
    df_db=df_db.drop(columns={'Result'})
    df_db=df_db.rename(columns={'Health Index':'total'})
    df_db['analysis_time']=analysis_time_db
    df_db=df_db.assign(id_node=df_db.index.to_list())
    df_db=df_db.fillna(100.0)
    
    df=ddb.get_table_columns(ddb.db_name, table_name)
    if(len(df.columns) == len(df_db.columns)):
        ddb.insert_df2table(ddb.db_name, df_db, table_name)
        logger.info('complete')
    else:
        logger.warning('missmatch table[%s] columns count..!! - Set(%d)/Real(%d)',\
                       table_name, len(df_db.columns), len(df.columns))
    
    #############################################################################################################
    table_name='index_station'    
    logger.info('start update table of db: %s', table_name)
    #rename - column 정리 , 나중에 Table로 관리.. 현재는 기본 Key Name으로 하고 있음.. 
    df=ddb.get_table_columns(ddb.db_name, table_name)
    df_db=dfHiStation.sort_index(axis=1)
    df_db=df_db.drop(columns={'Result'})
    df_db=df_db.rename(columns={'Health Index':'total'})
    df_db['analysis_time']=analysis_time_db
    df_db=df_db.assign(id_station=df_db.index.to_list())
    df_db=df_db.fillna(100.0)
    
    df=ddb.get_table_columns(ddb.db_name, table_name)
    if(len(df.columns) == len(df_db.columns)):
        ddb.insert_df2table(ddb.db_name, df_db, table_name)
        logger.info('complete')
    else:
        logger.warning('missmatch table[%s] columns count..!! - Set(%d)/Real(%d)',\
                       table_name, len(df_db.columns), len(df.columns))    

    #############################################################################################################
    # Save report - db table export
    # 지금은 모든 DB Table을 다가져옴.. 일단 미사용
    # analysis_time 기준 가져오게 업데이트 필요,
    # analysis_time 없는 table 예외 처리 필요
    is_report=False
    if(is_report==True):
        logger.info('Save db to excel report start..')
        filename=analysis_time_file+'_Diag Report_'+ArgGroup+'('+from_date+'_'+to_date+')'
        if not os.path.isdir(fpDiagReport):
            os.mkdir(fpDiagReport)
            
        ddb.select_all_to_excel(ddb.db_name, fpDiagReport, filename)
        logger.info('Save db to excel report complete..!!')

###############################################################################
if __name__ == '__main__':
    # initial
    dt_now=dt.datetime.now()
    analysis_time_log=dt_now.strftime('%Y%m%d_%H%M%S')
    analysis_time_db=dt_now.strftime('%Y-%m-%d %H:%M:%S')
    analysis_time_file=dt_now.strftime('%Y-%m-%d %H%M%S')
    
    dfRawD=dfRawS=dfRawT=dfStatus1s=dfStatus5s=dfStatus5m=pd.DataFrame()
    dfHiOht=dfHiNode=dfHiStation=pd.DataFrame()
    
    logger = logging.getLogger('main')       

    # set logger
    set_logger()
    logger.info('(Step) mcc analysis start..!!')
    
    # check args
    args=check_args()

    # set args
    ArgSite = args[1]
    ArgGroup = args[2]
    from_date = args[3]
    to_date = args[4]
    ArgDate = from_date
    lst_vhl=str(args[5]).split(',')       
    
    # set option
    dic_opt=set_option()
    
    # set path
    fpMCCLog = r'D:/LogMCCRaw/'
    fpDiagData = r'D:/DiagMCCData/'
    fpDiagReport = fpDiagData + ArgSite + '/Report/'
    fpPklDump = fpDiagData + ArgSite + '/Temp/'
    fpFigureCpk = r'D:/grafana/grafana/public/DiagMCCData/'+ArgSite+'/Figure_Cpk/'+\
                  analysis_time_file+'_'+ArgGroup+'_'+from_date+'_'+to_date+'/'
    
    # Check & Create Site Folder
    fdir=r'D:/DiagMCCData/'+ArgSite
    if not os.path.isdir(fdir): # One-Time
        logger.info('Make Site Directory: %s',fdir)
        os.mkdir(fdir)
        fdir=r'D:/DiagMCCData/'+ArgSite+'/Data_Day/'
        os.mkdir(fdir)
        fdir=r'D:/DiagMCCData/'+ArgSite+'/Data/'
        os.mkdir(fdir)
        fdir=r'D:/DiagMCCData/'+ArgSite+'/Report/'
        os.mkdir(fdir)
        fdir=r'D:/grafana/grafana/public/DiagMCCData/'+ArgSite
        os.mkdir(fdir)
        fdir=r'D:/grafana/grafana/public/DiagMCCData/'+ArgSite+'/Figure_Cpk/'
        os.mkdir(fdir)

    # everytime - based analysistime
    os.mkdir(fpFigureCpk)

    # Loading sequence
    load_exist_pickles = False #test    
    if(load_exist_pickles & os.path.isfile(f'{fpPklDump}dfRawD_{from_date}_{to_date}.pklz')):
        logger.info('Test -- Load exist pickle')
        logger.info('Start load dfRawD pickles')
        dfRawD=dfn.LoadPickleFile(f'{fpPklDump}dfRawD_{from_date}_{to_date}.pklz', True)
        logger.info('Start load dfRawT pickle')
        dfRawT=dfn.LoadPickleFile(f'{fpPklDump}dfRawT_{from_date}_{to_date}.pklz', True)
        logger.info('Start load dfRawS pickle')
        dfRawS=dfn.LoadPickleFile(f'{fpPklDump}dfRawS_{from_date}_{to_date}.pklz', True)
        logger.info('Start load dfStatus pickles')
        dfStatus1s = dfRawS[(dfRawS['Type']=='STS') & (dfRawS['EventID']=='PERIODIC 1s')]
        dfStatus5s = dfRawS[(dfRawS['Type']=='STS') & (dfRawS['EventID']=='PERIODIC 5s')]
        dfStatus5m = dfRawS[(dfRawS['Type']=='STS') & (dfRawS['EventID']=='PERIODIC 5M')]
    else:
        # check pickle exist -> 1) log read -> pickle dump -> log compress
        #                    -> 2) zip read -> pickle dump
        exec_log2pickle()
        [dfRawD, dfRawT, dfRawS] = exec_load_pickles()
        # pre-processing
        [dfRawD, dfRawT, dfStatus1s, dfStatus5s, dfStatus5m]=exec_preprocess(dfRawD, dfRawT, dfRawS)    

        # check dump option
        ref=check_dump_pickles()

    # summary info.
    log_summary()

    # ready to reference
    ref=ddb.get_cpk_ref_data(ddb.db_name)
    ref=check_ref_refresh([dfRawD, dfRawT, dfRawS], ref)

    # ready to indexing
    logger.info('(Step) Health indexing start..!!')
    lst_machine=dfStatus5m['MachineID'].unique().tolist()
    lst_node=dfStatus1s['NodeID'].unique().tolist()
    lst_station=dfRawT['StationID'].unique().tolist()    
    dfHiOht=pd.DataFrame(index=lst_machine)
    dfHiNode=pd.DataFrame(index=lst_node)
    dfHiStation=pd.DataFrame(index=lst_station)
    
    # excute health indexing    
    [dfHiOht, dfHiNode, dfHiStation] = exec_indexing(dfRawD, dfRawT, dfStatus1s, dfStatus5s, dfStatus5m,\
                                                     ref, fpFigureCpk)
    [dfHiOht, dfHiNode, dfHiStation] = calc_total_indexing(dfHiOht, dfHiNode, dfHiStation)
    
    logger.info('(Step) Excute reporting start..!!')
    exec_db_update(dfHiOht, dfHiNode, dfHiStation, dfRawT)    
    logger.info('----- Excute reporting complete -----')
    
    if(dic_opt['plot use']==True):
        logger.info('----- Excute plot start -----')
        df_frame=[dfRawD, dfRawT, dfStatus1s, dfStatus5s, dfStatus5m]
        dplot.diag_plot(analysis_time_file, ArgSite, ArgGroup, from_date, to_date, lst_vhl, df_frame, dic_opt)
        logger.info('----- Excute plot complete -----')


