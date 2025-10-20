#!/usr/bin/python

# Usage : target.sh [--ip <targetip>] [session]

import os, sys, string, time

class RunInfo:
    name = None
    session = None
    path = None
    args = None
    ip = None

if len(sys.argv) > 1 :    
    if sys.argv[1] == '--ip':
        RunInfo.ip = sys.argv[2]
        if len(sys.argv) > 3:
            RunInfo.session = sys.argv[3]
        else:
            RunInfo.session = ""
    else:
        RunInfo.session = sys.argv[1]    
else:
    RunInfo.session = ""


if RunInfo.ip == None:
    try:
        RunInfo.ip = os.environ['targetip']
    except:
        RunInfo.ip = "masterbd"
        print "Warning! 'targetip' is not set.\nSet environment variable like 'export targetip=xxx.xxx.xxx.x' or use --ip parameter.\nDefault ip =",RunInfo.ip        

script_path = os.path.split(os.popen('which target.sh').readline())[0]

os.system('scp.sh %s/remote.attach2screen root@%s:/root/.attach2screen master123' % (script_path, RunInfo.ip))    
time.sleep(0.1)
os.system('rattach.sh %s %s' % (RunInfo.ip, RunInfo.session))
