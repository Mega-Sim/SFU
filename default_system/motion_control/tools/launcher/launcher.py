#!/usr/bin/env python3
import os, sys, signal, time, yaml
from subprocess import Popen, PIPE

pid_file_path = '/tmp/launcher.pid'
with open(pid_file_path, 'w') as pidfile:
    pidfile.write('%d\n' % os.getpid())

basedir = os.getcwd();

class App:
    def __init__(self, ses, path, args, workingdir, wait_after_run = 0, wait_wafter_kill = 0):
        self.session = ses
        self.path = path
        self.appname = os.path.split(path)[-1]
        self.args = args if args else ''
        self.wd = workingdir
        self.run_wait = wait_after_run
        self.term_wait = wait_wafter_kill
        print("App %s(%s) added" % (ses, path))
    
    def is_running(self):
        with Popen(['ps', '-ax'], stdout=PIPE) as proc:
            for line in proc.stdout.readlines():
                line = line.decode().strip()
                if self.path in line: return True
        return False

    def get_pid(self):
        with Popen(['ps', '-ax'], stdout=PIPE) as proc:
            for line in proc.stdout.readlines():
                line = line.decode().strip()
                if self.path in line:
                    words = line.split()
                    if self.path in words[4]: return words[0]
                    elif 'python' in words[4]: return words[0]

        return None

    def run(self):
        if self.is_running():
            print(' ', "%s is already running..." % self.appname)
            return
        app_path = self.path if self.wd[0] == '/' else basedir + '/' + self.path
        os.chdir(self.wd)
        cmd = 'screen -dmS %s %s %s' % (self.session, app_path, self.args)
        print(' ', cmd)        
        os.system(cmd)
        time.sleep(self.run_wait)
        os.chdir(basedir)

    def kill(self):
        pid = self.get_pid()
        if pid:
            cmd = "kill -15 " + pid
            print(' ', self.appname, ':', cmd)
            os.system(cmd)
            time.sleep(self.term_wait)
            if self.is_running():
                cmd = "kill -9 " + pid
                print(' ', self.appname, '(forced to terminate):', cmd)
                os.system(cmd)

        else:
            print(' ', self.appname, ': process not found' )

apps = []
if len(sys.argv) > 1:
    with open(sys.argv[1]) as config_file:
        conf = yaml.load(config_file.read())
        print(conf)
        for i,app in enumerate(conf):
            apps.append(App(app['name'], app['path'], app['args'], app['cwd'],
                            app['runwait'] if 'runwait' in app else 0,
                            app['termwait'] if 'termwait' in app else 0))


def terminate_processes():
    print('Terminating processes')
    for app in apps[::-1]:
        app.kill()

def run_processes():
    terminate_processes();
    time.sleep(0.5)
    print('Run processes')
    for app in apps:
        app.run()

def exit():
    global pid_file_path
    os.remove(pid_file_path)
    sys.exit(0)

def receive_signal(signum, stack):
    print('Received:', signum)
    if signum == signal.SIGUSR1 :
        terminate_processes()
    elif signum == signal.SIGUSR2 :
        run_processes()
    elif signum == signal.SIGINT:
        exit()

# Register signal handlers
signal.signal(signal.SIGUSR1, receive_signal) # SIGUSR1 = 10
signal.signal(signal.SIGUSR2, receive_signal) # SIGUSR2 = 12
signal.signal(signal.SIGINT, receive_signal) # SIGINT = 2

# run processes initially
run_processes()

i = 0
while True:
    print("%d Launcher pid: %d, to stop: 10, to launch: 12" % (i, os.getpid()))
    i += 1
    time.sleep(10)



