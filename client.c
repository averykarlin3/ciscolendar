#include "client.h"
#include "calendar.h"

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

int sock(char* ipadd) {
/*
 *Parameters: NA
 *Outputs: int id - The file descriptor of the socket to the server
 *Function: Creates a socket to a server
*/	
 	int i;
	int id = socket(AF_INET, SOCK_STREAM, 0);
	error(id);
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(1701);
	inet_aton(ipadd, &(serv.sin_addr));
	printf("test2\n");
	i = connect(id, (struct sockaddr*) &serv, sizeof(serv));
	printf("<client> connect returned: %d\n", i);
	error(i);
	if (i == -1) {
		exit(0);
	}
	return id;
}

int checkConnection() {
/*Output - Returns 0 if the internet is down, or some positive number otherwise
 *Function - Checks if the internet is connected
*/
	int google = socket(AF_INET, SOCK_STREAM, 0);
	error(google);
	struct sockaddr_in g;
	g.sin_family = AF_INET;
	g.sin_port = htons(80);
	inet_aton("74.125.224.72", &(g.sin_addr));
	int test = connect(google, (struct sockaddr*) &g, sizeof(g));
	close(google);
	return ++test;
}

int alphaOnly(char* s) {
	/* Output: Returns 1 if s only contains letters of the alphabet, 0 otherwise
	*/
	int i = 0;
	while (s[i]) {
		if (!(((s[i]> 64) && (s[i] < 91)) || ((s[i] > 96) && (s[i] < 123)))) {
			return 0;
		}
		i += 1;
	}
	return 1;
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

int signup(int socket) {
	/* Function: Makes an account on the server for the user
	   Output: 1 if the login was successful, 0 if it was a failure
	*/
	char buffer[16] = "signup";
	int test = write(socket, buffer, 16);
	char * error;
	char input[128];
	char* user = NULL;
	char* pass = NULL;
	while (!user) {
		printf("Please enter a username (only letters of the alphabet are allowed): ");
		fflush(stdin);
		error = fgets(input,sizeof(input),stdin);
		user = input;
		user = strsep(&user, "\n");
		if (!alphaOnly(user)) {
			user = NULL;
			printf("\n");
		}
	}
	test = write(socket, input, sizeof(input));
	printf("Please enter a password: ");
	fflush(stdin);
	error = fgets(input,sizeof(input),stdin);
	nullify(input, 128);
	test = write(socket, input, 128);
	printf("Congratulations your account has been made!!!\n");
	int timeFil = open("timestamp", O_TRUNC | O_CREAT | O_WRONLY, 0744);
	time_t now = time(NULL);
	char nows[DTS];
	sprintf(nows, "%li", (long) now);
	nullify(nows, DTS);
	test = write(timeFil, nows, sizeof(nows));
	close(timeFil);
	return 1;
}

int login(int socket) {
	/* Function: Logs the user in and updates any necessary files
	   Output: 1 if the login was successful, 0 if it was a failure
	*/
	int usrSuccess = 0;
	int pasSuccess = 0;
	char input[128];
	char buffer[16] = "login";
	char* error;
	int test = write(socket, buffer, 16);
	while (!usrSuccess) {
		printf("What is your username: ");
		fflush(stdin);
		error = fgets(input,sizeof(input),stdin);
		printf("username: %s\n", input);
		if (input[strlen(input) - 1] == '\n') {
			input[strlen(input) - 1] = '\0';
		}
		nullify(input, 128);
		write(socket, input, 128);
		read(socket, input, 128);
		usrSuccess = atoi(input);
		if (usrSuccess == 0) {
			printf("Invalid Username!!!\n");
		}
	}
	while (!pasSuccess) {
		printf("What is your password: ");
		fflush(stdin);
		error = fgets(input,sizeof(input),stdin);
		if (input[strlen(input) - 1] == '\n') {
			input[strlen(input) - 1] = '\0';
		}
		printf("hfks: %s\n", input);
		nullify(input, 128);
		printf("hfks: %s\n", input);
		write(socket, input, sizeof(input));
		read(socket, input, sizeof(input));
		pasSuccess = atoi(input);
		if (pasSuccess == 0) {
			printf("Incorrect Password!!!\n");
		}
	}
	printf("The Login Was Correct. The two calendars are being reconciled.\n");
	int fil = open("timestamp", O_RDONLY, 0744);
	read(fil,input, sizeof(input));
	write(socket, input, sizeof(input));
	read(socket, input, sizeof(input));
	char textFil[65535];
	int datfil;
	if (atoi(input) == 1) {
		read(socket, textFil, sizeof(textFil));
		nullify(textFil, 65535);
		datfil = open("event.dat", O_WRONLY | O_TRUNC | O_CREAT, 0744);
		printf("%i\n", datfil);
		write(datfil, textFil, sizeof(textFil));
	}
	else {		
		printf("<C> - here\n");
		int datfil = open("event.dat", O_RDONLY, 0744);
		printf("%i\n", datfil);
		write(datfil, textFil, sizeof(textFil));
		write(socket, textFil, sizeof(textFil));
	}
	close(fil);
	close(datfil);
	return 1;
}

int main() {
	int socket = -1;
	int conn = checkConnection();
	if (!conn) {
		printf("Not connected to the internet. The program will not skip straight to the calendar portion. \n");
	}
	char* user = NULL;
	int success = 0;
	char * error;
	char temp[50];
	char input[100];
	char* temps;
	if (conn) {
		printf("\n Would you like to access the calendar or the server? Type y for the calendar and any other text for the server: ");
		fflush(stdin);
		error = fgets(input,sizeof(input),stdin);
		if (!strcmp(input,"y") || !strcmp(input,"y\n")) {
			conn = 0;
		}
	}
	if (conn) {
		while (socket == -1) {
			printf("Please enter the IP Address of the server you would like to connect to: ");
			fflush(stdin);
			error = fgets(temp,sizeof(temp),stdin);
			printf("Test1\n");
			socket = sock(temp);
			printf("Test1\n");
		}		
		while (!success) { //While the user is not logged in
			printf("Please enter whether you would like to login or signup: ");
			fflush(stdin);
			error = fgets(temp,sizeof(temp),stdin);
			printf("%s\n",temp);
			if (!strcmp("login\n", temp)) {
				success = login(socket);
			}
			else if (!strcmp("signup\n", temp)) {
				success = signup(socket);
			}
			else if (!strcmp("exit\n", temp)){
				char buffer[16] = "exit";
				int test = write(socket, buffer, 16);
				exit(0);
			}
			else{
				printf("Invalid Input\n");
			}
		}
	}
	else {
		printf("\n \n \n");
		runcal();
		int timeFil = open("timestamp", O_TRUNC | O_CREAT | O_WRONLY, 0744);
		time_t now = time(NULL);
		char nows[DTS];
		sprintf(nows, "%li", (long) now);
		nullify(nows, DTS);
		int test = write(timeFil, nows, sizeof(nows));
		close(timeFil);
	}
	close(socket);
	return 0;
}
