#!/bin/python
'''
Author: Michael S. Lewis
CS 372 Winter 2018
Programming Assignment #1
Description: chatserve.py implements a chat server for a simple chat system.
'''

import sys
from socket import *

MAX_HANDLE = 10     # Maximum number of characters allowed for user's handle.
MAX_MESSAGE = 500   # Maximum number of characters allowed per message.

'''
Name: chat(socketConnection, clientName, serverName)
Description: Initiates a chat session through the new socket connection. The
             serverclient must be running first. First message will be sent
             by the chatclient.
'''
def chat(socketConnection, clientName, serverName):
    buffer = ""
    while 1:    # Keeps chat open until client or server closes connection.
        status = socketConnection.recv(MAX_MESSAGE + 1)[0:-1]   # Receives message from client.
        if status == "":    # Waits for new connection if no message was received.
            print "Connection closed."
            break
        # Prints prompt of client's handle followed by received message.
        print "{}> {}".format(clientName, status)   # Formats line with prompt and message.
        '''        
        Prepares to send server's message.
        '''
        buffer = "" # Clears out buffer for new input.
        while (len(buffer) == 0 or len(buffer) > MAX_MESSAGE):    # Ensures input is within spec.
            buffer = raw_input("{}> ".format(serverName))
        '''
        Sends message to client. Exits upon "\quit".
        '''
        if buffer == "\quit":
            print "Connection closed."
            socketConnection.close()    # Closes the connection.
            exit(0)                     # Exits the chatserve program.
        socketConnection.send(buffer)   # Else, sends the message in the buffer to the client.


'''
Name: exchangeNameData(socketConnection, serverName)
Description: Initiates an exchange of information between the client and the
             server. Allows the server to have the client's name and for the
             client to have the server's name.
'''
def exchangeNameData(socketConnection, serverName):
    clientName = socketConnection.recv(1024) # Accepts the client's username for this session.
    socketConnection.send(serverName)   # Provides the server's username for this session to send to the client.
    return clientName


'''
Name: main()
Description: Ensures proper number of arguments are accepted. Creates connection
             and initiates chat by calling chat() and exchangeNameData().
'''
def main():
    serverName = "" 
    if len(sys.argv) != 2:  # Exits upon incorrect number of arguments or improper usage.
        print "Invalid number of arguments."
        print "Proper usage: chatclient <server_IP> <server_Port>"
        print "Example: python chatserve.py 30020"
        exit(1)
    serverport = sys.argv[1]    # Retrieves port number from argument 1.
    serversocket = socket(AF_INET, SOCK_STREAM) # Creates a TCP socket.
    serversocket.bind(('', int(serverport)))    # Binds TCP socket to the specified port.
    serversocket.listen(1)  # Listens for incoming message on specified port.

    while len(serverName) == 0 or len(serverName) > MAX_HANDLE: # Accepts user's name from raw_input.
        serverName = raw_input("Please enter a user name of up to 10 characters: ")
        if (len(serverName) == 0 or len(serverName) > MAX_HANDLE):   # Notifies user of proper handle length.
            print "User name cannot be greater than 10 characters."

    print "The server is ready to receive a connection from the client."

    while 1:    # Creates new socket upon incoming connection and maintains until closed.
        socketConnection, address = serversocket.accept()   # Accepts socket connection.
        print "Received new connection on address {}".format(address)   # Prints confirmation of connection.
        '''
        Initiates chat and exchanges username data with client.
        '''
        chat(socketConnection, exchangeNameData(socketConnection, serverName), serverName)
        socketConnection.close()
        exit(0) # Exits the chatserve program.


if __name__ == "__main__":
    main()