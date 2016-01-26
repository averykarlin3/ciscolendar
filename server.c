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
	printf("Test1\n");
	int from, to;
	int id = socket(AF_INET, SOCK_STREAM, 0);
	error(id);
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(1701);
	serv.sin_addr.s_addr = INADDR_ANY;
	bind(id, (struct sockaddr*)& serv, sizeof(serv));
	listen(id, 1);
	printf("Test4\n");
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

int openData(char* user, int flags) {
/*Inputs - char* user (Username), int flags (Flags used when opening the data file)
 *Outputs - int data (File descriptor of the data file)
 *Function - Opens the data file for the user, and returns the descriptor, creatin gthe file if it is not found
*/
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
/*Input - int socket (Socket file descriptor), char* user (Username)
 *Function - Checks if the data file for the user is more recently updated in the client or server, and copies the more recent to the server
*/
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
		sprintf(check, "%lli", (long long) d.st_size);
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
/*Inputs - int socket (socket file descriptor), char* user (username)
 *Function - Reads any input from the client, processes it as necissary, and writes whatever needed to the client
*/
	char buffer[100];
	int test = read(socket, buffer, sizeof(buffer));
	error(test);
	printf("<C %i> %s\n", socket, buffer);
	int data = openData(user, O_RDWR);
	//Server processing here (including changing file)
	if(!strcmp(buffer, "save"))
		confirmData(user, socket);
	timeUp(data);
	test = write(socket, buffer, sizeof(buffer));
	error(test);
}

void timeUp(int data) {
/*Inputs - int data (File descriptor of the data file)
 *Function - Updates the data file time at the top of the file, with the current time
*/
	time_t now = time(NULL);
	char nows[DTS];
	sprintf(nows, "%li", (long) now);
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
	char* username;
	char* inpassword;
	char* password;
	while (!usrSuccess) {
		test = read(socket, buffer, sizeof(buffer));
		fil = open(buffer, O_RDONLY, 0744);
		username = buffer;
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
	printf("We did it; We did it; Hooray!!!!!!\n");
	read(socket, buffer, sizeof(buffer));
	int servertim = atoi(buffstring);
	int clienttim = atoi(buffer);
	strcat(username, ".dat");
	char textFil[65535];
	int datfil;
	if (servertim > clienttim) {
		printf("<S> - s\n");
		sprintf(buffer, "1");
		write(socket, buffer, sizeof(buffer));
		datfil = open(username, O_RDONLY, 0744);
		write(datfil, textFil, sizeof(textFil));
		write(socket, textFil, sizeof(textFil));
	}
	else {
		printf("<S> - c\n");
		sprintf(buffer, "0");
		write(socket, buffer, sizeof(buffer));
		read(socket, textFil, sizeof(textFil));
		datfil = open(username, O_WRONLY | O_TRUNC | O_CREAT, 0744);
		write(datfil, textFil, sizeof(textFil));
	}
	printf("server: %i\n client: %i\n", servertim, clienttim);
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
	printf("Test3\n");
	int socket = -1;
	signal(SIGINT, sighandler);
	printf("Test2\n");
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
	return 0;
}
