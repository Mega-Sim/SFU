# TCP client example
import socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect(("localhost", 40015))
while 1:
    data = client_socket.recv(512).decode()
    if ( data == 'q' or data == 'Q'):
        client_socket.close()
        break;
    else:
        print ("RECEIVED:" , data)
        #data = input ( "SEND( TYPE q or Q to Quit):" )
        #if ( data == 'q' or data == 'Q'):
        #    client_socket.send(data.encode())
        #    client_socket.close()
        #    break;
        #else:
        #    client_socket.send(data.encode())
print ("socket colsed... END.")

