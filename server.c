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

void readData(char* user, int flags) {
	char* path;
	sprintf(path, "%s/%s", "./data", user);
	int data = open(path, flags, 0666);  
	error(data);
}

void confirmData(char* user, int socket) {
	
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
		remove("connect");
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
