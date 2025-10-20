# -*- coding: utf-8 -*-
"""
Created on Mon Mar  2 19:28:07 2020

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
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.font_manager as font_manager
import matplotlib.dates
from matplotlib.dates import WEEKLY,MONTHLY, DateFormatter, rrulewrapper, RRuleLocator 
import numpy as np
from numpy import nan as NaN
import seaborn as sns
from scipy.stats import norm
from xlsxwriter import Workbook
import logging

sys.path.append('C:/MCCDiag/python_package')
import diag_functions as dfn
import MyGantt as myg
import diag_dbms as ddb

# logger ####################################################################################################
# create logger with module name
logger_plot = logging.getLogger('main.plot')
    

# 인자 개수 체크
def diag_plot(analysis_time, site, site_group, from_date, to_date, lst_vhl, df_frame, dic_opt):
    dfRawD=df_frame[0]
    dfRawT=df_frame[1]
    dfStatus1s=df_frame[2]
    dfStatus5s=df_frame[3]
    dfStatus5m=df_frame[4]
    
    opt_rawData_save=dic_opt['rawData save']
    opt_myplot_filter=dic_opt['plot filter']
    
    ## "Drive" Type ########################################################################################
    dfDrv = dfRawD
    
    ## "Trans" Type ########################################################################################
    dfTransEQ=dfRawT[(dfRawT['PortType']==1)]
    dfTransSTK=dfRawT[(dfRawT['PortType']==3)]
    dfTransLSTB=dfRawT[(dfRawT['PortType']==5)]
    dfTransRSTB=dfRawT[(dfRawT['PortType']==6)]

    ## log summary - only local excute..
    if __name__ == '__main__':
        logger_plot.info('dfRawD events: %d', len(dfRawD))
        logger_plot.info('dfRawT events: %d', len(dfRawT))
        logger_plot.info('dfTransEQ events: %d , trans cnt: %d', len(dfTransEQ),\
                         len(dfTransEQ[(dfTransEQ['EventID']=='LD DONE') & (dfTransEQ['StartEnd']=='End')]))
        logger_plot.info('dfTransSTK events: %d , trans cnt: %d', len(dfTransSTK),\
                         len(dfTransSTK[(dfTransSTK['EventID']=='LD DONE') & (dfTransSTK['StartEnd']=='End')]))
        logger_plot.info('dfTransLSTB events: %d , trans cnt: %d', len(dfTransLSTB),\
                         len(dfTransLSTB[(dfTransLSTB['EventID']=='LD DONE') & (dfTransLSTB['StartEnd']=='End')]))
        logger_plot.info('dfTransRSTB events: %d , trans cnt: %d', len(dfTransRSTB),\
                         len(dfTransRSTB[(dfTransRSTB['EventID']=='LD DONE') & (dfTransRSTB['StartEnd']=='End')]))
        logger_plot.info('dfStatus1s events: %d', len(dfStatus1s))
        logger_plot.info('dfStatus5s events: %d', len(dfStatus5m))
        logger_plot.info('dfStatus5m events: %d', len(dfRawD))
        logger_plot.info('Machine amount: %d', len(dfStatus5m['MachineID'].unique()))
        logger_plot.info('Node amount: %d', len(dfRawD['NodeID'].unique()))
        logger_plot.info('Station amount: %d', len(dfRawT['StationID'].unique()))
  
    # 4. file directory
    fdir=r'D:/grafana/grafana/public/DiagMCCData/'+site+'/Figure/'+\
        analysis_time+'_'+site_group+'_'+from_date+'_'+to_date+'/'
    if not os.path.isdir(fdir):
        os.mkdir(fdir)
        logger_plot.info('Create Directory: %s', fdir)
        
    ###############################################################################
    ## AMC Voltage
    
    title_file='amc_voltage'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfStatus1s
    df=df[(df['EventID']=='PERIODIC 1s')]
    df=df[['Time','MachineID','NodeID','Value3','Value4','Value5','Value6']]
    df[['Value3','Value4','Value5','Value6']]=df[['Value3','Value4','Value5','Value6']].apply(pd.to_numeric,errors = 'coerce')
    df_calc=df.loc[:,['Value3','Value4','Value5','Value6']]/100.0
    df.update(df_calc)
    df=df.rename(columns={'Value3':'amc_3V','Value4':'amc_5V','Value5':'amc_12Vp','Value6':'amc_12Vm'})
    
    title='amc_3V'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':3}
    ref_y={'min':3.0, 'max':3.5, 'ref':3.3}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='amc_5V'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':4, 'max':6}
    ref_y={'min':5.0, 'max':5.25, 'ref':5.15}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='amc_12Vp'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':10, 'max':13}
    ref_y={'min':11.8, 'max':12.0, 'ref':11.9}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='amc_12Vm'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':10, 'max':13}
    ref_y={'min':11.8, 'max':12.0, 'ref':3.3}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')    
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## Regulator Voltage
    title_file='regulator_voltage'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfStatus1s
    df=df[(df['EventID']=='PERIODIC 1s')]
    df=df[['Time','MachineID','NodeID','Value7','Value8','Value9']]
    df[['Value7','Value8','Value9']]=df[['Value7','Value8','Value9']].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value7':'regulator_voltage_Edc',
                          'Value8':'regulator_voltage_Ecap',
                          'Value9':'regulator_voltage_Eout'})
    
    title='regulator_voltage_Edc'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':200, 'max':400}
    ref_y={'min':230, 'max':355, 'ref':300}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='regulator_voltage_Ecap'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':200, 'max':400}
    ref_y={'min':230, 'max':355, 'ref':300}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='regulator_voltage_Eout'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':200, 'max':400}
    ref_y={'min':230, 'max':355, 'ref':300}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='regulator_voltage_diff_EdcEout'
    df[title]=df['regulator_voltage_Edc']-df['regulator_voltage_Eout']
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':-30, 'max':30}
    ref_y={'min':-15, 'max':15, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='regulator_voltage_diff_EdcEcap'
    df[title]=df['regulator_voltage_Edc']-df['regulator_voltage_Ecap']
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':-30, 'max':30}
    ref_y={'min':-15, 'max':15, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## Regulator Ampere
    title_file='regulator_current'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfStatus1s
    df=df[(df['EventID']=='PERIODIC 1s')]
    df=df[['Time','MachineID','NodeID','Value10','Value11']]
    df[['Value10','Value11']]=df[['Value10','Value11']].apply(pd.to_numeric,errors = 'coerce')
    df_calc=df.loc[:,['Value10','Value11']]/10.0
    df.update(df_calc)
    df=df.rename(columns={'Value10':'regulator_current_Iout',
                          'Value11':'regulator_current_Iac'})
    
    title='regulator_current_Iout'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':20.0}
    ref_y={'min':0, 'max':15.0, 'ref':9.0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='regulator_current_Iac'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':20.0}
    ref_y={'min':0, 'max':15.0, 'ref':6.5}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## Regulator Temperature
    title_file='regulator_temperature'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfStatus1s
    df=df[(df['EventID']=='PERIODIC 1s')]
    df=df[['Time','MachineID','NodeID','Value12','Value13','Value14','Value15','Value16']]
    df[['Value12','Value13','Value14','Value15','Value16']]=df[['Value12','Value13','Value14','Value15','Value16']].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value12':'regulator_temperature_pickup',
                          'Value13':'regulator_temperature_IGB',
                          'Value14':'regulator_temperature_Ecap',
                          'Value15':'regulator_temperature_Ecap2',
                          'Value16':'regulator_temperature_EcapRound'})
    
    title='regulator_temperature_pickup'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':80.0}
    ref_y={'min':0, 'max':60.0, 'ref':25}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
        
    title='regulator_temperature_IGB'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':80.0}
    ref_y={'min':0, 'max':70.0, 'ref':25}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='regulator_temperature_Ecap'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':80.0}
    ref_y={'min':0, 'max':60.0, 'ref':25}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='regulator_temperature_Ecap2'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':80.0}
    ref_y={'min':0, 'max':60.0, 'ref':25}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='regulator_temperature_EcapRound'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':80.0}
    ref_y={'min':0, 'max':60.0, 'ref':25}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='regulator_temperature_diff_Ecap'
    df[title]=df['regulator_temperature_Ecap']-df['regulator_temperature_EcapRound']
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':-20, 'max':20}
    ref_y={'min':-8, 'max':8, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='regulator_temperature_diff_Ecap2'
    df[title]=df['regulator_temperature_Ecap2']-df['regulator_temperature_EcapRound']
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':-20, 'max':20}
    ref_y={'min':-8, 'max':8, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## IPC Monitor
    title_file='ipc_monitor'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfStatus1s
    df=df[(df['EventID']=='PERIODIC 1s')]
    df=df[['Time','MachineID','NodeID','Value17','Value18']]
    df[['Value17','Value18']]=df[['Value17','Value18']].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value17':'ipc_temperature','Value18':'ipc_voltage'})
    
    title='ipc_temperature'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':80.0}
    ref_y={'min':0, 'max':60.0, 'ref':25}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='ipc_voltage'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':3, 'max':8.0}
    ref_y={'min':5.0, 'max':5.5, 'ref':5.2}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
        
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## IPC Perfomance
    title_file='ipc_performance'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfStatus5s
    df=df[(df['EventID']=='PERIODIC 5s')]
    df=df[['Time','MachineID','NodeID','Value1','Value2']]
    df[['Value1','Value2']]=df[['Value1','Value2']].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value1':'ipc_cpu_usage','Value2':'ipc_memory_usage'})
    
    title='ipc_memory_usage'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':100}
    ref_y={'min':0, 'max':50, 'ref':30}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='ipc_cpu_usage'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':100}
    ref_y={'min':0, 'max':99, 'ref':25}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
        
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## Process Cycle
    title_file='thread_cycle'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfStatus5s
    df=df[(df['EventID']=='PERIODIC 5s')]    
    df=df[['Time','MachineID','NodeID','Value4','Value5','Value6','Value7','Value8','Value9']]
    lst_key=['Value4','Value5','Value6','Value7','Value8','Value9']
    df[lst_key]=df[lst_key].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value4':'thread_cycle_log',
                          'Value5':'thread_cycle_task',
                          'Value6':'thread_cycle_status',
                          'Value7':'thread_cycle_excute',
                          'Value8':'thread_cycle_extrajob',
                          'Value9':'thread_cycle_amc'})
    
    title='thread_cycle_status'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':200}
    ref_y={'min':0, 'max':100, 'ref':50}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)

    title='thread_cycle_amc'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':600}
    ref_y={'min':0, 'max':100, 'ref':16}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)

    title='thread_cycle_task'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':200}
    ref_y={'min':0, 'max':100, 'ref':16}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='thread_cycle_excute'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':200}
    ref_y={'min':0, 'max':100, 'ref':16}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='thread_cycle_log'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':10000}
    ref_y={'min':0, 'max':3000, 'ref':1000}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='thread_cycle_extrajob'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':200}
    ref_y={'min':0, 'max':100, 'ref':50}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
            
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## Hoist Teaching Position Diff
    title_file='hoist_tc_position'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfTransEQ
    df=df[(df['EventID']=='LD MOVE UP HOIST SLOW') & (df['StartEnd']=='Start')]
    df=df[['Time','MachineID','StationID','EventID','StartEnd','Value4','Value5']]
    lst_key=['Value4','Value5']
    df[lst_key]=df[lst_key].apply(pd.to_numeric,errors = 'coerce')    
    df=df.rename(columns={'Value4':'Hoist Pos','Value5':'Teach Value'})

    df['hoist_tc_position_diff']=df['Teach Value']-df['Hoist Pos']
    
    title='hoist_tc_position_diff'    
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':-30, 'max':30}
    ref_y={'min':-5, 'max':5, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)

    plot_xy={'x':'StationID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_station.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
        
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## Hoist Home Position
    title_file='hoist_home'
    logger_plot.info('Plot - %s', title_file) 
    start = time.perf_counter()
    frames_raw=[]
    lst_raw_keys=[]
    
    df=dfTransEQ
    df=df[(df['EventID']=='LD DONE') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','StationID','EventID','StartEnd','Value3']]
    df['Value3']=df['Value3'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value3':'hoist_home_on_position_ld'})
   
    title='hoist_home_on_position_ld'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':10.0}
    ref_y={'min':3, 'max':9, 'ref':6}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)
    
    df=dfTransEQ
    df=df[(df['EventID']=='ULD DONE') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','StationID','EventID','StartEnd','Value3']]
    df['Value3']=df['Value3'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value3':'hoist_home_on_position_uld'})    
    
    title='hoist_home_on_position_uld'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':10.0}
    ref_y={'min':2, 'max':8, 'ref':5}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)    
      
    # Raw Data Save
    if(opt_rawData_save==True):
        df_raw=pd.concat(frames_raw, keys=lst_raw_keys, sort=False)
        lst_col=df_raw.columns.to_list()
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df_raw, lst_col, 'RawData')  
      
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)        
    
    
    ###############################################################################
    ## QR Mark detect - sensor distance, enter velocity
    title_file='mark_qr'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfDrv
    df=df[(df['EventID']=='QR MARK') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','NodeID','EventID','StartEnd','Target',\
           'Value2','Value3','Value4','Value5','Value6','Value7']]
    lst=['Value2','Value3','Value4','Value5','Value6','Value7']
    df[lst]=df[lst].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'qr_mark_start_distance',
                          'Value3':'qr_offset',
                          'Value4':'qr_mark_start_velocity',
                          'Value5':'qr_mark_enter_velocity2',
                          'Value6':'qr_mark_enter_velocity',
                          'Value7':'qr_mark_offset_diff',
                          'Target':'StationID'})
    
    #df=df[df['node_mark_start_distance']>0] # pre sensor detect -> mark start
    #df=df[df['node_mark_enter_velocity'] != 0] # 분석 중..
    
    title='qr_offset'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':-10, 'max':100}
    ref_y={'min':5, 'max':40, 'ref':25}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    

    title='qr_mark_enter_velocity'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':-0.5, 'max':1.0}
    ref_y={'min':0, 'max':0.25, 'ref':0.1}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'StationID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_station.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    

    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)    
    
    
    ###############################################################################
    ## Station Front/Rear Sensor Distance
    title_file='mark_station'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfDrv
    df=df[(df['EventID']=='STATION MARK') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','Target','EventID','StartEnd',\
           'Value2','Value3','Value4','Value5','Value6','Value7','Value8']]
    lst_key=['Value2','Value3','Value4','Value5','Value6','Value7','Value8']
    df[lst_key]=df[lst_key].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'station_mark_start_distance',
                          'Value3':'station_sensor_distance',
                          'Value4':'station_mark_start_velocity',
                          'Value5':'station_mark_enter_velocity',
                          'Value6':'station_mark_enter_velocity2',
                          'Value7':'station_sensor_haunt_count',
                          'Value8':'station_mark_offet_diff',
                          'Target':'StationID'})

    #filter
    df=df[df['station_mark_start_distance']>0]

    title='station_sensor_distance'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':180}
    ref_y={'min':145, 'max':160, 'ref':152}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'StationID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_station.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='station_mark_enter_velocity2'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':2.0}
    ref_y={'min':0, 'max':0.1, 'ref':0.05}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'StationID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_station.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)        
        
    title='station_sensor_haunt_count'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':10}
    ref_y={'min':0, 'max':2, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'StationID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_station.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)

    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
        
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    
    ###############################################################################
    ## Trans BCR Read Fail Count
    title_file='bcr_trans'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfDrv
    df=df[(df['EventID']=='STATION MARK') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','Target','EventID','StartEnd','Value9','Value10']]
    lst_key=['Value9','Value10']
    df[lst_key]=df[lst_key].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value9':'BCR_trans_readfail_count',
                          'Value10':'BCR_trans_trigger_count',
                          'Target':'StationID'})
    
    title='BCR_trans_readfail_count'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':10}
    ref_y={'min':0, 'max':1, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)    
    
    plot_xy={'x':'StationID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_station.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='BCR_trans_trigger_count'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':10}
    ref_y={'min':0, 'max':2, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)    
    
    plot_xy={'x':'StationID', 'y':title}    
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_station.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
        
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    

    ###############################################################################
    ## Node Mark detect - sensor distance, enter velocity
    title_file='mark_node'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfDrv
    df=df[(df['EventID']=='NODE MARK') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','NodeID','EventID','StartEnd','Value2','Value3','Value4','Value5','Value6']]
    lst=['Value2','Value3','Value4','Value5','Value6']
    df[lst]=df[lst].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'node_mark_start_distance',
                          'Value3':'node_sensor_distance',
                          'Value4':'node_mark_start_velocity',
                          'Value5':'node_mark_enter_velocity2',
                          'Value6':'node_mark_enter_velocity'})
    
    df=df[df['node_mark_start_distance']>0] # pre sensor detect -> mark start
    df=df[df['node_mark_enter_velocity'] != 0] # 분석 중..
    
    title='node_sensor_distance'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':400}
    ref_y={'min':265, 'max':295, 'ref':285}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    title='node_mark_enter_velocity'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':-0.5, 'max':1.0}
    ref_y={'min':0, 'max':0.25, 'ref':0.1}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    
    
    ###############################################################################
    ## Drive BCR Read Fail Count
    title_file='bcr_drive'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfDrv
    df=df[(df['EventID']=='NODE MARK') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','NodeID','EventID','StartEnd','Value7']]
    df['Value7']=df['Value7'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value7':'BCR_drive_readfail_count'})
    
    title='BCR_drive_readfail_count'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':10}
    ref_y={'min':0, 'max':1, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
        
    plt.close()
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## Slide Home Sensor Off Position
    title_file='slide_home'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    frames_raw=[]
    lst_raw_keys=[]

    df=dfTransLSTB
    if(len(df)>0):
        df=df[(df['EventID']=='LD MOVE SHIFTROTATE') & (df['StartEnd']=='End')]
        df=df[['Time','MachineID','EventID','StartEnd','Value3']]
        df['Value3']=df['Value3'].apply(pd.to_numeric,errors = 'coerce')
        df=df.rename(columns={'Value3':'slide_home_off_position_lstb'})
        
        #filter
        df_abnormal = df[df['slide_home_off_position_lstb']==0]
        df = df.drop(df_abnormal.index.to_list())
        
        title='slide_home_off_position_lstb'
        plot_xy={'x':'MachineID', 'y':title}
        limit_y={'min':-10, 'max':0}
        ref_y={'min':-6.5, 'max':-4.5, 'ref':-5.5}
        filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
        dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
        frames_raw.append(df)
        lst_raw_keys.append(title)
    else:
        logger_plot.info('Data is empty..!!')
    
    
    df=dfTransRSTB
    df=df[(df['EventID']=='LD MOVE SHIFTROTATE') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','EventID','StartEnd','Value3']]
    df['Value3']=df['Value3'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value3':'slide_home_off_position_rstb'})
    
    #filter
    df_abnormal = df[df['slide_home_off_position_rstb']==0]
    df = df.drop(df_abnormal.index.to_list())    
    
    title='slide_home_off_position_rstb'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':10}
    ref_y={'min':4.5, 'max':6.5, 'ref':5.5}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'    
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        df_raw=pd.concat(frames_raw, keys=lst_raw_keys, sort=False)
        lst_col=df_raw.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df_raw, lst_col, 'RawData')
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## Slide Home Sensor Off Position
    title_file='slide_home_off'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()    
    df=dfDrv
    df=df[(df['EventID']=='DRIVING') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','NodeID','EventID','StartEnd','Value5']]
    df['Value5']=df['Value5'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value5':'slide_home_sensor_off'})
    
    title='slide_home_sensor_off'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':10}
    ref_y={'min':0, 'max':1, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plt.figure(figsize=(24,12))
    plot_xy={'x':'NodeID', 'y':title}
    limit_y={'min':0, 'max':10}
    ref_y={'min':0, 'max':1, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## Hand Open/Close Time
    title_file='hand_movetime'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    frames_raw=[]
    lst_raw_keys=[]
    
    df=dfRawT
    df=df[(df['EventID']=='LD RELEASE') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','EventID','StartEnd','Value2']]
    df['Value2']=df['Value2'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'hand_release_time'})
   
    title='hand_release_time'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':1200}
    ref_y={'min':800, 'max':1000, 'ref':900}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)
    
    df=dfRawT
    df=df[(df['EventID']=='ULD GRIP') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','EventID','StartEnd','Value2']]
    df['Value2']=df['Value2'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'hand_grip_time'})
    
    title='hand_grip_time'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':1200}
    ref_y={'min':800, 'max':1000, 'ref':900}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        df_raw=pd.concat(frames_raw, keys=lst_raw_keys, sort=False)
        lst_col=df_raw.columns.to_list()
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df_raw, lst_col, 'RawData')
    
    plt.close()
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
 
    
    ###############################################################################
    ## Shutter Front Open/Close Time
    title_file='shutter_time'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    frames_raw=[]
    lst_raw_keys=[]
    
    df=dfRawD
    df=df[(df['EventID']=='SHUTTER FRONT OPEN') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','NodeID','Offset','EventID','StartEnd','Value2']]
    df['Value2']=df['Value2'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'shutter_front_open_movetime'})  
    
    title='shutter_front_open_movetime'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':600, 'max':1200}
    ref_y={'min':800, 'max':1000, 'ref':880}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)
    
    df=dfRawD
    df=df[(df['EventID']=='SHUTTER FRONT CLOSE') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','NodeID','Offset','EventID','StartEnd','Value2']]
    df['Value2']=df['Value2'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'shutter_front_close_movetime'})
    
    title='shutter_front_close_movetime'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':600, 'max':1200}
    ref_y={'min':800, 'max':1000, 'ref':880}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)
    
    df=dfRawD
    df=df[(df['EventID']=='SHUTTER REAR OPEN') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','NodeID','Offset','EventID','StartEnd','Value2']]
    df['Value2']=df['Value2'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'shutter_rear_open_movetime'})
    
    title='shutter_rear_open_movetime'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':600, 'max':1200}
    ref_y={'min':800, 'max':1000, 'ref':880}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)
            
        
    df=dfRawD
    df=df[(df['EventID']=='SHUTTER REAR CLOSE') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','NodeID','Offset','EventID','StartEnd','Value2']]
    df['Value2']=df['Value2'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'shutter_rear_close_movetime'})
    
    title='shutter_rear_close_movetime'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':600, 'max':1200}
    ref_y={'min':800, 'max':1000, 'ref':880}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)
        
    # Raw Data Save
    if(opt_rawData_save==True):
        df_raw=pd.concat(frames_raw, keys=lst_raw_keys, sort=False)
        lst_col=df_raw.columns.to_list()
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df_raw, lst_col, 'RawData')
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
        
    ###############################################################################
    ## Hoist Home Sensor Off Count
    title_file='hoist_home_sensor'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfDrv
    df=df[(df['EventID']=='DRIVING') & (df['StartEnd']=='End')]
    df=df[['Time','MachineID','NodeID','EventID','StartEnd','Value6']]
    df['Value6']=df['Value6'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value6':'hoist_home_sensor_off_count'})
    
    
    title='hoist_home_sensor_off_count'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':10}
    ref_y={'min':0, 'max':1, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
        
    ###############################################################################
    ## Inner FOUP Abnormal Detect/Not Detect Count
    title_file='inner_foup_detect'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfRawD
    df=df[(df['EventID']=='DRIVING') & (df['StartEnd']=='End')]
    df=df[['Time','EventID','MachineID','NodeID','Velocity','StartEnd','Value3','Value4']]
    df[['Value3','Value4']]=df[['Value3','Value4']].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value3':'foup_abnormal_detect','Value4':'foup_abnormal_undetected'})
    
    title='foup_abnormal_detect'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':10}
    ref_y={'min':0, 'max':1, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='foup_abnormal_undetected'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':10}
    ref_y={'min':0, 'max':1, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
        
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## Node Compensation
    title_file='tag_node'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfDrv
    df=df[(df['EventID']=='NODE DETECT')]
    df=df[['Time','MachineID','NodeID','EventID','StartEnd','Value4']]
    df['Value4']=df['Value4'].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value4':'node_compensation'})
    
    title='node_compensation'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':-400, 'max':400}
    ref_y={'min':-100, 'max':100, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)    
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)

    ###############################################################################
    ## CID Info - CID R First Link Level
    title_file='cidr_monitor'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfDrv
    df=df[(df['EventID']=='CID LINK') & (df['StartEnd']=='End') ]
    df=df[['Time','MachineID','NodeID','EventID','Velocity','Value1','Value2','Value3',\
           'Value4','Value5','Value6','Value7']]
    lst=['Value1','Value2','Value3','Value4','Value5','Value6','Value7']
    df[lst]=df[lst].apply(pd.to_numeric,errors = 'coerce')
    df_calc=df.loc[:,['Value4']]/100.0
    df.update(df_calc)
    df_calc=df.loc[:,['Value5']]/10.0
    df.update(df_calc)
    df=df.rename(columns={'Value1':'CIDR_first_linklevel','Value2':'CIDR_fiber_error',\
                          'Value3':'CIDR_RF_error','Value4':'CIDR_max_current',\
                          'Value5':'CIDR_max_voltage','Value6':'CIDR_max_temperature',\
                          'Value7':'CIDR_noise'})
    
    title='CIDR_first_linklevel'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':150, 'max':350}
    ref_y={'min':170, 'max':280, 'ref':250}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='CIDR_fiber_error'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':100}
    ref_y={'min':0, 'max':1, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='CIDR_RF_error'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':100}
    ref_y={'min':0, 'max':1, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='CIDR_max_current'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':1.0}
    ref_y={'min':0, 'max':0.5, 'ref':0.1}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)    
    
    
    title='CIDR_max_voltage'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':20, 'max':30}
    ref_y={'min':23, 'max':25, 'ref':24}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='CIDR_max_temperature'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':80}
    ref_y={'min':0, 'max':55, 'ref':25}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='CIDR_noise'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':50}
    ref_y={'min':0, 'max':30, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## CID O Monitor Infomation
    title_file='cido_monitor'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfDrv
    df=df[(df['EventID']=='CID LINK') & (df['StartEnd']=='End') ]
    df=df[['Time','MachineID','NodeID','EventID','Velocity','Value8','Value9','Value10']]
    lst=['Value8','Value9','Value10']
    df[lst]=df[lst].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value8':'CIDO_first_linklevel',
                          'Value9':'CIDO_fiber_error',
                          'Value10':'CIDO_RF_error'})
    
    title='CIDO_first_linklevel'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':150, 'max':350}
    ref_y={'min':170, 'max':280, 'ref':250}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='CIDO_fiber_error'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':100}
    ref_y={'min':0, 'max':1, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
        
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    
    title='CIDO_RF_error'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':100}
    ref_y={'min':0, 'max':1, 'ref':0}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
    
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## CID Info - CID Link Info
    title_file='cid_link'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfDrv
    df=df[(df['EventID']=='CID LINK') & (df['StartEnd']=='Start') ]
    df=df[['Time','MachineID','NodeID','EventID','Velocity','Value1','Value2','Value3']]
    lst=['Value1','Value2','Value3']
    df[lst]=df[lst].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value1':'CIDLinkTime','Value2':'CIDMapLinkOffset','Value3':'CIDLinkDistance'})
    
    if(len(df)>0):
        df['CID_link_distance_diff']=df['CIDLinkDistance']-df['CIDMapLinkOffset']
        title='CID_link_distance_diff'
        plot_xy={'x':'MachineID', 'y':title}
        limit_y={'min':-5000, 'max':5000}
        ref_y={'min':-100, 'max':100, 'ref':0}
        filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
        dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
        
        plot_xy={'x':'NodeID', 'y':title}
        filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
        dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    else:
        logger_plot.info('Data is empty..!!')
    
    # Raw Data Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    ###############################################################################
    ## Steer 구동 시간
    title_file='steer_time'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    frames_raw=[]
    lst_raw_keys=[]

    df=dfDrv
    df=df[(df['EventID']=='STEER FRONT LEFT') & (df['StartEnd']=='End') ]
    df=df[['Time','MachineID','NodeID','Offset','EventID','Velocity','Value2','Value3','Value4','Value6','Value7','Value10']]
    lst=['Value2','Value4','Value6','Value7','Value10']
    df[lst]=df[lst].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'steer_front_left_movetime',
                          'Value3':'steer_front_left_sensor_count',
                          'Value4':'OHTDetectOutput',
                          'Value6':'SteerRightOffTick',
                          'Value7':'SteerLeftOnTick',
                          'Value10':'LeftCommandTick'})
    
    #method#1 --------------------------------------------------------------
    #filter
    df=df[df['steer_front_left_movetime']<3000]
    df=df[df['Velocity']>0]
    df=df[df['OHTDetectOutput']!=11] #N-Branch
    if(site == 'SEMES'):
        df=df[(df['NodeID']==909) | (df['NodeID']==912) | (df['NodeID']==913) | (df['NodeID']==915) |\
              (df['NodeID']==906) | (df['NodeID']==924) | (df['NodeID']==925) | (df['NodeID']==928) |\
              (df['NodeID']==932) | (df['NodeID']==942) | (df['NodeID']==945) | (df['NodeID']==947) |\
              (df['NodeID']==65009) | (df['NodeID']==65013) | (df['NodeID']==65024) |\
              (df['NodeID']==66001) | (df['NodeID']==66002) | (df['NodeID']==66004) |\
              (df['NodeID']==66005) | (df['NodeID']==66006) | (df['NodeID']==66007) |\
              (df['NodeID']==66008) | (df['NodeID']==66011) | (df['NodeID']==67008) |\
              (df['NodeID']==67009) | (df['NodeID']>68001)] #2F    
       
    title='steer_front_left_movetime'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':1000}
    ref_y={'min':250, 'max':750, 'ref':500}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)

    
    df=dfDrv
    df=df[(df['EventID']=='STEER FRONT RIGHT') & (df['StartEnd']=='End') ]
    df=df[['Time','MachineID','NodeID','Offset','EventID','Velocity','Value2','Value3','Value4']]
    lst=['Value2','Value3','Value4']
    df[lst]=df[lst].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'steer_front_right_movetime',
                            'Value3':'steer_front_right_sensor_count',
                            'Value4':'OHTDetectOutput'})
    
    #filter
    df=df[df['steer_front_right_movetime']<3000]
    df=df[df['Velocity']>0]
    df=df[df['OHTDetectOutput']!=11] #N-Branch
    if(site == 'SEMES'):    
        df=df[(df['NodeID']==912) | (df['NodeID']==915) | (df['NodeID']==916) | (df['NodeID']==921) |\
                (df['NodeID']==925) | (df['NodeID']==934) | (df['NodeID']==935) | (df['NodeID']==936) |\
                (df['NodeID']==937) | (df['NodeID']==941) | (df['NodeID']==943) | (df['NodeID']==944) |\
                (df['NodeID']==65009) | (df['NodeID']==65013) | (df['NodeID']==65024) |
                (df['NodeID']==66001) | (df['NodeID']==66002) | (df['NodeID']==66004) |
                (df['NodeID']==66005) | (df['NodeID']==66006) | (df['NodeID']==66007) |
                (df['NodeID']==66008) | (df['NodeID']==66011) | (df['NodeID']==67008) |
                (df['NodeID']==67009) | (df['NodeID']>68001)] #2F
            
    title='steer_front_right_movetime'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':1000}
    ref_y={'min':250, 'max':750, 'ref':500}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)
    
    
    df=dfDrv
    df=df[(df['EventID']=='STEER REAR LEFT') & (df['StartEnd']=='End') ]
    df=df[['Time','MachineID','NodeID','Offset','EventID','Velocity','Value2','Value3','Value4']]
    lst=['Value2','Value3','Value4']
    df[lst]=df[lst].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'steer_rear_left_movetime',
                            'Value3':'steer_rear_left_sensor_count',
                            'Value4':'OHTDetectOutput'})
    
    #filter
    df=df[df['steer_rear_left_movetime']<3000]
    df=df[df['Velocity']>0]
    df=df[df['OHTDetectOutput']!=11] #N-Branch
    if(site == 'SEMES'):    
        df=df[(df['NodeID']==912) | (df['NodeID']==913) | (df['NodeID']==915) |\
                (df['NodeID']==906) | (df['NodeID']==924) | (df['NodeID']==925) | (df['NodeID']==928) |\
                (df['NodeID']==932) | (df['NodeID']==942) | (df['NodeID']==945) | (df['NodeID']==947) |\
                (df['NodeID']==65024) |
                (df['NodeID']==66001) | (df['NodeID']==66002) | (df['NodeID']==66004) |
                (df['NodeID']==66005) | (df['NodeID']==66006) | (df['NodeID']==66007) |
                (df['NodeID']==66008) | (df['NodeID']==66011) |        
                (df['NodeID']==67009) | ((df['NodeID']>68006) & (df['NodeID']<68016))] #2F
    
    title='steer_rear_left_movetime'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':1000}
    ref_y={'min':250, 'max':750, 'ref':500}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)
    
    
    df=dfDrv
    df=df[(df['EventID']=='STEER REAR RIGHT') & (df['StartEnd']=='End') ]
    df=df[['Time','MachineID','NodeID','Offset','EventID','Velocity','Value2','Value3','Value4']]
    lst=['Value2','Value3','Value4']
    df[lst]=df[lst].apply(pd.to_numeric,errors = 'coerce')
    df=df.rename(columns={'Value2':'steer_rear_right_movetime',
                            'Value3':'steer_rear_right_sensor_count',
                            'Value4':'OHTDetectOutput'})
    
    #filter
    df=df[df['steer_rear_right_movetime']<3000]
    df=df[df['Velocity']>0]
    df=df[df['OHTDetectOutput']!=11] #N-Branch
    if(site == 'SEMES'):    
        df=df[(df['NodeID']==915) | (df['NodeID']==916) | (df['NodeID']==923) |
                (df['NodeID']==925) | (df['NodeID']==931) | (df['NodeID']==934) | (df['NodeID']==935) |
                (df['NodeID']==937) | (df['NodeID']==941) | (df['NodeID']==951) | (df['NodeID']==952) |\
                (df['NodeID']==65024) |
                (df['NodeID']==66001) | (df['NodeID']==66002) | (df['NodeID']==66004) |
                (df['NodeID']==66005) | (df['NodeID']==66006) | (df['NodeID']==66007) |
                (df['NodeID']==66008) | (df['NodeID']==66011) |        
                (df['NodeID']==67009) | ((df['NodeID']>68006) & (df['NodeID']<68016))] #2F
    
    title='steer_rear_right_movetime'
    plot_xy={'x':'MachineID', 'y':title}
    limit_y={'min':0, 'max':1000}
    ref_y={'min':250, 'max':750, 'ref':500}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    
    plot_xy={'x':'NodeID', 'y':title}
    filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
    dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    frames_raw.append(df)
    lst_raw_keys.append(title)
   
    # RawData Save
    if(opt_rawData_save==True):
        df_raw=pd.concat(frames_raw, keys=lst_raw_keys, sort=False)
        lst_col=df_raw.columns.to_list()
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df_raw, lst_col, 'RawData')

    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
    
    ###############################################################################
    ## Foup Cover
    title_file='foup_cover_detect_position'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfTransLSTB
    if(len(df)>0):    
        df=df[(df['EventID']=='LD MOVE SHIFTROTATE') & (df['StartEnd']=='End')]
        df=df[['Time','MachineID','StationID','EventID','StartEnd','Value4']]
        df['Value4']=df['Value4'].apply(pd.to_numeric,errors = 'coerce')
        df=df.rename(columns={'Value4':'foup_cover_detect_on_position'})
        
        title='foup_cover_detect_on_position'
        plot_xy={'x':'MachineID', 'y':title}
        limit_y={'min':-20, 'max':0.0}
        ref_y={'min':-13, 'max':-5, 'ref':10}
        filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
        dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
    else:
        logger_plot.info('Data is empty..!!')

    # RawData Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')

    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)

    ###############################################################################
    ## Abnormal Stop
    title_file='abnormal_stop'
    logger_plot.info('Plot - %s', title_file)
    start = time.perf_counter()
    df=dfDrv
    if(len(df)>0):    
        df=df[(df['EventID']=='ABNORMAL STOP') & (df['StartEnd']=='End')]
        df=df[['Time','MachineID','NodeID','EventID','StartEnd','Value1']]
        df['Value1']=df['Value1'].apply(pd.to_numeric,errors = 'coerce')
        df=df.rename(columns={'Value1':'abnormal_stop_time'})
        
        if(len(df)>0):
            title='abnormal_stop_time'
            plot_xy={'x':'MachineID', 'y':title}
            limit_y={'min':0, 'max':15000}
            ref_y={'min':0, 'max':5000, 'ref':0}
            filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
            dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
            
            plot_xy={'x':'NodeID', 'y':title}
            filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
            dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
        else:
            logger_plot.info('Data is empty..!!')
    else:
        logger_plot.info('Data is empty..!!')

    # RawData Save
    if(opt_rawData_save==True):
        lst_col=df.columns.to_list() #전체
        dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')

    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)

    ###############################################################################
    ## OBS abnormal detect
    if(site!="SEMES"):
        title_file='obs_abnormal_detect'
        logger_plot.info('Plot - %s', title_file)
        start = time.perf_counter()
        df=dfDrv
        df=df[(df['EventID']=='OHT DETECT AND STOP')]
        if(len(df)>0):
            df=df[['Time','MachineID','EventID','StartEnd','NodeID','EventID','Velocity',
                   'Value1','Value2','Value3','Value4']]
            lst=['Value1','Value2','Value3','Value4']
            df[lst]=df[lst].apply(pd.to_numeric,errors = 'coerce')
            
            dff=pd.DataFrame()
            for i in range(len(df)):
                if((df.iloc[i]['StartEnd']=='End') &\
                   (df.iloc[i]['Value1']==0) &\
                   (i>0)):        
                    vhl=df.iloc[i]['MachineID']
                    node=df.iloc[i]['NodeID']
                    if((df.iloc[i-1]['MachineID']==vhl) &\
                       (df.iloc[i-1]['NodeID']==node) &\
                       (df.iloc[i-1]['StartEnd']=='Start') &\
                       (df.iloc[i-1]['Value3']==8) & #oht detect level
                       (df.iloc[i-1]['Value4']==3)): #obs detect level
                        
                        print(f"{i} : {len(df)}")
                        dff=dff.append(df.iloc[i].to_frame().T)                
            
            df=dff
            if(len(df)>0):
                df=df.rename(columns={'Value1':'oht_abnormal_detect_time',
                                      'Value2':'obs_abnormal_detect_time',
                                      'Value3':'oht_abnormal_detect_lv',
                                      'Value4':'obs_abnormal_detect_lv'})
                
            #    df=df[df['oht_detect_lv']==8]
            #    df=df[df['obs_detect_time']!=0] # obstacle detect
            #    df=df[df['oht_detect_time']==0] # oht undetect
                
                title='obs_abnormal_detect_time'
                plot_xy={'x':'MachineID', 'y':title}
                limit_y={'min':-5000, 'max':5000}
                ref_y={'min':0, 'max':0, 'ref':0}
                filename=fdir+analysis_time+'_'+title_file+'('+title+')_machine.png'
                dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)
                
                plot_xy={'x':'NodeID', 'y':title}
                filename=fdir+analysis_time+'_'+title_file+'('+title+')_node.png'
                dfn.my_plot(title, df, plot_xy,(24,12), 1, limit_y, ref_y, True, filename, opt_myplot_filter)        
            else:
                logger_plot.info('Abnormal data is empty..!!')
        else:
            logger_plot.info('Data is empty..!!')
    
        # RawData Save
        if(opt_rawData_save==True):
            lst_col=df.columns.to_list() #전체
            dfn.save_rawData(fdir, analysis_time+'_'+title_file+'_raw', df, lst_col, 'RawData')    
    
    logger_plot.debug('Elapsed Time: %.2f sec', time.perf_counter()-start)
    
###############################################################################
if __name__ == '__main__': #직접 실행시 name이 '__main__' -> Module Import 시 실행하고 싶지 않을 경우 구문
    print('diag_plot module Testing..!!')
   
    site = 'V1L_10F'
    site_group = 'V1L_10F'
    from_date = '2020090400'
    to_date   = '2020090402'
    lst_vhl=['ALL']
    dic_opt={'rawData save':False,
             'plot filter':True,
             'plot use':True,
             'MCC Ver':2.0,
             'dump pickle':False}       
    
    site = 'SEMES'
    site_group = 'SEMES_2F'
    from_date = '2020081010'
    to_date = '2020081010'
    lst_vhl=['ALL']
    dic_opt={'rawData save':False,
             'plot filter':False,
             'plot use':True,
             'MCC Ver':2.1,
             'Ref Refresh':False,
             'dump pickle':False}   
    
    f=r'D:/DiagMCCData/'+site+'/Temp/'
    print('1. load Drive pickle start')
    dfRawD=dfn.LoadPickleFile(f+f'dfRawD_{from_date}_{to_date}.pklz', True)
    print('2. load Trans pickle start')
    dfRawT=dfn.LoadPickleFile(f+f'dfRawT_{from_date}_{to_date}.pklz', True)
    print('3. load status1s pickle start')
    dfStatus1s=dfn.LoadPickleFile(f+f'dfStatus1s_{from_date}_{to_date}.pklz', True)
    print('4. load status5s pickle start')
    dfStatus5s=dfn.LoadPickleFile(f+f'dfStatus5s_{from_date}_{to_date}.pklz', True)
    print('5. load status5m pickle start')
    dfStatus5m=dfn.LoadPickleFile(f+f'dfStatus5m_{from_date}_{to_date}.pklz', True)
    
    df_frame=[dfRawD, dfRawT, dfStatus1s, dfStatus5s, dfStatus5m]
    
    #get db data
    ref=ddb.get_cpk_ref_data(ddb.db_name)
    
    print('6. Plot start')
    dt_now=dt.datetime.now()
    str_analysis_time=dt_now.strftime('%Y-%m-%d %H%M%S')
    analysis_time=str_analysis_time
    diag_plot(str_analysis_time, site, site_group, from_date, to_date, lst_vhl, df_frame, dic_opt)
    

    