#include "client.h"

#define DTS 26

void error(int r) {
	if(r < 0) {
		printf("Error: %s\n", strerror(errno));
	}
}

int sock() {
	int id = socket(AF_INET, SOCK_STREAM, 0);
	error(id);
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(1701);
	inet_aton("65.78.20.209", &(serv.sin_addr));
	bind(id, (struct sockaddr*) &serv, sizeof(serv));
	connect(id, (struct sockaddr*) &serv, sizeof(serv));
	return id;
}

int openData(char* user, int flags) {
	char* path = "~/.cal";
	int data = open(path, flags , 0666);
	error(data);
	//Account for nonexistant
	return data;
}

int process(int socket, char* input) {
	if(!strcmp(input, "exit"))
		return 0;
	printf("<C> %s\n", input);
	int test = write(socket, input, sizeof(input));
	error(test);
	char buffer[100];
	test = read(socket, buffer, sizeof(buffer));
	error(test);
	printf("<S> %s\n", buffer);
	return 1;
}

void confirmData(char* user, int socket) {
	int data = openData(user, O_RDWR | O_CREAT);
	char buffer[DTS];
	int test = read(data, buffer, DTS);
	error(test);
	test = write(socket, buffer, DTS);
	error(test);
	char check[sizeof(int)];
	test = read(socket, check, sizeof(int));
	error(test);
	char ndata[(int) check];
	if(check > 0) {
		test = read(socket, ndata, check);
		error(test);
		close(data);
		int data = openData(user, O_RDWR | O_TRUNC);
		write(data, ndata, check);
	}
	else if(check == -1){
		//More recent client file 
	}
}

int main() {
	//Set user
	char* user;
	int socket = sock();
	confirmData(user, socket);
	int ret = 1;
	while(ret) {
		char input[100];
		printf("$ ");
		char* testa = fgets(input, sizeof(input), stdin);
		*strchr(input, '\n') = 0;
		if(testa == NULL) {
			error(-1);
		}
		ret = process(socket, input);
	}
	close(socket);
	return 0;
}
