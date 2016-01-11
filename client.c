#include "client.h"

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
	printf("<C> Connected to WKP\n");
	int test = write(*from, private, sizeof(private));
	error(test);
	test = mkfifo(private, 0666);
	error(test);
	int to = open(private, O_RDONLY);
	printf("<C> Connected to Private\n");
	error(to);
	char buffer[100];
	test = read(to, buffer, sizeof(buffer));	
	error(test);
	remove(private);
	return to;
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

int main() {
	int from, to;
	to = connect(&from);
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
