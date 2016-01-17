#include "server.h"

#define DTS sizeof(int) * 8

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
		char* start = "0\n";
		write(data, start, sizeof(start));
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
	char check[DTS];
	if(atoi(buffer) > atoi(client)) {
		struct stat d;
		char* path;
		sprintf(path, "%s/%s", "~/.data", user);
		stat(path, &d);
		sprintf(check, "%i", d.st_size);
	}
	else {
		sprintf(check, "%i", -1);
	}
	test = write(socket, check, DTS);
	error(test);
	if(atoi(check) > 0) { //More recent server file
		timeUp(data);
		char ndata[atoi(check)];
		test = read(data, ndata, atoi(check));
		error(test);
		test = write(socket, ndata, atoi(check));
		error(test);
		close(data);
	}
	else if(atoi(check) == -1){
		//More recent client file 
		char size[DTS];
		test = read(socket, size, DTS);
		error(test);
		char ndata[atoi(size)];
		test = read(socket, ndata, sizeof(ndata));
		error(test);
		close(data);
		data = openData(user, O_RDWR | O_TRUNC);
		test = write(data, ndata, sizeof(ndata));
		error(test);
	}
	close(data);
}

void process(int socket, char* user) {
	char buffer[100];
	int test = read(socket, buffer, sizeof(buffer));
	error(test);
	printf("<C %i> %s\n", socket, buffer);
	int data = openData(user, O_RDWR);
	//Server processing here (including changing file)
	timeUp(data);
	test = write(socket, buffer, sizeof(buffer));
	error(test);
}

void timeUp(int data) {
	time_t now = time(NULL);
	char nows[DTS];
	sprintf(nows, "%i", now);
	int test = write(data, nows, sizeof(nows));
	error(test);
	test = lseek(data, 0, SEEK_SET);
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
		process(socket, user);
	}
	return 0;
}
