# Stock-Market

# Brief Description

A simple TCP/IP communication between Server and Client for information regarding stock prices using C programming language.

# How-to-run
1. To run, download server.c and client.c into a directory
2. Compile both server.c and client.c
3. Run server.c with two .csv files and a port number as arguments   ``` e.g. ./server PFE.csv MRNA.csv 30000 ```
4. Run client.c with a domain and a port number as arguments    ``` e.g. ./client domain.name 30000```
5. That's all!

# Detailed Description
****Implementation Details****
This program uses TCP/IP protocol to communicate between server and client. When the server begins, it will read the two csv files from the argument and store it into data structure to remove the need to re-read the csv files. Once completed, it will wait for a client to connect to its designated port number.

When a client connects to the server at the designated port number, the client will be able to send commands to the server, requesting for information about the stock prices from the given two csv files. The client can repeatedly request information until it decides to quit. When the client is request information, the server will display the client's input while the client will display the information receive from the server.

When the client quits, it will disconnect itself from the server and the server will wait for another client to connect. This process repeats indefinitely. 

# Commands
```PricesOnDate <date>```

```MaxPossible <profit|loss> <file> <start-date> <end-date>```

```quit```

# Additional Information about Commands

```PricesOnDate``` will display the stock prices of both stocks on a given date.

```MaxPossible``` will calculate the maximum possible profit/loss for a ****single share**** of a given stock in a given time span and display the result to the client.

```quit``` will disconnect the client from the server and return the user control to the Unix shell


