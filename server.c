// Partner: Victor Chhun
// Student ID: 87506313
//
// Partner: Haver Ho
// Student ID: 54379591


// server.c
// Assignment 5

// Headers
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
// Define
#define MAX 80
#define MAXLINE 1024
// Struct Class
// Contains information about the date and price
// to be used in PricesOnDate and MaxPossible
struct info {
	char date[MAX];
	double price;
};


// Global Variables
char* firstFile;
char* secondFile;
char* PORT = 0;
//char msg[MAX];
char formatDate[MAXLINE];
struct info file1Array[MAXLINE];
struct info file2Array[MAXLINE];
int file1Len = 0;
int file2Len = 0;

// Initalize Methods
void PricesOnDate(char* date, int connfd);

void MaxPossible(char* flag, char* name, char* startDate, char* endDate, int connfd);

void initializeArrays(char* file1_name, char* file2_name);

void formattedDate(char* givenDate);
// Define Methods
void initializeArrays(char* file1_name, char* file2_name){
	FILE *file1, *file2;
	char f1_row[MAXLINE], f2_row[MAXLINE];
	
	file1 = fopen(file1_name, "r");
	file2 = fopen(file2_name, "r");
	
	int file1Count = 0, file2Count = 0;

	int arrayI = 0;
	// For file1 (PFE.csv)
	while (fgets(f1_row, MAXLINE, file1) != NULL){
		// Retrieve the date for the corresponding row
		char* token = strtok(f1_row, ",");
		char* date = token;
		int i;

		// Skips several column to reach the desired column (closing)
		for (i = 0; i < 4; i++){
			token = strtok(NULL, ",");
		}

		double closing = atof(token);
		strcpy(file1Array[arrayI].date, date);
		file1Array[arrayI].price = closing;
		arrayI++;
		file1Len++;
	}

	arrayI = 0;

	// For file2 (MRNA.csv)
	while (fgets(f2_row, MAXLINE, file2) != NULL){
		// Retrieve the date for the corresponding row
		char* token = strtok(f2_row, ",");
		char* date = token;
		int i;	
		// Skips several column to reach the desired column (closing)
		for (i = 0; i < 4; i++){
			token = strtok(NULL, ",");
		}

		double closing = atof(token);
		strcpy(file2Array[arrayI].date, date);
		file2Array[arrayI].price = closing;
		arrayI++;
		file2Len++;
	}
}

// Re-formats the date given by client to match
// .csv files
// YYYY-MM-DD --> MM/DD/YYYY
void formattedDate(char* givenDate){
	char* delim = "-";
	char newDate[MAXLINE] = "";
	
	int y,m,d;
	char month[MAXLINE], day[MAXLINE];

	char* year = strtok(givenDate, delim);
	
	m = atoi(strtok(NULL, delim));
	d = atoi(strtok(NULL, delim));

	sprintf(month, "%d", m);
	sprintf(day, "%d", d);

	strcat(newDate, month);
	strcat(newDate, "/");
	strcat(newDate, day);
	strcat(newDate, "/");
	strcat(newDate, year);

	strcpy(formatDate, newDate);
}

// givenDate == YY-MM-DD
void PricesOnDate(char* givenDate, int connfd){

	FILE* file1, *file2;
	char str[MAXLINE];
	char phrase[MAX*2];
	char msg[MAXLINE] = "";
	char file1Name[MAX];
	char file2Name[MAX];
	int fileDatePresent1 = 0, fileDatePresent2 = 0;
	strcpy(file1Name, firstFile);
	strcpy(file2Name, secondFile);
	char* token1 = strtok(file1Name, ".");
	char* token2 = strtok(file2Name, ".");
	formattedDate(givenDate);
	for (int i = 0 ; i < file1Len; i++){
		if (strcmp(file1Array[i].date, formatDate) == 0){
			sprintf(phrase, "%s: %.2f |", token1, file1Array[i].price);
			fileDatePresent1 = 1;
			break;
		}
	}
	if (fileDatePresent1 == 0){
		sprintf(phrase, "%s: Unknown |", token1);
	}


	for (int i = 0; i < file2Len; i++){
		if (strcmp(file2Array[i].date, formatDate) == 0){
			sprintf(msg, "%s %s: %.2f\n", phrase, token2, file2Array[i].price);
			fileDatePresent2 = 1;
			break;
		}
	}
	if (fileDatePresent2 == 0){
		sprintf(msg, "%s %s: Unknown\n", phrase, token2);
	}
	char lenMSG[MAXLINE];
	sprintf(lenMSG, "%c", (char) strlen(msg));
	strcat(lenMSG, msg);
	write(connfd, lenMSG, strlen(lenMSG));
}

// Helper Function to find start index 
// for MaxPossible(...)
int startDateIndex( char* startDate, char* fileName){
	formattedDate(startDate);
	int startIndex;
	if (strcmp(fileName, firstFile) == 0){
		for (startIndex = 0; startIndex < file1Len &&
				strcmp(formatDate, file1Array[startIndex].date) != 0;
				startIndex++){
		}
	}
	else if (strcmp(fileName, secondFile) == 0){
		for (startIndex = 0; startIndex < file2Len &&
				strcmp(formatDate, file2Array[startIndex].date) != 0;
				startIndex++){
		}
	}
	return startIndex;
}
// Helper Function to find end index
// for MaxPossible(...)
int endDateIndex(char* endDate, char* fileName){
	formattedDate(endDate);
	int endIndex;
	if (strcmp(fileName, firstFile) == 0){
		for (endIndex = 0; endIndex < file1Len &&
				strcmp(formatDate, file1Array[endIndex].date) != 0; 
					endIndex++){
				}
	}
	else if (strcmp(fileName, secondFile) == 0){
		for (endIndex = 0; endIndex < file2Len &&
				strcmp(formatDate, file2Array[endIndex].date) != 0;
				endIndex++){
		}
	}
	return endIndex;
}

// Calculates the maximum profit/loss between two given dates (startDate, endDate)
// If flag == profit -> Buy Minimum / Sell Maximum
// If flag == loss --> Buy Maximum / Sell Minimum
void MaxPossible(char* flag, char* name, char* startDate, char* endDate, int connfd){

	char msg[MAX];
	int startIndex, endIndex;
	double difference = INT_MIN;
	// If file == PFE.csv
	if (strncmp(name, firstFile, strlen(name)) == 0){
		// Get the startIndex and endIndex for searching
		startIndex = startDateIndex(startDate, firstFile);
		endIndex = endDateIndex(endDate, firstFile);

		// If the startIndex/endIndex reaches the end of the file to indicate 
		// it did not find a corresponding start/end date in the csv files.
		// Or, in the case the startDate is greater than the endIndex
		// send "Unknown" to the client
		if (startIndex >= file1Len || endIndex >= file1Len || startIndex > endIndex){
			sprintf(msg, "Unknown\n");
			char lenMSG[MAXLINE];
			sprintf(lenMSG, "%c", (char) strlen(msg));
			strcat(lenMSG, msg);
			write(connfd, lenMSG, strlen(lenMSG));
			return;
		}
		int i,j;
		// Find the maximum profit in the given range of date
		if (strcmp(flag, "profit") == 0){
			for (i = startIndex; i<= endIndex; i++){
				for (j = i+1; j <= endIndex; j++){
					difference = difference > (file1Array[j].price - file1Array[i].price) 
						? difference : file1Array[j].price - file1Array[i].price;
				
				}
			}
		}
		// Find the maximum loss in the given range of date
		else{
			for(i = startIndex; i <= endIndex; i++){
				for (j = i + 1; j <= endIndex; j++){
					difference = difference > (file1Array[i].price - file1Array[j].price)
						? difference : file1Array[i].price - file1Array[j].price;
				}
			}
		}
	}
	// If file == MRNA.csv
	else if (strncmp(name, secondFile, strlen(name)) == 0){
		// Get the startIndex and endIndex for searching
		startIndex = startDateIndex(startDate, secondFile);
		endIndex = endDateIndex(endDate, secondFile);
		
		// If the startIndex/endIndex reaches the end of the file to indicate
		// it did not find a corresponding start/end date in the csv files.
		// Or, in the case the startDate si greater than the endIndex
		// send "Unknown" to the client
		if (startIndex >= file2Len || endIndex >= file2Len || startIndex > endIndex){
			sprintf(msg, "Unknown\n");
			char lenMSG[MAXLINE];
			sprintf(lenMSG, "%c", (char) strlen(msg));
			strcat(lenMSG, msg);

			write(connfd, lenMSG, strlen(lenMSG));
			return;
		}
		int i, j;
		// Find the maximum profit in the given range of date
		if (strcmp(flag, "profit") == 0){
			for (i = startIndex; i <= endIndex; i++){
				for (j = i + 1; j <= endIndex; j++){
					difference = difference > (file2Array[j].price - file2Array[i].price)
						? difference : file2Array[j].price - file2Array[i].price;
				}
			}
		}
		// Find the maximum loss in the given range of date
		else{
			for (i = startIndex; i <= endIndex; i++){
				for (j = i+1; j <= endIndex; j++){
				difference = difference > (file2Array[i].price - file2Array[j].price)
					? difference : file2Array[i].price - file2Array[j].price;
				}
			}
		}
	}
	if (difference == INT_MIN){
		difference = 0;
	}

	// Send the maximum profit/loss to the client
	sprintf(msg, "%.2f\n", difference);
	char lenMSG[MAXLINE];
	sprintf(lenMSG, "%c", (char)strlen(msg));
	strcat(lenMSG, msg);
	write(connfd, lenMSG, strlen(lenMSG));
}

// Listens for any incoming clients that are trying to connect
// to the port 
int open_listenfd(char *port){
	struct addrinfo hints, *listp, *p;
	int listenfd, optval = 1;

	memset(&hints, 0, sizeof(struct addrinfo));
	// Flags for addrinfo
	hints.ai_socktype = SOCK_STREAM;		// Open up a connection
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	hints.ai_flags |= AI_NUMERICSERV;
	getaddrinfo(NULL, port, &hints, &listp);

	// Search for possible connections 
	for (p = listp; p; p = p->ai_next){
		if ((listenfd = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) < 0)
			continue;

		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
				(const void *)&optval, sizeof(int));
		
		// Connection is successful with the client
		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
			break;
		// If all connections fail, try the next one
		close(listenfd);
	}
	// Free up the linked list
	freeaddrinfo(listp);
	// All connections have failed
	if (!p)
		return -1;
	if (listen(listenfd, 1) < 0) {
		close(listenfd);
		return -1;
	}
	// Return the found connection file descriptor
	return listenfd;
}

// Server
void echo(int connfd){
	size_t n;
	char buf[MAX];
	char msg[MAX];
	char invalidCMD[MAX] = "Invalid syntax\n";
	char invalid[MAX];
	sprintf(invalid, "%c", (char)strlen(invalidCMD));
	strcat(invalid, invalidCMD);
	char* delim =  " \n\t";
	while ((n = read(connfd, msg, MAX)) != 0){
		strncpy(buf, msg + 1, msg[0]);
		if (msg[0] > 2){				// Greater than 1 byte received e.g. "'\0'"
			buf[n-1] = '\0';				// Set Terminating Character
			printf("%s", buf);			// Print user command on server-side
			char* token = strtok(buf, delim);		// Get the first part of the command
			// PricesOnDate YYYY-MM-DD
			if (strcmp(token, "PricesOnDate") == 0){
				char* date = strtok(NULL, delim);
				PricesOnDate(date, connfd);
			}
			// MaxPossible flag file startDate endDate
			else if (strcmp(token, "MaxPossible") == 0){
				char* flag = strtok(NULL, delim);		// Flag - Profit/Loss
				char* file = strtok(NULL, delim); 		// Which File to use
				if (((strcmp(flag, "loss") == 0 || strcmp(flag, "profit") == 0)) && 
					(strcmp(file, "PFE") == 0 || strcmp(file, "MRNA") == 0)){
						
					char* startDate = strtok(NULL, delim);	// Starting - date
					char* endDate = strtok(NULL, delim); 	// Ending - date
					MaxPossible(flag, file, startDate, endDate, connfd);
				}
				else{
					write(connfd, invalid, strlen(invalid));
				}	
			}
			else{
				write(connfd, invalid, strlen(invalid));
			}
		}
		else{
			printf("\n");
			write(connfd, invalid, strlen(invalid));
		}
	}
}

// Main Function
int main(int argc, char* argv[]){
	// File Names
	firstFile = argv[1];		//	PFE.csv
	secondFile = argv[2];		//	MRNA.csv
	
	// Initialize the array for both firstFile and secondFile
	initializeArrays(firstFile, secondFile); 
	// Server variables
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAX], client_port[MAX];
	
	// Listens for any connections
	listenfd = open_listenfd(argv[3]);
	printf("server started\n");
	// Beginning of the server
	while (1){
		clientlen = sizeof(struct sockaddr_storage);
		connfd = accept(listenfd, (struct sockaddr*) &clientaddr,
				&clientlen);
		getnameinfo((struct sockaddr*)&clientaddr, clientlen,
				client_hostname, MAX, client_port, MAX, 0);
		echo(connfd);
		close(connfd);
	}
	exit(0);
}
