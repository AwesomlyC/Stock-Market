// Partner: Victor Chhun
// Student ID: 87506313
//
// Partner: Haver Ho
// Student ID: 54379591


// client.c
// Assignment 5

// Headers
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Define
#define MAX 80
#define MAXLEN 1024
// Global Variables
char* domain;
char* PORT;
char msg[MAX];
// Initalize Methods

// Define Methods
int open_clientfd(char* hostname, char* port){
	int clientfd;
	struct addrinfo hints, *listp, *p;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICSERV;
	hints.ai_flags |= AI_ADDRCONFIG;

	getaddrinfo(hostname, port, &hints, &listp);

	for (p = listp; p; p = p->ai_next){
		if ((clientfd = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) < 0)
				continue;

		int a;
		if ((a = connect(clientfd, p->ai_addr, p->ai_addrlen)) != -1){

			break;
		}

		close(clientfd);
	}
	freeaddrinfo(listp);
	if (!p){
		printf("All connections failed\n");
		return -1;
	}
	return clientfd;
}

// Main Function
int main(int argc, char *argv[]){
	int clientfd;
	char *host, *port;
	int rio;
	host = argv[1];
	PORT = argv[2];
	clientfd = open_clientfd(host, PORT);
	
	char buf[MAX];
	char msg[MAXLEN];
	if (clientfd != -1){

		printf("> ");
		while (strncmp(fgets(buf, MAX, stdin), "quit", 4)){
			sprintf(msg, "%c", (char)strlen(buf));
			strcat(msg, buf);
			send(clientfd, msg, strlen(msg), 0);
			int l = read(clientfd, msg, MAX);
			strncpy(buf, msg + 1, msg[0]);
			buf[l-1] = '\0';				// Set Terminating character
			printf("%s", buf);
			printf("> ");


		}
	}
	close(clientfd);
	exit(0);


}
