#include "server.h"

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
	serv.sin_addr.s_addr = INADDR_ANY;
	bind(id, (struct sockaddr*)& serv, sizeof(serv));
	listen(id, 1);
	int parent = fork();
	if(!parent) {
		int client = accept(id, NULL, NULL);
		return client;
	}
	else {
		return -1;
	}
}

int openData(char* user, int flags) {
	char* path;
	sprintf(path, "%s/%s", "~/.data", user);
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

void confirmData(char* user, int socket) {
	int data = openData(user, O_RDWR | O_CREAT);
	char buffer[DTS];
	int test = read(data, buffer, DTS);
	error(test);
	char client[DTS];
	test = read(socket, buffer, DTS);
	error(test);
	char check[sizeof(int)];
	if(atoi(buffer) > atoi(client)) {
		struct stat d;
		char* path;
		sprintf(path, "%s/%s", "~/.data", user);
		stat(path, d);
		check = d.st_size;
	}
	else {
		check = -1;
	}
	test = write(socket, check, sizeof(int));
	error(test);
	if(check > 0) { //More recent server file
		time_t now = time(NULL);
		char nows[sizeof(time_t)];
		sprintf(nows, "%i", now);
		test = write(data, nows, sizeof(nows);
		error(test);
		test = lseek(data, 0, SEEK_SET);
		error(test);
		char ndata[(int) check];
		test = read(data, ndata, check);
		error(test);
		test = write(socket, ndata, check);
		error(test);
		close(data);
	}
	else if(check == -1){
		//More recent client file 
		char size[sizeof(int)];
		test = read(socket, size, sizeof(size));
		error(test);
		char ndata[size];
		test = read(socket, ndata, sizeof(ndata));
		error(test);
		close(data);
		data = openData(user, O_RDWR | O_TRUNC);
		test = write(data, ndata, sizeof(ndata));
		error(test);
	}
	close(data);
}

void process(int socket) {
	char buffer[100];
	int test = read(socket, buffer, sizeof(buffer));
	error(test);
	printf("<C %i> %s\n", socket, buffer);
	//Processing here
	test = write(socket, buffer, sizeof(buffer));
	error(test);
}

static void sighandler(int signo) {
	if(signo == SIGINT) {
		close(socket);
		exit(0);
	}
}

int main() {
	signal(SIGINT, sighandler);
	int socket;
	while(socket == -1) {
		socket = sock();
	}
	//Get user
	char* user;
	confirmData(user, socket);
	while(1) {
		process(socket);
	}
	return 0;
}
