#include "server.h"

void error(int r) {
	if(r < 0) {
		printf("Error: %s\n", strerror(errno));
	}
}

int connect(int* to) {
	char* wkp = "connect";
	int test = mkfifo(wkp, 0666);
	error(test);
	*to = open(wkp, O_RDONLY);
	error(*to);
	printf("<S> Connected to WKP\n");
	char buffer[100];
	test = read(*to, buffer, sizeof(buffer)); 
	error(test);
	printf("<C> Connected to WKP\n");
	printf("Private Pipe Name: %s\n", buffer);
	remove(wkp);
	int parent = fork();
	error(parent);
	if(!parent) {
		int from = open(buffer, O_WRONLY);
		error(from);
		printf("<S> Connected to Private\n");
		char* confirm = "<S> Connected to Private\n";
		test = write(from, confirm, sizeof(confirm));
		error(test);
		return from;
	}
	else {
		return -1;
	}
}

void process(int from, int to) {
	char buffer[100];
	int test = read(to, buffer, sizeof(buffer));
	error(test);
	printf("Recieved: %s\n", buffer);
	int len = strlen(buffer);
	if (buffer[len - 1] == '.') {
		buffer[len - 1] = '\0';
	}
	else{
		buffer[len] = '.';
		buffer[len + 1] = '\0';
	}
	test = write(from, buffer, sizeof(buffer));
	error(test);
	printf("Sending: %s\n", buffer);
}

static void sighandler(int signo) {
	if(signo == SIGINT) {
		remove("connect");
		exit(0);
	}
}

int main() {
	signal(SIGINT, sighandler);
	int from = -1;
	int to;
	while(from == -1) {
		from = connect(&to);
	}
	while(1) {
		process(from, to);
	}
	return 0;
}
