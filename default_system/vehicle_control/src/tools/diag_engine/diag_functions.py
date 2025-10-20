# -*- coding: utf-8 -*-
"""
Created on Fri Jan 10 12:00:40 2020

@author: taein78.kim
"""

import sys
import time
import os
import glob
import numpy as np
import pandas as pd
import pickle
import gzip
import zipfile
import csv
import logging
import matplotlib.pyplot as plt
import seaborn as sns

sys.path.append('C:/MCCDiag/python_package')
import diag_dbms as ddb

# logger ####################################################################################################
# create logger with module name
logger_func = logging.getLogger('main.func')

# check & make directory
def check_directory(file_path, do_make):
    # just think..
    return 0
    

# Pickle Dump
def DumpPickleFile(data, fp, IsCompress):
    if(IsCompress):
        with gzip.open(fp, 'wb') as f:
            pickle.dump(data, f)
    else:
        with open(fp, 'wb') as f:
            pickle.dump(data, f, pickle.HIGHEST_PROTOCOL)
    
# Pickle Load
def LoadPickleFile(fp, IsCompress):
    if(IsCompress):
        with gzip.open(fp, 'rb') as f:
            return pickle.load(f)
    else:
        with open(fp, 'rb') as f:
            return pickle.load(f)


# Log File to loading
def loadFiles(fPath, site, date):
    fp = fPath + '\\v*\\MCC' + date + '.log'
    files = glob.glob(fp)
    fcnt = 0
    start = time.perf_counter()
    logger_func.debug('file count: %d', len(files))
    for f in files:
       df = pd.read_csv (f, names = ['Time','MachineID', 'Type', 'Date', 'Time2', 'Event ID', 'Staus', 'Box type', 'Station ID', 'EQ', 'Node ID', 'Offset', 'Data Key 1','Data Value 1','Data Key 2','Data Value 2','Data Key 3','Data Value 3','Data Key 4','Data Value 4','Data Key 5','Data Value 5','Data Key 6','Data Value 6','Data Key 7','Data Value 7','Data Key 8','Data Value 8','Data Key 9','Data Value 9','Data Key 10','Data Value 10'
, 'Data Key 11','Data Value 11', 'c34', 'c35', 'c36', 'c37', 'c38', 'c39','c40','c41','c42','c43','c44','c45','c46'])       

       if(fcnt == 0):
           df_all = df
       else:
           df_all = df_all.append(df, ignore_index=True)
       fcnt = fcnt + 1
       
    fpPklFile = 'D:/DiagMCCData/SEMES/Data/SEMES_' + date + '.pkl'   
    DumpPickleFile(df_all, fpPklFile, True)    
    logger_func.info('loadFiles time: %.2f sec', time.perf_counter() - start)
           
    if(fcnt == 0):
        return 0
    else:
        return df_all    


# Raw -> Drive Type
def parseDriveType(df):
    # drop Key-Value
    lst = df.columns.to_list()
    df = df.drop(lst[31:], axis=1)
    # rename
    df = df.rename(columns={'C1':'Time', 'C2':'MachineID', 'C3':'Type', 'C4':'EventID', 'C5':'StartEnd', 'C6':'Material'})
    df = df.rename(columns={'C7':'Source', 'C8':'Target', 'C9':'NodeID', 'C10':'Offset', 'C11':'Velocity'})
    df = df.rename(columns={'C12':'Key1', 'C13':'Value1', 'C14':'Key2', 'C15':'Value2', \
                            'C16':'Key3', 'C17':'Value3', 'C18':'Key4', 'C19':'Value4', \
                            'C20':'Key5', 'C21':'Value5', 'C22':'Key6', 'C23':'Value6', \
                            'C24':'Key7', 'C25':'Value7', 'C26':'Key8', 'C27':'Value8', \
                            'C28':'Key9', 'C29':'Value9', 'C30':'Key10', 'C31':'Value10'})
    return df

# Raw -> Trans Type
def parseTransType(df):
    # drop Key-Value
    lst = df.columns.to_list()
    df = df.drop(lst[32:], axis=1)
    # rename
    df = df.rename(columns={'C1':'Time', 'C2':'MachineID', 'C3':'Type', 'C4':'EventID', 'C5':'StartEnd', 'C6':'Material'})
    df = df.rename(columns={'C7':'StationID', 'C8':'PortType', 'C9':'NodeID', 'C10':'Offset'})
    df = df.rename(columns={'C11':'Key1', 'C12':'Value1', 'C13':'Key2', 'C14':'Value2', \
                            'C15':'Key3', 'C16':'Value3', 'C17':'Key4', 'C18':'Value4', \
                            'C19':'Key5', 'C20':'Value5', 'C21':'Key6', 'C22':'Value6', \
                            'C23':'Key7', 'C24':'Value7', 'C25':'Key8', 'C26':'Value8', \
                            'C27':'Key9', 'C28':'Value9', 'C29':'Key10', 'C30':'Value10',\
                            'C31':'Key11', 'C32':'Value11'})
    return df


# Raw -> Status Type
def parseStatusType(df):
    # drop Key-Value
    lst = df.columns.to_list()
    df = df.drop(lst[54:], axis=1)
    # rename
    df = df.rename(columns={'C1':'Time', 'C2':'MachineID', 'C3':'Type', 'C4':'EventID', 'C5':'Material'})
    df = df.rename(columns={'C6':'NodeID', 'C7':'Offset', 'C8':'Velocity'})
    df = df.rename(columns={'C9':'Key1', 'C10':'Value1', 'C11':'Key2', 'C12':'Value2', \
                            'C13':'Key3', 'C14':'Value3', 'C15':'Key4', 'C16':'Value4', \
                            'C17':'Key5', 'C18':'Value5', 'C19':'Key6', 'C20':'Value6', \
                            'C21':'Key7', 'C22':'Value7', 'C23':'Key8', 'C24':'Value8', \
                            'C25':'Key9', 'C26':'Value9', 'C27':'Key10', 'C28':'Value10',\
                            'C29':'Key11', 'C30':'Value11', 'C31':'Key12', 'C32':'Value12',\
                            'C33':'Key13', 'C34':'Value13', 'C35':'Key14', 'C36':'Value14',\
                            'C37':'Key15', 'C38':'Value15', 'C39':'Key16', 'C40':'Value16',\
                            'C41':'Key17', 'C42':'Value17', 'C43':'Key18', 'C44':'Value18',\
                            'C45':'Key19', 'C46':'Value19', 'C47':'Key20', 'C48':'Value20',\
                            'C49':'Key21', 'C50':'Value21', 'C51':'Key22', 'C52':'Value22',\
                            'C53':'Key23', 'C54':'Value23'})
    return df

# Log File to loading
def log2dataframe(fPath, site, dateTime, fp_data):
    fp = fPath + r'/' + site + r'/V*/MCC' + dateTime + '.log'
    files = glob.glob(fp)
    for f, cnt in zip(files,range(len(files))):
        vhl_name=f.split('\\')[-2]
        logger_func.info('%d / %d : %s', cnt+1, len(files), vhl_name)
        # 0. check file exist - by status file
        fp = fp_data + r'/' + site + r'/Data/'+vhl_name+r'/'+vhl_name+'_'+dateTime+'_Status.pklz'
        if(os.path.isfile(fp) == True):
            logger_func.debug('Exist file: %s', fp)
            continue        
        
        # 1. read log
        df = pd.read_csv (f, names = ['C1','C2', 'C3', 'C4', 'C5', 'C6', 'C7', 'C8', 'C9', 'C10', \
                                     'C11', 'C12', 'C13', 'C14', 'C15', 'C16','C17','C18','C19','C20',\
                                     'C21', 'C22', 'C23', 'C24', 'C25', 'C26','C27','C28','C29','C30',\
                                     'C31', 'C32', 'C33', 'C34', 'C35', 'C36','C37','C38','C39','C40',\
                                     'C41', 'C42', 'C43', 'C44', 'C45', 'C46','C47','C48','C49','C50',\
                                     'C51', 'C52', 'C53', 'C54', 'C55', 'C56','C57','C58','C59','C60'])
        
        # 2. compress(zip) & delete log
        z = zipfile.ZipFile(f + r'g.zip', 'w')
        z.write(f, f, compress_type = zipfile.ZIP_DEFLATED )       
        
        os.remove(f)
        
        # 2-1. File Name
        strDate = f[-14:-10] + '-' + f[-10:-8] + '-' + f[-8:-6] #yyyyMMdd
        dfBuf =df['C1']
    
        lst = []
        for strTime in dfBuf:
            if(isinstance(strTime, str)):
                strDateTime = strDate + ' ' + strTime[1:13]
                lst.append(strDateTime)
            else:
                logger_func.warning('[log2dataframe] exception strTime Invalid..!!')
                lst.append(strDate + ' ' + '000000000000')
        df['C1'] = lst  
                
        # 3. Type(Drive, Trans, Status) Ã«Â³?Parsing
        dfDrv = df[df['C3'] == 'DRV']
        dfDrv = parseDriveType(dfDrv)        
        dfTrans = df[df['C3'] == 'TRANS']
        dfTrans = parseTransType(dfTrans)
        dfStatus = df[df['C3'] == 'STS']
        dfStatus = parseStatusType(dfStatus)
        
        # 4. make directory
        #if(type(df.loc[0]['C2']) == type('machine')):
        if(isinstance(df.loc[0]['C2'], str)):
            machieid=df.loc[0]['C2']
        else:
            machieid=df.loc[1]['C2']
        
        f = r'D:/DiagMCCData/' + site + r'/Data/' + machieid + r'/'
        if not os.path.isdir(f):
            os.mkdir(f)
        
        # 5. Pickle dump - each type        
        f= f + machieid + '_' + dateTime + '.pklz'
        #DumpPickleFile(df, f, True)
        ff = f[:-5] + '_Drive.pklz'
        DumpPickleFile(dfDrv, ff, True)
        ff = f[:-5] + '_Trans.pklz'
        DumpPickleFile(dfTrans, ff, True)
        ff = f[:-5] + '_Status.pklz'
        DumpPickleFile(dfStatus, ff, True)


def log2dataframeByVHL(fPath, site, dateTime, lst_vhl, fp_data):
    for l in lst_vhl:
        logger_func.debug('VHL: %s', l)
        
        # check file exist - by status file
        fp = fp_data + r'/' + site + r'/Data/'+l+r'/'+ l +'_'+dateTime+ '_Status.pklz'
        if(os.path.isfile(fp) == True):
            logger_func.debug('Exist file: %s', fp)
            continue
        
        fp = fPath + r'/' + site + r'/'+l+r'/MCC' + dateTime + '.log'
        files = glob.glob(fp)
        for f, cnt in zip(files,range(len(files))):
            logger_func.info('%d / %d : %s', cnt+1, len(files), l)
            # 1. read log
            df = pd.read_csv (f, names = ['C1','C2', 'C3', 'C4', 'C5', 'C6', 'C7', 'C8', 'C9', 'C10', \
                                         'C11', 'C12', 'C13', 'C14', 'C15', 'C16','C17','C18','C19','C20',\
                                         'C21', 'C22', 'C23', 'C24', 'C25', 'C26','C27','C28','C29','C30',\
                                         'C31', 'C32', 'C33', 'C34', 'C35', 'C36','C37','C38','C39','C40',\
                                         'C41', 'C42', 'C43', 'C44', 'C45', 'C46','C47','C48','C49','C50',\
                                         'C51', 'C52', 'C53', 'C54', 'C55', 'C56','C57','C58','C59','C60'])
            
            # 2. compress(zip) & delete log
            z = zipfile.ZipFile(f + r'g.zip', 'w')
            z.write(f, f, compress_type = zipfile.ZIP_DEFLATED )       

            os.remove(f)
            
            # 2-1. File Name
            strDate = f[-14:-10] + '-' + f[-10:-8] + '-' + f[-8:-6] #yyyyMMdd
            dfBuf =df['C1']
        
            lst = []
            for strTime in dfBuf:
                if(isinstance(strTime, str)):
                    strDateTime = strDate + ' ' + strTime[1:13]
                    lst.append(strDateTime)
                else:
                    logger_func.warning('[log2dataframeByVHL] exception strTime Invalid..!!')
                    lst.append(strDate + ' ' + '000000000000')
            df['C1'] = lst              
            
            # 3. Type(Drive, Trans, Status)
            dfDrv = df[df['C3'] == 'DRV']
            dfDrv = parseDriveType(dfDrv)        
            dfTrans = df[df['C3'] == 'TRANS']
            dfTrans = parseTransType(dfTrans)
            dfStatus = df[df['C3'] == 'STS']
            dfStatus = parseStatusType(dfStatus)
            
            # 4. make directory
            #if(type(df.loc[0]['C2']) == type('machine')):
            if(isinstance(df.loc[0]['C2'], str)):
                machieid=df.loc[0]['C2']
            else:
                machieid=df.loc[1]['C2']
            
            f = r'D:/DiagMCCData/' + site + r'/Data/' + machieid + r'/'
            if not os.path.isdir(f):
                os.mkdir(f)
            
            # 5. Pickle dump - each type        
            f= f + machieid + '_' + dateTime + '.pklz'
            
            #DumpPickleFile(df, f, True)
            ff = f[:-5] + '_Drive.pklz'
            DumpPickleFile(dfDrv, ff, True)
            ff = f[:-5] + '_Trans.pklz'
            DumpPickleFile(dfTrans, ff, True)
            ff = f[:-5] + '_Status.pklz'
            DumpPickleFile(dfStatus, ff, True)

# Log File to loading
def log2dataframeEx(fPath, site, dateTime, fp_data):
    # .log list
    fp = fPath + r'/' + site + r'/V*/MCC' + dateTime + '.log.zip'
    files = glob.glob(fp)
    for f, cnt in zip(files,range(len(files))):
        vhl_name=f.split('\\')[-2]
        logger_func.info('%d / %d : %s', cnt+1, len(files), vhl_name)
        
        # 0. check file exist - by status file
        fp = fp_data + r'/' + site + r'/Data/'+vhl_name+r'/'+vhl_name+'_'+dateTime+'_Status.pklz'
        if(os.path.isfile(fp) == True):
            logger_func.debug('Exist file: %s', fp)
            continue        
        
        zf=zipfile.ZipFile(f)
        lf=f[-21:-4]
        # 1. read log
        df = pd.read_csv (zf.open(lf), names = ['C1','C2', 'C3', 'C4', 'C5', 'C6', 'C7', 'C8', 'C9', 'C10', \
                                                'C11', 'C12', 'C13', 'C14', 'C15', 'C16','C17','C18','C19','C20',\
                                                'C21', 'C22', 'C23', 'C24', 'C25', 'C26','C27','C28','C29','C30',\
                                                'C31', 'C32', 'C33', 'C34', 'C35', 'C36','C37','C38','C39','C40',\
                                                'C41', 'C42', 'C43', 'C44', 'C45', 'C46','C47','C48','C49','C50',\
                                                'C51', 'C52', 'C53', 'C54', 'C55', 'C56','C57','C58','C59','C60'])        
        
        # 2-1. File Name
        strDate = f[-18:-14] + '-' + f[-14:-12] + '-' + f[-12:-10] #yyyyMMdd
        dfBuf =df['C1']
    
        lst = []
        for strTime in dfBuf:
            #if(type(strTime)==type('str')):
            if(isinstance(strTime, str)):
                strDateTime = strDate + ' ' + strTime[1:13]
                lst.append(strDateTime)
            else:
                logger_func.warning('[log2dataframeEx] exception strTime Invalid..!!')
                lst.append(strDate + ' ' + '000000000000')
        df['C1'] = lst  
                
        # 3. Type(Drive, Trans, Status) Ã«Â³?Parsing
        dfDrv = df[df['C3'] == 'DRV']
        dfDrv = parseDriveType(dfDrv)        
        dfTrans = df[df['C3'] == 'TRANS']
        dfTrans = parseTransType(dfTrans)
        dfStatus = df[df['C3'] == 'STS']
        dfStatus = parseStatusType(dfStatus)
        
        # 4. make directory
        #if(type(df.loc[0]['C2']) == type('machine')):
        if(isinstance(df.loc[0]['C2'], str)):
            machieid=df.loc[0]['C2']
        else:
            machieid=df.loc[1]['C2']
        
        f = r'D:/DiagMCCData/' + site + r'/Data/' + machieid + r'/'
        if not os.path.isdir(f):
            os.mkdir(f)
        
        # 5. Pickle dump - each type        
        f= f + machieid + '_' + dateTime + '.pklz'
        #DumpPickleFile(df, f, True)
        ff = f[:-5] + '_Drive.pklz'
        DumpPickleFile(dfDrv, ff, True)
        ff = f[:-5] + '_Trans.pklz'
        DumpPickleFile(dfTrans, ff, True)
        ff = f[:-5] + '_Status.pklz'
        DumpPickleFile(dfStatus, ff, True)

def log2dataframeByVHLEx(fPath, site, dateTime, lst_vhl, fp_data):
    for l in lst_vhl:
        logger_func.debug('VHL: %s', l)
        
        # check file exist - by status file
        fp = fp_data + r'/' + site + r'/Data/'+l+r'/'+ l +'_'+dateTime+ '_Status.pklz'
        if(os.path.isfile(fp) == True):
            logger_func.debug('Exist file: %s', fp)
            continue
        
        fp = fPath + r'/' + site + r'/'+l+r'/MCC' + dateTime + '.log.zip'
        files = glob.glob(fp)
        for f, cnt in zip(files,range(len(files))):
            logger_func.info('%d / %d : %s', cnt+1, len(files), l)
            zf=zipfile.ZipFile(f)
            lf=f[-21:-4]
            # 1. read log
            df = pd.read_csv (zf.open(lf), names = ['C1','C2', 'C3', 'C4', 'C5', 'C6', 'C7', 'C8', 'C9', 'C10', \
                                                    'C11', 'C12', 'C13', 'C14', 'C15', 'C16','C17','C18','C19','C20',\
                                                    'C21', 'C22', 'C23', 'C24', 'C25', 'C26','C27','C28','C29','C30',\
                                                    'C31', 'C32', 'C33', 'C34', 'C35', 'C36','C37','C38','C39','C40',\
                                                    'C41', 'C42', 'C43', 'C44', 'C45', 'C46','C47','C48','C49','C50',\
                                                    'C51', 'C52', 'C53', 'C54', 'C55', 'C56','C57','C58','C59','C60'])        
            
            # 2-1. File Name
            strDate = f[-18:-14] + '-' + f[-14:-12] + '-' + f[-12:-10] #yyyyMMdd
            dfBuf =df['C1']
        
            lst = []
            for strTime in dfBuf:
                if(isinstance(strTime, str)):
                    strDateTime = strDate + ' ' + strTime[1:13]
                    lst.append(strDateTime)
                else:
                    logger_func.warning('[log2dataframeByVHLEx] exception strTime Invalid..!!')
                    lst.append(strDate + ' ' + '000000000000')
            df['C1'] = lst              
            
            # 3. Type(Drive, Trans, Status) Ã«Â³?Parsing
            dfDrv = df[df['C3'] == 'DRV']
            dfDrv = parseDriveType(dfDrv)        
            dfTrans = df[df['C3'] == 'TRANS']
            dfTrans = parseTransType(dfTrans)
            dfStatus = df[df['C3'] == 'STS']
            dfStatus = parseStatusType(dfStatus)
            
            # 4. make directory
            #if(type(df.loc[0]['C2']) == type('machine')):
            if(isinstance(df.loc[0]['C2'], str)):
                machieid=df.loc[0]['C2']
            else:
                machieid=df.loc[1]['C2']
            
            f = r'D:/DiagMCCData/' + site + r'/Data/' + machieid + r'/'
            if not os.path.isdir(f):
                os.mkdir(f)
            
            # 5. Pickle dump - each type        
            f= f + machieid + '_' + dateTime + '.pklz'
            
            #DumpPickleFile(df, f, True)
            ff = f[:-5] + '_Drive.pklz'
            DumpPickleFile(dfDrv, ff, True)
            ff = f[:-5] + '_Trans.pklz'
            DumpPickleFile(dfTrans, ff, True)
            ff = f[:-5] + '_Status.pklz'
            DumpPickleFile(dfStatus, ff, True)


# Loading Drive pickles 
def loadPickleDriveByDataTime(fPath, site, dateTime):
    logger_func.info('loadPickleDriveByDataTime: %s , %s', site, dateTime)

    fp = fPath + r'/' + site + r'/Data/V*/V?????_' + dateTime + '_Drive.pklz'
    files = glob.glob(fp)
    df_all = pd.DataFrame()
    for f, cnt in zip(files,range(len(files))):
        logger_func.info('%d / %d : Drv - %s', cnt+1, len(files), dateTime)
        # 1. Load & Append Pickles
        df = LoadPickleFile(f, True)
        df_all = df_all.append(df, ignore_index=True)
        
    return df_all

def loadPickleDriveByDataTimenVHL(fPath, site, dateTime, lst_vhl):
    logger_func.info('loadPickleDriveByDataTimenVHL: %s , %s', site, dateTime)
    
    df_all = pd.DataFrame()
    for l in lst_vhl:
        logger_func.info('%s', l)
        fp = fPath + r'/' + site + r'/Data/'+l+'/V?????_' + dateTime + '_Drive.pklz'
        files = glob.glob(fp)        
        for f, cnt in zip(files,range(len(files))):            
            # 1. Load & Append Pickles
            df = LoadPickleFile(f, True)
            df_all = df_all.append(df, ignore_index=True)
        
    return df_all

# Loading Trans pickles
def loadPickleTransByDataTime(fPath, site, dateTime):
    logger_func.info('loadPickleTransByDataTime: %s , %s', site, dateTime)
    
    fp = fPath + r'/' + site + r'/Data/V*/V?????_' + dateTime + '_Trans.pklz'
    files = glob.glob(fp)
    df_all = pd.DataFrame()
    for f, cnt in zip(files,range(len(files))):
        logger_func.info('%d / %d : Trans - %s', cnt+1, len(files), dateTime)
        # 1. Load & Append Pickles
        df = LoadPickleFile(f, True)
        df_all = df_all.append(df, ignore_index=True)
        
    return df_all

def loadPickleTransByDataTimenVHL(fPath, site, dateTime, lst_vhl):
    logger_func.info('loadPickleTransByDataTimenVHL: %s , %s', site, dateTime)

    df_all = pd.DataFrame()
    for l in lst_vhl:
        logger_func.info('%s', l)
        fp = fPath + r'/' + site + r'/Data/'+l+'/V?????_' + dateTime + '_Trans.pklz'
        files = glob.glob(fp)        
        for f, cnt in zip(files,range(len(files))):            
            # 1. Load & Append Pickles
            df = LoadPickleFile(f, True)
            df_all = df_all.append(df, ignore_index=True)
        
    return df_all

# Loading Status pickles
def loadPickleStatusByDataTime(fPath, site, dateTime):
    logger_func.info('loadPickleStatusByDataTime: %s , %s', site, dateTime)
    
    fp = fPath + r'/' + site + r'/Data/V*/V?????_' + dateTime + '_Status.pklz'
    files = glob.glob(fp)
    df_all = pd.DataFrame()
    for f, cnt in zip(files,range(len(files))):
        logger_func.info('%d / %d : Sts - %s', cnt+1, len(files), dateTime)
        # 1. Load & Append Pickles
        df = LoadPickleFile(f, True)
        df_all = df_all.append(df, ignore_index=True)
    
    return df_all

def loadPickleStatusByDataTimenVHL(fPath, site, dateTime, lst_vhl):
    logger_func.info('loadPickleStatusByDataTimenVHL: %s , %s', site, dateTime)

    df_all = pd.DataFrame()
    for l in lst_vhl:
        logger_func.info('%s', l)
        fp = fPath + r'/' + site + r'/Data/'+l+'/V?????_' + dateTime + '_Status.pklz'
        files = glob.glob(fp)        
        for f, cnt in zip(files,range(len(files))):            
            # 1. Load & Append Pickles
            df = LoadPickleFile(f, True)
            df_all = df_all.append(df, ignore_index=True)
        
    return df_all


def calcHealthIndex(df):
    return 0

def addHealthIndex(name, df_total, df_item):
    if(len(df_item)>0):
        df_total[name]=df_item['total']
    else:
        df_total[name]=0 

    return df_total

# Save Text
# file_name - except extension(.xlsx)
def save_text(file_path, file_name, df, lst_column):
    df[lst_column].to_csv(file_path+file_name+'.csv.gz', compression='gzip')

# Save Excel
# file_name - except extension(.xlsx)
def save_excel(file_path, file_name, df, lst_column, sheet):    
    if not os.path.isdir(file_path):
        os.mkdir(file_path)
    
    with pd.ExcelWriter(file_path+file_name+'.xlsx') as writer:
        df[lst_column].to_excel(writer, sheet_name=sheet)

# Save RawData - select format by df length
def save_rawData_old(file_path, file_name, df, lst_column, sheet):
    start=time.perf_counter()    
    if(len(df) > 500000):
        logger_func.info('Save CSV Start: %s', file_name)
        save_text(file_path, file_name, df, lst_column)                
    else:
        logger_func.info('Save Excel Start: %s', file_name)
        save_excel(file_path, file_name, df, lst_column, sheet)
        
    logger_func.info('Save Time: %.2f', time.perf_counter()-start)

# Save RawData - only csv <- BI integration
def save_rawData(file_path, file_name, df, lst_column, sheet):
    start=time.perf_counter()    
    logger_func.info('Save CSV Start: %s', file_name)
    save_text(file_path, file_name, df, lst_column)                
    logger_func.info('Save Time: %.2f', time.perf_counter()-start)

def my_plot_cpk(title, df, plot_xy, fig_size, scale_font, limit_y, ref_y, is_save, filename, ref, summary):
    # size & title
    fig=plt.figure(figsize=fig_size)
    plt.title(title, fontsize=30)
    
    [p_width, p_height]=fig.get_size_inches()*fig.dpi
    print(f'{p_width},{p_height}')    
    
    # axis-y limit set
    lim_y_min=limit_y['min']
    lim_y_max=limit_y['max']
    if(df[plot_xy['y']].max() > lim_y_max):
        lim_y_max=df[plot_xy['y']].max() * 1.1
    if(df[plot_xy['y']].min() < lim_y_min):
        lim_y_min=df[plot_xy['y']].min() * 1.1
    plt.ylim(lim_y_min,lim_y_max)
    
    plt.xticks(rotation = 45) #x축값 기울이기 추가
    plt.grid() #선표시추가
    
    for i in range(len(ref)):
        if(ref.iloc[i]['key_name'] == title):
            mean=ref.iloc[i]['mean']
            LSL=ref.iloc[i]['LSL']
            USL=ref.iloc[i]['USL']
            print(f'{i}, {LSL}, {USL}')
            break    
    
    plt.axhspan(ref_y['min'],ref_y['max'],facecolor='b', alpha=0.15)
    plt.axhspan(LSL, USL, facecolor='r', alpha=0.3) 
    plt.axhline(mean, ls='--', c='blue')
    
    g = sns.boxplot(x=plot_xy['x'], y=plot_xy['y'], data=df)
    g.set_xlabel(plot_xy['x'],fontsize=20) #추가
    g.set_ylabel(plot_xy['y'],fontsize=20) #추가
    
    g.tick_params(labelsize=20) #추가
    for label in g.get_xticklabels(): #추가
        label.set_fontsize(12) #추가
    for label in g.get_yticklabels():#추가
        label.set_fontsize(15)#추가    
    
    tyr=ty1=(ref_y['max']-ref_y['min'])*1.1 + ref_y['min']
    ty3=ty4=(ref_y['max']-ref_y['min'])*1.2 + ref_y['min']
    
    g.text(x=(p_width-100), y=(p_height-190),s="reference",bbox=dict(facecolor='green', alpha=0.5), transform=None)
    g.text(x=(p_width-100), y=(p_height-160),s="Normal",bbox=dict(facecolor='white', alpha=0.4), transform=None)
    g.text(x=(p_width-100), y=(p_height-130),s="Grade-3",bbox=dict(facecolor='yellow', alpha=0.3), transform=None)
    g.text(x=(p_width-100), y=(p_height-100),s="Grade-4",bbox=dict(facecolor='red', alpha=0.6), transform=None)
    
    for i in range(len(summary)):
        if summary['Cpk'][i] < 0.67: # 공정능력 4등급
            g.text(x=i, y=ty4, s=summary['Cpk'][i],\
                   horizontalalignment='center',bbox=dict(facecolor='red', alpha=0.6))
        elif 1> summary['Cpk'][i] >= 0.67: # 공정능력 3등급
            g.text(x=i, y=ty3, s=summary['Cpk'][i],\
                   horizontalalignment='center',bbox=dict(facecolor='yellow', alpha=0.3))
        elif summary['Cpk'][i] == 2.0: # 레퍼런스 호기
            g.text(x=i, y=tyr, s=summary['Cpk'][i],\
                   horizontalalignment='center',bbox=dict(facecolor='green', alpha=0.5))
        else: # 공정능력 2~특급
            g.text(x=i, y=ty1, s=summary['Cpk'][i],\
                   horizontalalignment='center',bbox=dict(facecolor='white', alpha=0.4))
    
    # save image
    if(is_save==True):
        plt.savefig(filename)
        
    plt.close()

def my_plot_all(title, df, plot_xy, fig_size, scale_font, limit_y, ref_y, is_save, filename):
    # size & title
    plt.figure(figsize=fig_size)
    plt.title(title, fontsize=35)
  
    # axis-y limit set
    lim_y_min=limit_y['min']
    lim_y_max=limit_y['max']
    if(df[plot_xy['y']].max() > lim_y_max):
        lim_y_max=df[plot_xy['y']].max() * 1.1
    if(df[plot_xy['y']].min() < lim_y_min):
        lim_y_min=df[plot_xy['y']].min() * 1.1
    plt.ylim(lim_y_min,lim_y_max)
    
    plt.xticks(rotation = 45) #x축값 기울이기 추가
    plt.grid() #선표시추가
    plt.axhspan(ref_y['min'],ref_y['max'],facecolor='b', alpha=0.1)
    plt.axhline(ref_y['ref'], ls='--', c='red')
    sns.set_context('notebook',font_scale=scale_font)
    g=sns.boxplot(x=plot_xy['x'], y=plot_xy['y'], data=df)
    g.set_xlabel(plot_xy['x'],fontsize=25) #추가
    g.set_ylabel(plot_xy['y'],fontsize=20) #추가
    g.tick_params(labelsize=10) #추가
    
    for label in g.get_xticklabels(): #추가
        label.set_fontsize(15) #추가
    for label in g.get_yticklabels():#추가
        label.set_fontsize(18)#추가    
    
    # save image
    if(is_save==True):
        plt.savefig(filename)
        
    plt.close()
    
def my_plot_filtered(title, df,plot_xy, fig_size, scale_font, limit_y, ref_y, is_save,filename):
    # size & title
    plt.figure(figsize=fig_size)
    plt.title(title)
    
    #pd.options.display.float_format = '{:.2f}'.format
    df[[plot_xy['x'],plot_xy['y']]].groupby(plot_xy['x']).describe().to_csv(filename[:-4]+'.tsv', sep='\t')
       
    # df filter
    df_abnormal=df[(df[plot_xy['y']]<ref_y['min']) | (df[plot_xy['y']]>ref_y['max'])]        
    if(len(df_abnormal)>0):
        df_filtered=pd.DataFrame()
        lst=df_abnormal[plot_xy['x']].unique().tolist()
        for l in lst:
            df_buf=df[df[plot_xy['x']]==l]
            df_filtered=df_filtered.append(df_buf)
        df=df_filtered
        
        # axis-y limit set
        lim_y_min=limit_y['min']
        lim_y_max=limit_y['max']
        if(df[plot_xy['y']].max() > lim_y_max):
            lim_y_max=df[plot_xy['y']].max() * 1.1
        if(df[plot_xy['y']].min() < lim_y_min):
            lim_y_min=df[plot_xy['y']].min() * 1.1
        plt.ylim(lim_y_min,lim_y_max)
    
        plt.axhspan(ref_y['min'],ref_y['max'],facecolor='b', alpha=0.1)
        sns.set_context('notebook',font_scale=scale_font)
        sns.boxplot(x=plot_xy['x'], y=plot_xy['y'], data=df)
        
        # save image
        if(is_save==True):
            plt.savefig(filename)            
            
        plt.close()
    else:
        logger_func.info('%s abnormal data is None..!!', title)
        plt.close()


def my_plot(title, df,plot_xy, fig_size, scale_font, limit_y, ref_y, is_save,filename, filteruse):
    df=df.sort_values(by=[plot_xy['x']], axis=0)
    if(filteruse == True):
        my_plot_filtered(title, df,plot_xy, fig_size, scale_font, limit_y, ref_y, is_save,filename)
    else:        
        my_plot_all(title, df,plot_xy, fig_size, scale_font, limit_y, ref_y, is_save,filename)

###############################################################################
####    
###############################################################################
class DiagType1:
    
    def __init__(self, filter_event, col_list, df_ref):
        self.filter_event=filter_event
        self.col_list=col_list
        self.col_key=[]
        for c in col_list:
            if(c[:5] == 'Value'):
                self.col_key.append(c)
        self.df_ref=df_ref
        self.df=pd.DataFrame()
        self.col_name=[]
        self.logger=logging.getLogger('main.func.diagtype1')


    def colDeleteSpace(self, df):
        colKey=self.col_key
        colName=[]
        for c,i in zip(colKey, range(len(colKey))):
            col='Key'+colKey[i][5:]
            
            #if(len(df)>1):
            #    colName.append(df.iloc[1][col])
            if(len(df)>0):
                colName.append(df.iloc[0][col])
            else:
                colName.append('Empty'+str(i+1))
            
        for c,i in zip(colName, range(len(colName))):
            self.logger.info('%s abnormal data is None..!!', c)
            if(c[:5]!='Empty'):
                colName[i] = c.replace(" ", "")
            
        return colName
    
    # Implementation Functions
    def filterRaw(self, df):
        # Index Filter by Event
        for item in self.filter_event.items():
            df = df.loc[(df[item[0]]==item[1])]
        # Key Name -> Column
        colName = self.colDeleteSpace(df)
        self.col_name = colName

        # Column Filter by Key-Value
        df=df[self.col_list]
        # Convert Column : Value -> Key Name
        for ck, cn in zip(self.col_key, colName):
            df.rename(columns={ck:cn},inplace=True)    
        # Value?
        df[colName]=df[colName].apply(pd.to_numeric,errors = 'coerce')

        return df        
    
    def filterRaw2(self, df):
        # Index Filter by Event
        for item in self.filter_event.items():
            df = df.loc[(df[item[0]]==item[1])]
        # Key Name -> Column ë³??? ANOBA ?ê³ ë¦¬ì¦ ?¬ì© ??ê³µë°± ?¬ì© ë¶ê?ë¡?ê³µë°± ?ê±°
        colName = self.colDeleteSpace(df)
        self.col_name = colName

        return df            
    
    ###########################################################################
    ## Apply Method    
    # Key ë³?ê¸°ì?ê°?ë¹êµ -> health index ?ì¶
    # df - Health index ?°ì°??Dataframe
    # dfRef - ê¸°ì? Data
    def calcHealthIndex(self, df):
        retValue=0
        dfRef = self.df_ref
        colName=self.col_name
        lst=dfRef.columns.to_list()
        for i in range(len(lst)):
            if dfRef.loc['Logic'][lst[i]]=='GT':
                if df[colName[i]]>dfRef.loc['Max'][lst[i]]:
                    retValue += dfRef.loc['Weight'][lst[i]]                    
                    
            if dfRef.loc['Logic'][lst[i]]=='LT':
                if df[colName[i]]<dfRef.loc['Min'][lst[i]]:
                    retValue += dfRef.loc['Weight'][lst[i]]
                    
            if dfRef.loc['Logic'][lst[i]]=='GLT':
                if df[colName[i]]>dfRef.loc['Max'][lst[i]]:
                    retValue += dfRef.loc['Weight'][lst[i]]
                if df[colName[i]]<dfRef.loc['Min'][lst[i]]:
                    retValue += dfRef.loc['Weight'][lst[i]]
                    
            if dfRef.loc['Logic'][lst[i]]=='EQUAL':
                if df[colName[i]]==dfRef.loc['Ref'][lst[i]]:
                    retValue += dfRef.loc['Weight'][lst[i]]
                    
            if dfRef.loc['Logic'][lst[i]]=='DIFF':
                if df[colName[i]]!=dfRef.loc['Ref'][lst[i]]:
                    retValue += dfRef.loc['Weight'][lst[i]]
            
        return retValue    

    # Machine/Node/Station ID ë³?health index ?ì¶ 
    def filterHealthIndexMachine(self, df, col_name):    
        df=df[df[col_name] > 0] 
        dfReturn=pd.DataFrame(df[col_name].groupby(df['MachineID']).sum()) 
        return dfReturn
    
    def filterHealthIndexNode(self, df, col_name):    
        df=df[df[col_name] > 0] 
        dfReturn=pd.DataFrame(df[col_name].groupby(df['NodeID']).sum()) 
        return dfReturn
    
    def filterHealthIndexStation(self, df, col_name, colStationID):    
        df=df[df[col_name] > 0] 
        dfReturn=pd.DataFrame(df[col_name].groupby(df[colStationID]).sum()) 
        return dfReturn


    def calcHealthIndexNodeID(self, df):
        dfRet=pd.DataFrame(index = df['NodeID'].unique())
        dfRef = self.df_ref
        colName=self.col_name
        lst=dfRef.columns.to_list()
        for c,i in zip(colName,range(len(colName))):
            if dfRef.loc['Logic'][lst[i]]=='GT':
                dff=df[['NodeID', c]]
                dfg = dff[dff[c]>dfRef.loc['Max'][lst[i]]].groupby(dff['NodeID'])
                dfHi=dfg.count().drop('NodeID', axis=1)
                dfRet[c] = dfHi * dfRef.loc['Weight'][lst[i]]
                
            if dfRef.loc['Logic'][lst[i]]=='LT':
                dff=df[['NodeID', c]]
                dfg = dff[dff[c]<dfRef.loc['Min'][lst[i]]].groupby(dff['NodeID'])
                dfHi=dfg.count().drop('NodeID', axis=1)
                dfRet[c] = dfHi * dfRef.loc['Weight'][lst[i]]
                
            if dfRef.loc['Logic'][lst[i]]=='GLT':
                dff=df[['NodeID', c]]
                dfg = dff[dff[c]>dfRef.loc['Max'][lst[i]]].groupby(dff['NodeID'])
                dfHi=dfg.count().drop('NodeID', axis=1)
                dfg = dff[dff[c]<dfRef.loc['Min'][lst[i]]].groupby(dff['NodeID'])
                dfHi+=dfg.count().drop('NodeID', axis=1)
                dfRet[c] = dfHi * dfRef.loc['Weight'][lst[i]]
                
            if dfRef.loc['Logic'][lst[i]]=='EQUAL':
                dff=df[['NodeID', c]]
                dfg = dff[dff[c]==dfRef.loc['Ref'][lst[i]]].groupby(dff['NodeID'])
                dfHi=dfg.count().drop('NodeID', axis=1)
                dfRet[c] = dfHi * dfRef.loc['Weight'][lst[i]]
                
            if dfRef.loc['Logic'][lst[i]]=='DIFF':
                dff=df[['NodeID', c]]
                dfg = dff[dff[c]!=dfRef.loc['Ref'][lst[i]]].groupby(dff['NodeID'])
                dfHi=dfg.count().drop('NodeID', axis=1)
                dfRet[c] = dfHi * dfRef.loc['Weight'][lst[i]]
        
        dfRet['total'] = dfRet.sum(axis=1)
        return dfRet    
    
    ###########################################################################
    ## Quick Method
    def calcHealthIndexQuick(self, diag_type, df):
        dfRet=pd.DataFrame(index = df[diag_type].unique())
        if(len(dfRet)==0):
            return dfRet
        dfRef = self.df_ref
        colName=self.col_name
        lst=dfRef.columns.to_list()
        for c,i in zip(colName,range(len(colName))):
            if dfRef.loc['Logic'][lst[i]]=='GT':
                dff=df[[diag_type, c]]
                dfg = dff[dff[c]>dfRef.loc['Max'][lst[i]]].groupby(dff[diag_type])
                dfHi=dfg.count().drop(diag_type, axis=1)
                dfRet[c] = dfHi * dfRef.loc['Weight'][lst[i]]
                
            if dfRef.loc['Logic'][lst[i]]=='LT':
                dff=df[[diag_type, c]]
                dfg = dff[dff[c]<dfRef.loc['Min'][lst[i]]].groupby(dff[diag_type])
                dfHi=dfg.count().drop(diag_type, axis=1)
                dfRet[c] = dfHi * dfRef.loc['Weight'][lst[i]]
                
            if dfRef.loc['Logic'][lst[i]]=='GLT':
                dff=df[[diag_type, c]]
                dfg = dff[dff[c]>dfRef.loc['Max'][lst[i]]].groupby(dff[diag_type])
                dfHi=dfg.count().drop(diag_type, axis=1)
                dfg = dff[dff[c]<dfRef.loc['Min'][lst[i]]].groupby(dff[diag_type])
                dfHi2=dfg.count().drop(diag_type, axis=1)
                #dfHi3=pd.merge(dfHi, dfHi2, left_index=True, right_index=True, how='outer')
                dfRet[c+'GT'] = dfHi * dfRef.loc['Weight'][lst[i]]
                dfRet[c+'LT'] = dfHi2 * dfRef.loc['Weight'][lst[i]]
                
            if dfRef.loc['Logic'][lst[i]]=='EQUAL':
                dff=df[[diag_type, c]]
                dfg = dff[dff[c]==dfRef.loc['Ref'][lst[i]]].groupby(dff[diag_type])
                dfHi=dfg.count().drop(diag_type, axis=1)
                dfRet[c] = dfHi * dfRef.loc['Weight'][lst[i]]
                
            if dfRef.loc['Logic'][lst[i]]=='DIFF':
                dff=df[[diag_type, c]]
                dfg = dff[dff[c]!=dfRef.loc['Ref'][lst[i]]].groupby(dff[diag_type])
                dfHi=dfg.count().drop(diag_type, axis=1)
                dfRet[c] = dfHi * dfRef.loc['Weight'][lst[i]]
        
        dfRet['total'] = dfRet.sum(axis=1)
        return dfRet        

    ###########################################################################
    ## Quick Method - ratio
    def calcHealthIndexQuickRatio(self, diag_type, df):
        dfRet=pd.DataFrame(index = df[diag_type].unique())
        if(len(dfRet)==0):
            return dfRet
        dfRef = self.df_ref
        colName=self.col_name
        lst=dfRef.columns.to_list()
        for c,i in zip(colName,range(len(colName))):
            if dfRef.loc['Logic'][lst[i]]=='GT':
                dff=df[[diag_type, c]]
                dfg_all=dff[c].groupby(dff[diag_type]).describe()
                dfg = dff[dff[c]>dfRef.loc['Max'][lst[i]]].groupby(dff[diag_type])
                if(len(dfg)>0):
                    dfg=dfg.describe()
                    dfHi=dfg[(c,'count')]/dfg_all['count']
                    dfHi=dfHi.fillna(0)
                else:                    
                    dfHi=dfg_all['count']*0                
                dfRet[c+'GT'] = 100.0-(round(dfHi * dfRef.loc['Weight'][lst[i]] * 100.0,1))
                
            if dfRef.loc['Logic'][lst[i]]=='LT':
                dff=df[[diag_type, c]]
                dfg_all=dff[c].groupby(dff[diag_type]).describe()
                dfg = dff[dff[c]<dfRef.loc['Min'][lst[i]]].groupby(dff[diag_type])
                if(len(dfg)>0):
                    dfg=dfg.describe()
                    dfHi=dfg[(c,'count')]/dfg_all['count']
                    dfHi=dfHi.fillna(0)
                else:                    
                    dfHi=dfg_all['count']*0                
                dfRet[c+'LT'] = 100.0-(round(dfHi * dfRef.loc['Weight'][lst[i]] * 100.0,1))
                
            if dfRef.loc['Logic'][lst[i]]=='GLT':
                dff=df[[diag_type, c]]
                dfg_all=dff[c].groupby(dff[diag_type]).describe()
                dfg = dff[dff[c]>dfRef.loc['Max'][lst[i]]].groupby(dff[diag_type])
                if(len(dfg)>0):
                    dfg=dfg.describe()
                    dfHi=dfg[(c,'count')]/dfg_all['count']
                    dfHi=dfHi.fillna(0)
                else:
                    dfHi=dfg_all['count']*0
                
                dfg = dff[dff[c]<dfRef.loc['Min'][lst[i]]].groupby(dff[diag_type])
                if(len(dfg)>0):
                    dfg=dfg.describe()
                    dfHi2=dfg[(c,'count')]/dfg_all['count']                    
                    dfHi2=dfHi2.fillna(0)
                else:
                    dfHi2=dfg_all['count']*0
                #dfHi3=pd.merge(dfHi, dfHi2, left_index=True, right_index=True, how='outer')
                dfRet[c+'GT'] = 100.0-(round(dfHi * dfRef.loc['Weight'][lst[i]] * 100.0,1))
                dfRet[c+'LT'] = 100.0-(round(dfHi * dfRef.loc['Weight'][lst[i]] * 100.0,1))
                
            if dfRef.loc['Logic'][lst[i]]=='EQUAL':
                dff=df[[diag_type, c]]
                dfg_all=dff[c].groupby(dff[diag_type]).describe()
                dfg = dff[dff[c]==dfRef.loc['Ref'][lst[i]]].groupby(dff[diag_type])
                if(len(dfg)>0):
                    dfg=dfg.describe()
                    dfHi=dfg[(c,'count')]/dfg_all['count']
                    dfHi=dfHi.fillna(0)
                else:                    
                    dfHi=dfg_all['count']*0                
                dfRet[c+'EQ'] = 100.0-(round(dfHi * dfRef.loc['Weight'][lst[i]] * 100.0,1))
                
            if dfRef.loc['Logic'][lst[i]]=='DIFF':
                dff=df[[diag_type, c]]
                dfg_all=dff[c].groupby(dff[diag_type]).describe()
                dfg = dff[dff[c]!=dfRef.loc['Ref'][lst[i]]].groupby(dff[diag_type])
                if(len(dfg)>0):
                    dfg=dfg.describe()
                    dfHi=dfg[(c,'count')]/dfg_all['count']
                    dfHi=dfHi.fillna(0)
                else:                    
                    dfHi=dfg_all['count']*0                
                dfRet[c+'DF'] = 100.0-(round(dfHi * dfRef.loc['Weight'][lst[i]] * 100.0,1))
        
        dfRet['total'] = round(dfRet.sum(axis=1)/ len(dfRet.columns),1)
        return dfRet   
    
    def calcHealthIndexQuickRatioEx(self, diag_type, df, lst_diag_col, df_ref):
        '''
        column item을 지정하여 index 산출

        Parameters
        ----------
        diag_type : machine, node, station
        df : dataframe data
        lst_diag_col : column lists
        df_ref : reference value

        Returns
        -------
        dfRet : index dataframe include total

        '''
        dfRet=pd.DataFrame(index = df[diag_type].unique())
        if(len(dfRet)==0):
            return dfRet
        dfRef = df_ref
        colName=lst_diag_col
        lst=dfRef.columns.to_list()
        for c,i in zip(colName,range(len(colName))):
            if dfRef.loc['Logic'][lst[i]]=='GT':
                dff=df[[diag_type, c]]
                dfg_all=dff[c].groupby(dff[diag_type]).describe()
                dfg = dff[dff[c]>dfRef.loc['Max'][lst[i]]].groupby(dff[diag_type])
                if(len(dfg)>0):
                    dfg=dfg.describe()
                    dfHi=dfg[(c,'count')]/dfg_all['count']
                    dfHi=dfHi.fillna(0)
                else:                    
                    dfHi=dfg_all['count']*0                
                dfRet[c+'GT'] = 100.0-(round(dfHi * dfRef.loc['Weight'][lst[i]] * 100.0,1))
                
            if dfRef.loc['Logic'][lst[i]]=='LT':
                dff=df[[diag_type, c]]
                dfg_all=dff[c].groupby(dff[diag_type]).describe()
                dfg = dff[dff[c]<dfRef.loc['Min'][lst[i]]].groupby(dff[diag_type])
                if(len(dfg)>0):
                    dfg=dfg.describe()
                    dfHi=dfg[(c,'count')]/dfg_all['count']
                    dfHi=dfHi.fillna(0)
                else:                    
                    dfHi=dfg_all['count']*0                
                dfRet[c+'LT'] = 100.0-(round(dfHi * dfRef.loc['Weight'][lst[i]] * 100.0,1))
                
            if dfRef.loc['Logic'][lst[i]]=='GLT':
                dff=df[[diag_type, c]]
                dfg_all=dff[c].groupby(dff[diag_type]).describe()
                dfg = dff[dff[c]>dfRef.loc['Max'][lst[i]]].groupby(dff[diag_type])
                if(len(dfg)>0):
                    dfg=dfg.describe()
                    dfHi=dfg[(c,'count')]/dfg_all['count']
                    dfHi=dfHi.fillna(0)
                else:
                    dfHi=dfg_all['count']*0
                
                dfg = dff[dff[c]<dfRef.loc['Min'][lst[i]]].groupby(dff[diag_type])
                if(len(dfg)>0):
                    dfg=dfg.describe()
                    dfHi2=dfg[(c,'count')]/dfg_all['count']                    
                    dfHi2=dfHi2.fillna(0)
                else:
                    dfHi2=dfg_all['count']*0
                #dfHi3=pd.merge(dfHi, dfHi2, left_index=True, right_index=True, how='outer')
                dfRet[c+'GT'] = 100.0-(round(dfHi * dfRef.loc['Weight'][lst[i]] * 100.0,1))
                dfRet[c+'LT'] = 100.0-(round(dfHi * dfRef.loc['Weight'][lst[i]] * 100.0,1))
                
            if dfRef.loc['Logic'][lst[i]]=='EQUAL':
                dff=df[[diag_type, c]]
                dfg_all=dff[c].groupby(dff[diag_type]).describe()
                dfg = dff[dff[c]==dfRef.loc['Ref'][lst[i]]].groupby(dff[diag_type])
                if(len(dfg)>0):
                    dfg=dfg.describe()
                    dfHi=dfg[(c,'count')]/dfg_all['count']
                    dfHi=dfHi.fillna(0)
                else:                    
                    dfHi=dfg_all['count']*0                
                dfRet[c+'EQ'] = 100.0-(round(dfHi * dfRef.loc['Weight'][lst[i]] * 100.0,1))
                
            if dfRef.loc['Logic'][lst[i]]=='DIFF':
                dff=df[[diag_type, c]]
                dfg_all=dff[c].groupby(dff[diag_type]).describe()
                dfg = dff[dff[c]!=dfRef.loc['Ref'][lst[i]]].groupby(dff[diag_type])
                if(len(dfg)>0):
                    dfg=dfg.describe()
                    dfHi=dfg[(c,'count')]/dfg_all['count']
                    dfHi=dfHi.fillna(0)
                else:                    
                    dfHi=dfg_all['count']*0                
                dfRet[c+'DF'] = 100.0-(round(dfHi * dfRef.loc['Weight'][lst[i]] * 100.0,1))
        
        dfRet['total'] = round(dfRet.sum(axis=1)/ len(dfRet.columns),1)
        return dfRet
    


###############################################################################
def get_cpk_ref_data_by_machine(df, machine_id, key_name):
    retVal={'mean':0, 'std':0, 'LSL':0.0, 'LCL':0.0, 'UCL':0.0, 'USL':0.0}
    df=df[df['MachineID']==machine_id]
    mean=df[key_name].mean()
    std=df[key_name].std()
    
    retVal['mean']=mean
    retVal['std']=std
    retVal['LCL']=round(mean - (3*std), 3)
    retVal['UCL']=round(mean + (3*std), 3)
    retVal['LSL']=round(mean - (6*std), 3)
    retVal['USL']=round(mean + (6*std), 3)

    return retVal     
           
    
def calc_cpk(df, key_name, ref, analysis_time):
    df=df[['MachineID', key_name]]

    ret=df.groupby('MachineID')[key_name].describe()
    ret=ret.drop(columns=['25%', '50%', '75%'])

    for i in range(len(ref)):
        if(ref.iloc[i]['key_name'] == key_name):
            LSL=ref.iloc[i]['LSL']
            USL=ref.iloc[i]['USL']
            #print(f'{i}, {LSL}, {USL}')
            break

    ret['K']=round((((USL+LSL)/2)-ret['mean'])/((USL-LSL)/2),3)
    ret['Cp']=round(((USL-LSL)/(6*ret['std'])),3)
    ret['Cpk']=round(ret['Cp']*(1-abs(ret['K'])),3)
    ret['key_name']=key_name
    ret['LSL']=LSL
    ret['USL']=USL
    ret['analysis_time']=analysis_time

    ret=ret.reset_index(level='MachineID')
    ret=ret.rename(columns={'MachineID':'id_machine'})
    
    return ret
    

    
###############################################################################
if __name__ == '__main__': 
    print('Excute diag_functions.py')