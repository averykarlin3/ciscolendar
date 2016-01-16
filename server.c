#include "server.h"

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
	sprintf(path, "%s/%s", "./data", user);
	struct stat buffer;
	int data;
	if(!stat(path, &buffer)) {
		data = open(path, flags, 0666);  
		error(data);
	}
	else {
		data = open(path, flags | O_CREAT, 0666);
		error(data);
		time_t start = 0;
		char* starts = asctime(&start);
		write(data, starts, sizeof(starts));
	}
	return data;
}

void confirmData(char* user, int socket) { //From client - modify for server
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
	if(check > 0) { //More recent server file
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
		char* nows = asctime(&now);
		test = write(data, nows, sizeof(nows));
		error(test);
		char buffer[d.st_size];
		test = read(data, buffer, d.st_size);
		error(test);
		test = write(socket, buffer, d.st_size);
		error(test);
	}
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
