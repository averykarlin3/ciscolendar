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
#include <time.h>

void error(int r);
void process(int socket, char* user);
void confirmData(char* user, int socket);
int openData(char* user, int flags);
int sock();
void timeUp(int data);
