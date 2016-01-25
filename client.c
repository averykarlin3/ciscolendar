#include "client.h"

#define DTS sizeof(int) * 8

void error(int r) {
/*
 *Parameters: int r - The return value of a function that returns -1 if an error
 *Outputs: NA
 *Function: Takes a return value of a function, and prints an error message if it returns -1
*/
	if(r < 0) {
		printf("Error: %s\n", strerror(errno));
	} 
}

int sock() {
/*
 *Parameters: NA
 *Outputs: int id - The file descriptor of the socket to the server
 *Function: Creates a socket to a server
*/	
 	int i;
	int id = socket(AF_INET, SOCK_STREAM, 0);
	error(id);
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(1701);
	inet_aton("127.0.0.1", &(serv.sin_addr));
	i = connect(id, (struct sockaddr*) &serv, sizeof(serv));
	printf("<client> connect returned: %d\n", i);
	return id;
}

void timeUp(int data) {
/*Inputs - int data (File descriptor of the data file)
 *Function - Updates the data file time at the top of the file, with the current time
*/
	time_t now = time(NULL);
	char nows[DTS];
	sprintf(nows, "%li", (long) now);
	int test = write(data, nows, sizeof(nows));
	error(test);
	test = lseek(data, 0, SEEK_SET);
	error(test);
}

int openData(char* user, int flags) {
/*Inputs - char* user (Username), int flags (Flags used when opening the data file)
 *Outputs - int data (File descriptor of the data file)
 *Function - Opens the data file for the user, and returns the descriptor, creatin gthe file if it is not found
*/
	char* path = "~/.cal";
	struct stat buffer;
	int data;
	if(!stat(path, &buffer)) {
		data = open(path, flags , 0666);
		error(data);
	}
	else { //Account for new computer
		data = open(path, O_CREAT | flags, 0666);
		error(data);
		char* start = "0\n";
		write(data, start, sizeof(start));
	}
	return data;
}

int process(int socket, char* input, char* user) {
/*Inputs - int socket (socket file descriptor), char* input (The user input command) char* user (username)
 *Function - Reads any input from the user, and either sends it to the server to be processed, or processes it as necissary if there is no connection
 Return - int (0 if the client is ending, 1 otherwise)
*/
	if(!strcmp(input, "exit"))
		return 0;
	if(socket != -1) {
		printf("<C> %s\n", input); //Print command
		int test = write(socket, input, sizeof(input));
		error(test);
		if(!strcmp(input, "save"))
			confirmData(user, socket);
		char buffer[100];
		test = read(socket, buffer, sizeof(buffer));
		error(test);
		printf("<S> %s\n", buffer); //Print returned
		return 1;
	}
	else {
		int data = openData(user, O_RDWR);
		//Basic Client Processing (Including changing file)
		timeUp(data);
		return 1;
	}
}

void confirmData(char* user, int socket) {
/*Input - int socket (Socket file descriptor), char* user (Username)
 *Function - Checks if the data file for the user is more recently updated in the client or server, and copies the more recent to the server
*/
	int data = openData(user, O_RDWR | O_CREAT);
	char buffer[DTS];
	int test = read(data, buffer, DTS);
	error(test);
	printf("Test3\n");
	test = write(socket, buffer, DTS);
	error(test);
	char check[DTS];
	test = read(socket, check, DTS);
	error(test);
	if(atoi(check) > 0) { //More recent server file
		char ndata[atoi(check)];
		test = read(socket, ndata, atoi(check));
		error(test);
		close(data);
		int data = openData(user, O_RDWR | O_TRUNC);
		test = write(data, ndata, atoi(check));
		error(test);
	}
	else if(atoi(check) == -1){
		//More recent client file 
		struct stat d;
		stat(user, &d);
		char* size;
		sprintf(size, "%lli", (long long) d.st_size);
		test = write(socket, size, sizeof(size));
		error(test);
		test = lseek(data, 0, SEEK_SET);
		error(test);
		timeUp(data);
		char buffer[d.st_size];
		test = read(data, buffer, d.st_size);
		error(test);
		test = write(socket, buffer, d.st_size);
		error(test);
	}
	close(data);
}

int checkConnection() {
/*Output - Returns 0 if the internet is down, or some positive number otherwise
 *Function - Checks if the internet is connected
*/
	int google = socket(AF_INET, SOCK_STREAM, 0);
	error(google);
	struct sockaddr_in g;
	g.sin_family = AF_INET;
	g.sin_port = htons(80);
	inet_aton("https://www.google.com", &(g.sin_addr));
	int test = connect(google, (struct sockaddr*) &g, sizeof(g));
	close(google);
	return test;
}

int main() {
	//printf("Please enter the IP Address of the server you would like to connect to:");
	//Set user from here
	char* user;
	int conn = checkConnection();
	printf("Test1\n");
	int socket;
	if(conn) {
		socket = sock();
		confirmData(user, socket);
	}
	else {
		socket = -1;
	}
	int ret = 1;
	while(ret) {
		char input[100];
		printf("$ ");
		char* testa = fgets(input, sizeof(input), stdin);
		*strchr(input, '\n') = 0;
		if(testa == NULL) {
			error(-1);
		}
		ret = process(socket, input, user);
	}
	close(socket);
	return 0;
}
