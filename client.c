#include "client.h"

#define DTS 26

void error(int r) {
	if(r < 0) {
		printf("Error: %s\n", strerror(errno));
	}
}

int connect(int* from) {
	char* wkp = "connect";
	char private[100];
	sprintf(private, "%i", getpid());
	*from = open(wkp, O_WRONLY);
	error(*from);
	//printf("<C> Connected to WKP\n");
	int test = write(*from, private, sizeof(private));
	error(test);
	test = mkfifo(private, 0666);
	error(test);
	int to = open(private, O_RDONLY);
	//printf("<C> Connected to Private\n");
	error(to);
	char buffer[100];
	test = read(to, buffer, sizeof(buffer));	
	error(test);
	remove(private);
	return to;
}

int openData(char* user, int flags) {
	char* path = "~/.cal";
	int data = open(path, flags , 0666);
	error(data);
	//Account for nonexistant
	return data;
}

int process(int from, int to, char input[]) {
	if(!strcmp(input, "exit"))
		return 0;
	printf("<C> %s\n", input);
	int test = write(from, input, sizeof(input));
	error(test);
	char buffer[100];
	test = read(to, buffer, sizeof(buffer));
	error(test);
	printf("<S> %s\n", buffer);
	return 1;
}

void confirmData(char* user, int from, int to) {
	int data = openData(user, O_RDWR | O_CREAT);
	char buffer[DTS];
	test = read(data, buffer, DTS);
	error(test);
	test = write(from, buffer, DTS);
	error(test);
	char check[sizeof(int)];
	test = read(to, check, sizeof(int));
	error(test);
	char ndata[check];
	if(check > 0) {
		test = read(to, ndata, check);
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
	int from, to;
	//Set user
	to = connect(&from);
	confirmData(char* user, from, to);
	int ret = 1;
	while(ret) {
		char input[100];
		printf("$ ");
		char* testa = fgets(input, sizeof(input), stdin);
		*strchr(input, '\n') = 0;
		if(testa == NULL) {
			error(-1);
		}
		ret = process(from, to, input);
	}
	close(from);
	close(to);
	return 0;
}
