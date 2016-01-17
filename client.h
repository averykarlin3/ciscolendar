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
int process(int socket, char* input, char* user);
void confirmData(char* user, int socket);
int openData(char* user, int flags);
int sock();
int checkConnection();
void timeUp(int data);
