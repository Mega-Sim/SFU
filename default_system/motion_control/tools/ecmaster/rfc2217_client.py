# -*- coding:utf-8 -*-

import sys, os, select
from socket import *

#port number of launcher - sync with launch.Manager.LauncherPortNum
RF2217_ECAT_SLAVE_PORT = 43000

def PrintUsage():
    print 'Usage: %s <slave id>' % sys.argv[0]
    sys.exit(1)


# 0. argument check
if len(sys.argv) != 2: PrintUsage()
slaveid = int(sys.argv[1])
#1. create socket
sock = socket(AF_INET, SOCK_STREAM)

# 2. connect
try:
    print "Slave ID:", slaveid
    print "Port num:", RF2217_ECAT_SLAVE_PORT + slaveid
    sock.connect(("localhost", RF2217_ECAT_SLAVE_PORT+slaveid))

    #3. loop
    while True:
        i, o, e = select.select([sys.stdin, sock], [], [], 1)
        if sock in i : # 서버로부터 출력이 오면 이를 print out한다.
            output = sock.recv(4096)
            if len(output) > 0:
                print output,
                if output == '%EXIT%': break # '%EXIT%'은 session 종료를 confirm 받는 특수 메시지.
        elif sys.stdin in i: # socket 메시지가 있을 때는 메시지를 다 찍고 입력을 받는다.
            input = sys.stdin.readline().strip()
            sock.sendall(input)
        else: # time out... send kepp-alive packet
            #sock.sendall('keep-alive')
            pass
finally:
    sock.close()
print '>>> RFC2217 test client 종료! <<<'
