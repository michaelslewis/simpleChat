/*******************************************************************************
* Author: Michael S. Lewis                                                     *
* CS 372 Winter 2018														   *
* Programming Assignment #1													   *
********************************************************************************/

To run chatserve.py:

First please make sure to give chatserve.py executable permissions:
chmod +x chatserve.py

To run:
Usage:	 python chatserve.py <server_Port>
Example: python chatserve.py 30020

--------------------------------------------------------------------------------

To compile and run chatclient.c:

To Compile:
Type "make" without quotes.

OR 

Type "gcc chatclient chatclient.c" without quotes.

To run:
Usage:	 chatclient <server_IP> <server_Port>
Example: chatclient flip1.engr.oregonstate.edu 30020

--------------------------------------------------------------------------------

Notes:

This program has been tested on flip1.engr.oregonstate.edu

The connection must be started on chatserve.

The first message must be sent on chatclient.

To exit either the client or server, type "\quit" without quotes.

Please note that if the connection is closed by server, flip may take several
seconds to allow for a new connection on the same port that was just used.