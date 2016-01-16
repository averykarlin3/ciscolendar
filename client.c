#include "client.h"

#define DTS sizeof(time_t)

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
	struct stat buffer;
	int data;
	if(!stat(path, &buffer)) {
		data = open(path, flags , 0666);
		error(data);
	}
	else { //Account for new computer
		data = open(path, O_CREAT | flags, 0666);
		error(data);
		char start[sizeof(time_t)] = 0;
		write(data, start, sizeof(starts));
	}
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
	if(check > 0) { //More recent server file
		char ndata[(int) check];
		test = read(socket, ndata, check);
		error(test);
		close(data);
		int data = openData(user, O_RDWR | O_TRUNC);
		test = write(data, ndata, check);
		error(test);
	}
	else if(check == -1){
		//More recent client file 
		struct stat d;
		stat(user, &d);
		char* size;
		sprintf(size, "%i", d.st_size);
		test = write(socket, size, sizeof(size));
		error(test);
		test = lseek(data, 0, SEEK_SET);
		error(test);
		time_t now = time(NULL); //Update time first
		char nows[sizeof(time_t)];
		sprintf(nows, "%i", now);
		test = write(data, nows, sizeof(nows);
		error(test);
		char buffer[d.st_size];
		test = read(data, buffer, d.st_size);
		error(test);
		test = write(socket, buffer, d.st_size);
		error(test);
	}
	close(data);
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
