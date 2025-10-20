#!/usr/bin/env python2
#-*- coding: utf-8 -*-

import socket
import threading
import SocketServer
import time
from time import strftime, localtime
from Queue import Queue, Empty
from select import select
from struct import unpack
import sys


verbose = False # print logs to console if enabled

if '-v' in sys.argv:
    verbose = True

def getLogFileName():
    return strftime('%Y%m%d%H%M%S.log', localtime())


logQ = Queue()

def writeLog():
    global  dataQueue
    print (strftime('start : %Y%m%d%H%M%S', localtime()))
    logFileName = getLogFileName()
    logFile = open(logFileName, 'w')
    while True:
        try:
            item = logQ.get(True, 0.1)
            logFile.write(item)
            logQ.task_done()
            logFile.flush()
            if verbose:
                print item,
                sys.stdout.flush()
        except Empty:
            pass
        except KeyboardInterrupt:
            print ('Ctrl-C!')
            break

    logFile.close()
    print ('log end. See', logFileName)


class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):
    def handle(self):
        global dataQueue
        msg = ''
        cur_thread = threading.current_thread()
        print (cur_thread, 'Begin session from', self.client_address[0],  self.client_address[1])

        while True:
            try:
                log = self.request.recv(1024)
                if not log  :
                    print (cur_thread, 'End session!', self.client_address[0],  self.client_address[1])
                    break
                msg += log
            except:
                print ('Error', cur_thread)
                break
            if msg[:4] == '%%<<': msg = msg[4:] # remove prefix
            msgs = msg.split('%%<<')
            if msg[-4:] == '%%>>':
                msg = ''
            else:
                msg = msgs[-1]
                msgs = msgs[:-1]
            for m in msgs:
                logQ.put_nowait(m[:-4])


class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass


if __name__ == "__main__":
    # Port 0 means to select an arbitrary unused port
    HOST, PORT = "localhost", 1111    

    server = ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler)
    ip, port = server.server_address

    try:

        server_thread = threading.Thread(target=server.serve_forever)

        # Exit the server thread when the main thread terminates
        server_thread.daemon = True
        server_thread.start()
        print ("Server loop running in thread:", server_thread.name)

        # write
        writeLog()


    finally:
        server.shutdown()
        server.server_close()
