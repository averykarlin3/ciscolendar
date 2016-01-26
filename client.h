#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

void error(int r);
int sock(char* ipadd);
int checkConnection();
int login(int socket);
int signup(int socket);
int alphaOnly(char* s);
void nullify(char* s, int i);
