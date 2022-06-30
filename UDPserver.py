from ast import With
import socket

 

localIP     = "192.168.137.1"

localPort   = 10010

bufferSize  = 1024

fileName = "skateData.txt"

 

msgFromServer       = "Hello UDP Client"

bytesToSend         = str.encode(msgFromServer)

 

# Create a datagram socket

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

 

# Bind to address and ip

UDPServerSocket.bind((localIP, localPort))

 

print("UDP server up and listening")

 

# Listen for incoming datagrams

while(True):

    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)

    message = bytesAddressPair[0]

    address = bytesAddressPair[1]

    clientMsg = "Message from Client:{}".format(message)
    #clientIP  = "Client IP Address:{}".format(address)
    
    print(clientMsg)
    #print(clientIP)
    with open(fileName, mode='a+') as f :
        f.write(clientMsg+'\n') 

   

    # Sending a reply to client

    UDPServerSocket.sendto(bytesToSend, address)