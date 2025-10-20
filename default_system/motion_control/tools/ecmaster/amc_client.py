# -*- coding:utf-8 -*-

import sys, os, select
from socket import *

DPRAM_SIZE = 1024
AMC_DPRAM_PORT = 9000

dpram = bytearray(DPRAM_SIZE)



#1. create socket
sock = socket(AF_INET, SOCK_STREAM)

# 2. connect
try:    
    print "Port num:", AMC_DPRAM_PORT
    sock.connect(("localhost", AMC_DPRAM_PORT))

    #3. loop
    while True:
        i, o, e = select.select([sys.stdin, sock], [], [], 1)
        if sock in i : # 서버로부터 출력이 오면 이를 print out한다.
            output = sock.recv(1024)
            if len(output) > 0:
                dpram[:len(output)] = output;
                print dpram,
                if output == '%EXIT%': break # '%EXIT%'은 session 종료를 confirm 받는 특수 메시지.
        elif sys.stdin in i: # socket 메시지가 있을 때는 메시지를 다 찍고 입력을 받는다.
            keyin = sys.stdin.readline().strip()
            if keyin.isdigit():
                dpram[1022] = int(keyin)
            else:
                dpram[:len(keyin)] = keyin
            sock.sendall(dpram)
        else: # time out... send kepp-alive packet
            #sock.sendall('keep-alive')
            pass
finally:
    sock.close()
print '>>> AMC Client test client 종료! <<<'
