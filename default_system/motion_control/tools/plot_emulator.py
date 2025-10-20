#!/usr/bin/env python
#-*- coding: utf-8 -*-

import threading
import SocketServer
from Queue import Queue, Empty
from struct import unpack
from time import sleep
from datetime import datetime
import matplotlib.pyplot as plt


AXIS_COUNT = 4
DATA_BYTES = 4
DATA_SIZE = DATA_BYTES*AXIS_COUNT
PLOT_COUNT = 500
dataQueue = Queue()


def updatePlot():
    plt.ion()
    fig = plt.figure()

    ax = [
        fig.add_subplot(411),
        fig.add_subplot(412),
        fig.add_subplot(413),
        fig.add_subplot(414),
    ]
    #plt.show()

    global  dataQueue
    pulse_per_rot_ = 1 << 20
    gear_ratio_ = 1.0
    veldata = [ [0 for i in range(PLOT_COUNT)] for j in range(AXIS_COUNT) ]
    hnd = [None for i in range(AXIS_COUNT)]
    for i in range(AXIS_COUNT):
        hnd[i], = ax[i].plot(range(PLOT_COUNT), veldata[i])
        ax[i].set_ylim(-4000, 4000)
        ax[i].grid()
    plt.draw()

    while True:
        try:
            receved_count = 0
            while True:
                try:
                    item = dataQueue.get_nowait()
                except:
                    break;
                receved_count += 1
                vels = unpack('iiii', item)
                vels = map(lambda pulse : 60.0 * pulse / (pulse_per_rot_ * gear_ratio_), vels)
                for i in range(AXIS_COUNT):
                    veldata[i] = veldata[i][1:] + [vels[i]]
                dataQueue.task_done()
            if receved_count > 0 :
                for i in range(AXIS_COUNT):
                    hnd[i].set_ydata(veldata[i])
                plt.pause(0.01)
                #print datetime.now(), "plt"
            else :
                plt.pause(0.01)
        except Empty:
            pass
        except KeyboardInterrupt:
            print ('Ctrl-C!')
            break

class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):
    def handle(self):
        global dataQueue
        cur_thread = threading.current_thread()
        print (cur_thread, 'Begin session from', self.client_address[0],  self.client_address[1])

        while True:
            received_len = 0
            data = bytearray()
            try:
                while received_len < DATA_SIZE :
                    msg = self.request.recv(DATA_SIZE-received_len)
                    if not msg  :
                        print (cur_thread, 'End session!', self.client_address[0],  self.client_address[1])
                        return
                    received_len += len(msg)
                    data += msg
            except:
                print ('Error', cur_thread)
                break
            dataQueue.put_nowait(data)


class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass


if __name__ == "__main__":
    # Port 0 means to select an arbitrary unused port
    HOST, PORT = "localhost", 11112

    server = ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler)
    ip, port = server.server_address

    try:

        server_thread = threading.Thread(target=server.serve_forever)

        # Exit the server thread when the main thread terminates
        server_thread.daemon = True
        server_thread.start()
        print ("Server loop running in thread:", server_thread.name)

        # plot
        updatePlot()


    finally:
        server.shutdown()
        server.server_close()
