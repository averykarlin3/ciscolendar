#include "server.h"

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
 *Outputs: int client - The file descriptor of the client (-1 if the parent)
 *Function: Creates a socket to a client in a child process, returning -1 to the parent, and the client file descriptor to the child
*/
	int from, to;
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
		printf("<server> connected\n");
		from = open("parpause", O_WRONLY);
		return client;
	}
	else {
		int parpause = mkfifo("parpause", 0666);
		to = open("parpause", O_RDONLY);
		char buffer[100];
		parpause = read(to, buffer, sizeof(buffer)); 
		remove("parpause");
		printf("server escaped...\n");
		return -1;
	}
}


static void sighandler(int signo) {
	if(signo == SIGINT) {
		close(socket);
		exit(0);
	}
}

void nullify(char* s, int i) {
	/*Function: Of the first i chars, it sets all of the chars after the first null to null
	*/
	int j = 0;
	int passed = 0;
	while (j < i) {
		if (passed) {
			s[j] = '\0';
		}
		else {
			if (s[j] == '\0') {
				passed = 1;
			}
		}
		j += 1;
	}
}

int login(int socket) {
	/* Function: Communicates with the client to login the user and update his/her files
	   Output: Returns 0 upon failure, 1 upon success
	*/
	printf("<S> - Logged In\n");
	int usrSuccess = 0;
	int pasSuccess = 0;
	int test, fil;
	char buffer[128];
	char bigbuff[256];
	char* buffstring;
	char usbuff[128];
	char* username;
	char* inpassword;
	char* password;
	char* usercpy;
	while (!usrSuccess) {
		test = read(socket, usbuff, sizeof(buffer));
		fil = open(usbuff, O_RDONLY, 0744);
		username = usbuff;
		if (fil != -1) {
			usrSuccess = 1;
		}
		sprintf(buffer, "%i", usrSuccess);
		write(socket, buffer, sizeof(buffer));
	}
	while (!pasSuccess) {
		lseek(fil, 0, SEEK_SET);
		test = read(socket, buffer, 128);
		inpassword = buffer;
		read(fil, bigbuff, sizeof(bigbuff));
		buffstring = bigbuff;
		password = strsep(&buffstring, "\n");
		if (!strcmp(password, inpassword)) {
			pasSuccess = 1;
		}
		sprintf(buffer, "%i", pasSuccess);
		write(socket, buffer, sizeof(buffer));
		lseek(fil, 128, SEEK_SET);
		read(fil, bigbuff, sizeof(bigbuff));
		buffstring = bigbuff;

	}
	close(fil);
	read(socket, buffer, sizeof(buffer));
	int servertim = atoi(buffstring);
	int clienttim = atoi(buffer);
	strcat(username, ".dat");
	char textFil[65535];
	int datfil;
	if (servertim > clienttim) {
		sprintf(buffer, "1");
		write(socket, buffer, sizeof(buffer));
		datfil = open(username, O_RDONLY, 0744);
		write(datfil, textFil, sizeof(textFil));
		write(socket, textFil, sizeof(textFil));
	}
	else {
		sprintf(buffer, "0");
		write(socket, buffer, sizeof(buffer));
		read(socket, textFil, sizeof(textFil));
		datfil = open(username, O_WRONLY | O_TRUNC | O_CREAT, 0744);
		nullify(textFil, 65535);
		write(datfil, textFil, sizeof(textFil));
	}
	close(datfil);
	return 1;
}

int signup(int socket) {
	/* Function: Communicates with the client to signup the user and create his/her files
	   Output: Returns 0 upon failure, 1 upon success
	*/
	printf("<S> - Signed Up\n");
	char* userText;
	char buffer[128];
	char buff[128];
	int test = read(socket, buffer, sizeof(buffer));
	userText = buffer;
	printf("%s\n", userText);
	test = read(socket, buff, 128);
	int usrFile = open(userText, O_WRONLY | O_TRUNC | O_CREAT, 0744);
	write(usrFile, buff, sizeof(buff));
	time_t now = time(NULL);
	char nows[DTS];
	sprintf(nows, "%li", (long) now);
	nullify(nows, DTS);
	write(usrFile, nows, sizeof(nows));
	close(usrFile);
	strcat(userText,".dat");
	usrFile = open(userText, O_WRONLY | O_TRUNC | O_CREAT, 0744);
	close(usrFile);
	return 1;
}

int main() {
	int socket = -1;
	signal(SIGINT, sighandler);
	int test;
	char buffer[16];
	char* readText;
	int cont = 1;
	while(socket == -1) {
		socket = sock();
	}
	//Get user from client
	char* user;
	//confirmData(user, socket);
	while(cont) {
		test = read(socket, buffer, 16);
		readText = buffer;
		if (!strcmp(readText, "signup")) {
			signup(socket);
			cont = 0;
		}
		else if (!strcmp(readText, "login")) {
			login(socket);
		}
		else {
			cont = 0;
		}
		//process(socket, user);
	}
	printf("%i\n", getppid());
	printf("chlid left?\n");
	return 0;
}
