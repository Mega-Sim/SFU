# -*- coding: utf-8 -*-
"""
Created on Tue Jan 21 09:51:06 2020

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
import matplotlib.pyplot as plt
import seaborn as sns

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

# Log File to loading - Site ??ëª¨ë  VHL ?´ë¹ ?ê° Log -> Type ë³?Pickle ë³???
def log2dataframe(fPath, site, dateTime):
    fp = fPath + r'/' + site + r'/V*/MCC' + dateTime + '.log'
    files = glob.glob(fp)
    for f, cnt in zip(files,range(len(files))):
        print(str(cnt+1) + ' / ' + str(len(files)))
        # 1. read log
        df = pd.read_csv (f, names = ['C1','C2', 'C3', 'C4', 'C5', 'C6', 'C7', 'C8', 'C9', 'C10', \
                                     'C11', 'C12', 'C13', 'C14', 'C15', 'C16','C17','C18','C19','C20',\
                                     'C21', 'C22', 'C23', 'C24', 'C25', 'C26','C27','C28','C29','C30',\
                                     'C31', 'C32', 'C33', 'C34', 'C35', 'C36','C37','C38','C39','C40',\
                                     'C41', 'C42', 'C43', 'C44', 'C45', 'C46','C47','C48','C49','C50',\
                                     'C51', 'C52', 'C53', 'C54', 'C55', 'C56','C57','C58','C59','C60'])
        
        # 2. compress(zip) & delete log
        z = zipfile.ZipFile(f + r'.zip', 'w')
        z.write(f, f, compress_type = zipfile.ZIP_DEFLATED )       
        
        os.remove(f)
        
        # 2-1. File Name?ì ??????ì¶ì¶?ì¬ timeê³?ë³í©
        strDate = f[-14:-10] + '-' + f[-10:-8] + '-' + f[-8:-6] #yyyyMMdd
        dfBuf =df['C1']
    
        lst = []
        for strTime in dfBuf:
            strDateTime = strDate + ' ' + strTime[1:13]
            lst.append(strDateTime)   
    
        df['C1'] = lst  # ê²½ê³  ë°ì?ë.. ?¼ë¨ ë¬´ì        
        
        
        # 3. Type(Drive, Trans, Status) ë³?Parsing
        dfDrv = df[df['C3'] == 'DRV']
        dfDrv = parseDriveType(dfDrv)        
        dfTrans = df[df['C3'] == 'TRANS']
        dfTrans = parseTransType(dfTrans)
        dfStatus = df[df['C3'] == 'STS']
        dfStatus = parseStatusType(dfStatus)
        
        # 4. make directory
        if(type(df.loc[0]['C2']) == type('machine')):
            machieid=df.loc[0]['C2']
        else:
            machieid=df.loc[1]['C2']
        
        f = r'D:/DiagMCCData/' + site + r'/Data/' + machieid + r'/'
        if not os.path.isdir(f):
            os.mkdir(f)
        
        # 5. Pickle ????- ?ê° ?¨ì¶ ?ì ??1ê°??? (?ì²´ or ê°?????
        # 2ê°??´ì Type ?°ê³ ë¶ì ?ì§ ?ì¼ë¯?ë¡?. ?°ì  Type ë³?êµ¬ë¶?´ëê³??¬ì©?ë¤.
        f= f + machieid + '_' + dateTime + '.pklz'
        #DumpPickleFile(df, f, True)
        ff = f[:-5] + '_Drive.pklz'
        DumpPickleFile(dfDrv, ff, True)
        ff = f[:-5] + '_Trans.pklz'
        DumpPickleFile(dfTrans, ff, True)
        ff = f[:-5] + '_Status.pklz'
        DumpPickleFile(dfStatus, ff, True)

def log2dataframeByVHL(fPath, site, dateTime, lst_vhl):
    for l in lst_vhl:
        print('VHL: '+l)
        fp = fPath + r'/' + site + r'/'+l+r'/MCC' + dateTime + '.log'
        files = glob.glob(fp)
        for f, cnt in zip(files,range(len(files))):
            print(str(cnt+1) + ' / ' + str(len(files)))
            # 1. read log
            df = pd.read_csv (f, names = ['C1','C2', 'C3', 'C4', 'C5', 'C6', 'C7', 'C8', 'C9', 'C10', \
                                         'C11', 'C12', 'C13', 'C14', 'C15', 'C16','C17','C18','C19','C20',\
                                         'C21', 'C22', 'C23', 'C24', 'C25', 'C26','C27','C28','C29','C30',\
                                         'C31', 'C32', 'C33', 'C34', 'C35', 'C36','C37','C38','C39','C40',\
                                         'C41', 'C42', 'C43', 'C44', 'C45', 'C46','C47','C48','C49','C50',\
                                         'C51', 'C52', 'C53', 'C54', 'C55', 'C56','C57','C58','C59','C60'])
            
            # 2. compress(zip) & delete log
            z = zipfile.ZipFile(f + r'.zip', 'w')
            z.write(f, f, compress_type = zipfile.ZIP_DEFLATED )       
            
            os.remove(f)
            
            # 2-1. File Name?ì ??????ì¶ì¶?ì¬ timeê³?ë³í©
            strDate = f[-14:-10] + '-' + f[-10:-8] + '-' + f[-8:-6] #yyyyMMdd
            dfBuf =df['C1']
        
            lst = []
            for strTime in dfBuf:
                strDateTime = strDate + ' ' + strTime[1:13]
                lst.append(strDateTime)   
        
            df['C1'] = lst  # ê²½ê³  ë°ì?ë.. ?¼ë¨ ë¬´ì        
            
            
            # 3. Type(Drive, Trans, Status) ë³?Parsing
            dfDrv = df[df['C3'] == 'DRV']
            dfDrv = parseDriveType(dfDrv)        
            dfTrans = df[df['C3'] == 'TRANS']
            dfTrans = parseTransType(dfTrans)
            dfStatus = df[df['C3'] == 'STS']
            dfStatus = parseStatusType(dfStatus)
            
            # 4. ?´ë ê²????ì±
            if(type(df.loc[0]['C2']) == type('machine')):
                machieid=df.loc[0]['C2']
            else:
                machieid=df.loc[1]['C2']
                
            f = r'D:/DiagMCCData/' + site + r'/Data/' + machieid + r'/'
            if not os.path.isdir(f):
                os.mkdir(f)
            
            # 5. Pickle ????- ?ê° ?¨ì¶ ?ì ??1ê°??? (?ì²´ or ê°?????
            # 2ê°??´ì Type ?°ê³ ë¶ì ?ì§ ?ì¼ë¯?ë¡?. ?°ì  Type ë³?êµ¬ë¶?´ëê³??¬ì©?ë¤.  
            f= f + machieid + '_' + dateTime + '.pklz'
            #DumpPickleFile(df, f, True)
            ff = f[:-5] + '_Drive.pklz'
            DumpPickleFile(dfDrv, ff, True)
            ff = f[:-5] + '_Trans.pklz'
            DumpPickleFile(dfTrans, ff, True)
            ff = f[:-5] + '_Status.pklz'
            DumpPickleFile(dfStatus, ff, True)

# Loading Drive pickles - Site ??ëª¨ë  VHL ?´ë¹ ?ê° Pickle ë³?Drive Type Merge
def loadPickleDriveByDataTime(fPath, site, dateTime):
    fp = fPath + r'/' + site + r'/Data/V*/V?????_' + dateTime + '_Drive.pklz'
    print(fp)
    files = glob.glob(fp)
    df_all = pd.DataFrame()
    for f, cnt in zip(files,range(len(files))):
        print(str(cnt+1) + ' / ' + str(len(files)))
        # 1. Load & Append Pickles
        df = LoadPickleFile(f, True)
        df_all = df_all.append(df, ignore_index=True)
        
    return df_all

def loadPickleDriveByDataTimenVHL(fPath, site, dateTime, lst_vhl):
    df_all = pd.DataFrame()
    for l in lst_vhl:
        fp = fPath + r'/' + site + r'/Data/'+l+'/'+l+'_' + dateTime + '_Drive.pklz'
        files = glob.glob(fp)        
        for f, cnt in zip(files,range(len(files))):
            #print(str(cnt+1) + ' / ' + str(len(files)))
            # 1. Load & Append Pickles
            df = LoadPickleFile(f, True)
            df_all = df_all.append(df, ignore_index=True)
        
    return df_all

# Loading Trans pickles - Site ??ëª¨ë  VHL ?´ë¹ ?ê° Pickle ë³?Trans Type Merge
def loadPickleTransByDataTime(fPath, site, dateTime):
    fp = fPath + r'/' + site + r'/Data/V*/V?????_' + dateTime + '_Trans.pklz'
    files = glob.glob(fp)
    df_all = pd.DataFrame()
    for f, cnt in zip(files,range(len(files))):
        print(str(cnt+1) + ' / ' + str(len(files)))
        # 1. Load & Append Pickles
        df = LoadPickleFile(f, True)
        df_all = df_all.append(df, ignore_index=True)
        
    return df_all

def loadPickleTransByDataTimenVHL(fPath, site, dateTime, lst_vhl):
    df_all = pd.DataFrame()
    for l in lst_vhl:
        fp = fPath + r'/' + site + r'/Data/'+l+'/V?????_' + dateTime + '_Trans.pklz'
        files = glob.glob(fp)
        for f, cnt in zip(files,range(len(files))):
            #print(str(cnt+1) + ' / ' + str(len(files)))
            # 1. Load & Append Pickles
            df = LoadPickleFile(f, True)
            df_all = df_all.append(df, ignore_index=True)
            
    return df_all

# Loading Status pickles - Site ??ëª¨ë  VHL ?´ë¹ ?ê° Pickle ë³?Status Type Merge
def loadPickleStatusByDataTime(fPath, site, dateTime):
    fp = fPath + r'/' + site + r'/Data/V*/V?????_' + dateTime + '_Status.pklz'
    files = glob.glob(fp)
    df_all = pd.DataFrame()
    for f, cnt in zip(files,range(len(files))):
        print(str(cnt+1) + ' / ' + str(len(files)))
        # 1. Load & Append Pickles
        df = LoadPickleFile(f, True)
        df_all = df_all.append(df, ignore_index=True)
        
    return df_all

def loadPickleStatusByDataTimenVHL(fPath, site, dateTime, lst_vhl):
    df_all = pd.DataFrame()
    for l in lst_vhl:
        fp = fPath + r'/' + site + r'/Data/'+l+'/V?????_' + dateTime + '_Status.pklz'
        files = glob.glob(fp)        
        for f, cnt in zip(files,range(len(files))):
            #print(str(cnt+1) + ' / ' + str(len(files)))
            # 1. Load & Append Pickles
            df = LoadPickleFile(f, True)
            df_all = df_all.append(df, ignore_index=True)
        
    return df_all

# Save Text
# file_name - except extension(.xlsx)
def save_text(file_path, file_name, df, lst_column):
    df[lst_column].to_csv(file_path+file_name+'.csv.gz', compression='gzip')

# Save Excel
# file_name - except extension(.xlsx)
def save_excel(file_path, file_name, df, lst_column, sheet):
    with pd.ExcelWriter(file_path+file_name+'.xlsx') as writer:
        df[lst_column].to_excel(writer, sheet_name=sheet)

# Excel Save
# file_name - except extension(.xlsx)
def save_rawData(file_path, file_name, df, lst_column, sheet):
    start=time.perf_counter()    
    if(len(df) > 500000):        
        print('Save CSV Start: '+file_name)
        save_text(file_path, file_name, df, lst_column)                
    else:
        print('Save Excel Start: '+file_name)
        save_excel(file_path, file_name, df, lst_column, sheet)
        
    print('Save Time: '+format(time.perf_counter()-start,'.2f'))
        

def my_plot_all(title, df,plot_xy, fig_size, scale_font, limit_y, ref_y, is_save,filename):
    # size & title
    plt.figure(figsize=fig_size)
    plt.title(title)
  
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
        print(title + 'abnormal data is None..!!')
        plt.close()


def my_plot(title, df,plot_xy, fig_size, scale_font, limit_y, ref_y, is_save,filename, filteruse):
    if(filteruse == True):
        my_plot_filtered(title, df,plot_xy, fig_size, scale_font, limit_y, ref_y, is_save,filename)
    else:        
        my_plot_all(title, df,plot_xy, fig_size, scale_font, limit_y, ref_y, is_save,filename)

    
if __name__ == '__main__': 
    print('MyUtil.py excuted..!!')
    

