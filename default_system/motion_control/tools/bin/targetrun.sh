#!/usr/bin/python

import os, sys, string

# Usage : targetrun.sh [--ip <targetip>] exefile

class RunInfo:
    name = None
    session = None
    path = None
    args = None
    ip = None

if len(sys.argv) < 2:
    print "Usage : ./targetrun.sh [--ip <targetip>] <exefile>"
    sys.exit(-1)
    
if sys.argv[1] == '--ip':
    RunInfo.ip = sys.argv[2]
    RunInfo.name = sys.argv[3]
    RunInfo.args = string.join(sys.argv[4:])
else:
    RunInfo.name = sys.argv[1]
    RunInfo.args = string.join(sys.argv[2:])

if RunInfo.ip == None:
    try:
        RunInfo.ip = os.environ['targetip']
    except:
        RunInfo.ip = "masterbd"
        print "Warning! 'targetip' is not set.\nSet environment variable like 'export targetip=xxx.xxx.xxx.x' or use --ip parameter.\nDefault ip =",RunInfo.ip        

# get path
if not (RunInfo.name[0] == '/' or RunInfo.name[0] == '.'):
    RunInfo.name = '/root/' + RunInfo.name # default path 
RunInfo.path, RunInfo.name= os.path.split(RunInfo.name)

# set session
RunInfo.session = RunInfo.name
# set arg
if len(RunInfo.args) > 0 :
    RunInfo.args = "\"" + RunInfo.args + "\""
else:
    RunInfo.args = ""

script_path = os.path.split(os.popen('which target.sh').readline())[0]
    

os.system('scp.sh %s/remote.checkproc root@%s:/root/.checkproc master123' % (script_path, RunInfo.ip))
os.system('scp.sh %s/remote.attach2screen root@%s:/root/.attach2screen master123' % (script_path, RunInfo.ip))
#print 'rexec.sh %s %s %s %s %s' % (RunInfo.ip, RunInfo.session, RunInfo.path, RunInfo.name, RunInfo.args)
os.system('rexec.sh %s %s %s %s %s' % (RunInfo.ip, RunInfo.session, RunInfo.path, RunInfo.name, RunInfo.args))
