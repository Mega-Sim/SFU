# TCP server example
import socket
import signal, sys 
portid = int(sys.argv[1]) if len(sys.argv)>1 else 40015
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(("", portid))
server_socket.listen(5)
client_socket = None
print ("TCPServer Waiting for client on port ", portid)


def handleSignal(signal, frame):
	global server_socket, client_socket
	print("Ctrl-C")
	if client_socket != None:
		client_socket.close()
	server_socket.close()
	sys.exit(0)

signal.signal(signal.SIGINT, handleSignal)
	
while 1:
    client_socket, address = server_socket.accept()
    print ("I got a connection from ", address)
    received = b""
    while 1:
        data = client_socket.recv(512)
        if len(data) == 0 : break
        received += data
        if b'LON\r\n' in received:
            print(received)
            received = received[received.index(b'LON\r\n')+5:]
            client_socket.send(b'\x0212340\r\n')
            print("sent..1234")
        else:
            print(data)
    break;
    
server_socket.close()
print("SOCKET closed... END")

 
