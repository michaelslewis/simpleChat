/*******************************************************************************
* Author: Michael S. Lewis													   *
* CS 372 Winter 2018														   *
* Programming Assignment #1													   *
* Description: chatclient.c implements a chat client for a simple chat system. *
********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/* Preprocessor directives (to expand with constants). */
#define MAX_HANDLE 10	// Maximum number of characters allowed for user's handle.
#define MAX_MESSAGE 500	// Maximum number of characters allowed per message.

/* Function declarations. */
void getHandle(char* userInput);
struct addrinfo* createAddrInfo(char* input_addr, char* port);
int createSocket(struct addrinfo* res);
void connectSocket(int sockfd, struct addrinfo* res);
void chat(int sockfd, char* clientName, char* serverName);
void exchangeNameData(int sockfd, char* clientName, char* serverName);


/*******************************************************************************
 * To run main, use the following usage: chatclient <server_IP> <server_Port>
 * Example: chatclient flip1.engr.oregonstate.edu 30020
 *******************************************************************************/
int main(int argc, char* argv[])
{
	char clientName[MAX_HANDLE];	// Stores handle from client user.
	char serverName[MAX_HANDLE];	// Stores handle from server user.

	/* Gives proper usage and exits if user enters invalid number of arguments. */
	if (argc != 3)
	{
		fprintf(stderr, "Invalid number of arguments\n"
			"Proper usage: chatclient <server_IP> <server_Port>\n"
			"Example: chatclient flip1.engr.oregonstate.edu 30020\n");
		exit(1);
	}

	/* Scans standard input for client's username. */
	getHandle(clientName);

	/* Uses input arguments to create a struct containing address and port. */
	struct addrinfo* res = createAddrInfo(argv[1], argv[2]);

	/* Creates a new socket using the result from addrinfo. */
	int sockfd = createSocket(res);

	/* Connects the socket passed in using the result from addrinfo. */
	connectSocket(sockfd, res);

	/* Exchanges usernames for each user with server. */
	exchangeNameData(sockfd, clientName, serverName);

	/* Initiates chat with the server. */
	chat(sockfd, clientName, serverName);

	/* Frees up memory to avoid leaking -- references Beej's Guide Section 9.5
	 * http://beej.us/guide/bgnet/html/single/bgnet.html#getaddrinfoman */
	freeaddrinfo(res);
}


/*******************************************************************************
 * Name: void getHandle(char* userInput)
 * Description: Scans standard input for user's name.
 * Arguments: An array of chars, of maximum "MAX_HANDLE" chars.
 *******************************************************************************/
void getHandle(char* userInput)
{
	printf("Please enter a username of up to 10 characters: ");
	scanf("%s", userInput);	// Scans standard input.
}


/*******************************************************************************
 * Name: struct addrinfo * createAddrInfo(char* input_addr, char* port)
 * Description: Creates a pointer to a linked list of addrinfo, with each
 * 				element containing a corresponding input address and port.
 * Arguments: A pointer to char representing the input address, and a pointer
 *			  to char representing the port.
 * Returns: A struct containing the corresponding address info.
 *
 * References Beej's Guide Section 5.1 getaddrinfo()  
 * http://beej.us/guide/bgnet/html/single/bgnet.html#getaddrinfo
 *******************************************************************************/
struct addrinfo* createAddrInfo(char* input_addr, char* port)
{
	/* Creates status indicator and structures for address info. */
	int status;	// Stores result returned from getaddrinfo()
	struct addrinfo hints;	// Provides hints to addrinfo struct.
	struct addrinfo *res;	// Provides pointer to linked list.

	/* Resets the struct by clearing all hints and configures IP version. */
	memset(&hints, 0, sizeof hints);	// Clears out hints.
	hints.ai_family = AF_INET;			// Designates address type as IPv4.
	hints.ai_socktype = SOCK_STREAM;	// Uses TCP stream sockets.

	/* Attemtps to loads up struct and detects if there is an error. */
	if ((status = getaddrinfo(input_addr, port, &hints, &res)) != 0)
	{
		fprintf(stderr,
				"getaddrinfo error: %s\nPlease verify correct address and port are used.\n",
				gai_strerror(status));
		exit(1);
	}
	return res;	// Returns struct addrinfo.
}


/*******************************************************************************
 * Name: int createSocket(struct addrinfo* res)
 * Description: Creates a socket from addrinfo.
 * Arguments: A struct containing address info.
 * Returns: An int representing a socket file descriptor.
 *
 * References Beej's Guide Section 9.23 socket()  
 * http://beej.us/guide/bgnet/html/single/bgnet.html#socketman
 *******************************************************************************/
int createSocket(struct addrinfo* res)
{
	int sockfd;	// Stores result returned from socket().

	/* Creates socket. Prints error and exits if result returned == -1. */
	if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
	{
		fprintf(stderr, "Error: socket creation failed.\n");
		exit(1);
	}
	return sockfd;
}


/*******************************************************************************
 * Name: void connectSocket(int sockfd, struct addrinfo* res)
 * Description: Connects the socket to the address specified in the address
 *				info struct.
 * Arguments: An int representing a socket file descriptor and a struct
 *			  containing address info.
 *
 * References Beej's Guide Section 9.23 socket()  
 * http://beej.us/guide/bgnet/html/single/bgnet.html#socketman
 *******************************************************************************/
void connectSocket(int sockfd, struct addrinfo * res)
{
	int status;	// Stores result returned from connect().

	/* Connects socket. Prints error and exits if result returned == -1. */
	if ((status = connect(sockfd, res->ai_addr, res->ai_addrlen)) == -1)
	{
		fprintf(stderr, "Error: socket connection failed.\n");
		exit(1);
	}
}


/*******************************************************************************
 * Name: void chat(int sockfd, char* clientName, char* serverName)
 * Description: Initiates a chat session through the new socket connection. The
 *				serverclient must be running first. First message will be sent
 *				by the chatclient.
 * Arguments: An int representing a socket file descriptor, a string for the
 *			  client's username, and a string for the server's username.
 *
 * References Beej's Guide Section 6.2 "A Simple Stream Client"
 * http://beej.us/guide/bgnet/html/single/bgnet.html#simpleclient
 *******************************************************************************/
void chat(int sockfd, char* clientName, char* serverName)
{

	int byteCounter = 0;	// Counts number of bytes sent.
	int status;				// Flag set if error detected in message by recv().
	char inputBuffer[MAX_MESSAGE + 3];	// Buffer for input message received.
	char outputBuffer[MAX_MESSAGE + 1];	// Buffer for output message to be sent.

	memset(inputBuffer, 0, sizeof(inputBuffer));	// Clears out input buffer.
	memset(outputBuffer, 0, sizeof(outputBuffer));	// Clears out output buffer.

	fgets(inputBuffer, MAX_MESSAGE, stdin);	// Clears out stdin.

	while(1)
	{
		/* Prints client's username as prompt for message to be sent. */
		printf("%s> ", clientName);

		/* Reads up to "MAX_MESSAGE" characters from stdin into inputBuffer. */
		fgets(inputBuffer, MAX_MESSAGE + 2, stdin);

		/* Closes the connection with the command "\quit". */
		if (strcmp(inputBuffer, "\\quit\n") == 0)
		{
			break;
		}

		/* Sends message to chatserve. Stores return value as byteCounter. */
		byteCounter = send(sockfd, inputBuffer, strlen(inputBuffer) , 0);

		/* Prints error message and exits if send failed. */
		if (byteCounter == -1)
		{
				fprintf(stderr, "Error: message send failed.\n");
				exit(1);
		}

		/* Receives message from chatserve. Stores return value as status. */
		status = recv(sockfd, outputBuffer, MAX_MESSAGE, 0);

		/* Prints error message and exits if receive failed. */
		if (status == -1)
		{
			fprintf(stderr, "Error: message receive failed.\n");
			exit(1);
		}
		else if (status == 0)	// Prints message and closes if server closes connection.
		{
			printf("The server closed the connection.\n");
			break;
		}
		else	// Prints message received from server.
		{
			printf("%s> %s\n", serverName, outputBuffer);
		}

		/* Clears out the input and output buffers and flushes stdin before next exchange. */
		memset(inputBuffer, 0, sizeof(inputBuffer));
		memset(outputBuffer, 0, sizeof(outputBuffer));
	}

	/* Closes the connection. */
	close(sockfd);
	printf("Connection closed.\n");
}


/*******************************************************************************
 * Name: void exchangeNameData(int sockfd, char* clientName, char* serverName)
 * Description: Initiates an exchange of information between the client and the
 *				server. Allows the server to have the client's name and for the
 *				client to have the server's name.
 * Arguments: An int representing a socket file descriptor, a string for the
 *			  client's username, and a string for the server's username.
 *******************************************************************************/
void exchangeNameData(int sockfd, char* clientName, char* serverName)
{
	/* Provides the client's username for this session to send to the server. */
	int sendingcode = send(sockfd, clientName, strlen(clientName), 0);
	/* Accepts the server's username for this session. */
	int receivingcode = recv(sockfd, serverName, MAX_HANDLE, 0);
}