#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>

typedef struct {
	int year;
	int mon;
	int day;
	char text[100];
}	event;

struct tm * today();

char * day(int i);

char * month(int i);

int month_choice(struct tm * timeinfo);

int day_choice(struct tm * timeinfo);

struct tm * get_month(struct tm * timeinfo);

struct tm * display_month(struct tm * oldcopy);

struct tm * get_day(struct tm * timeinfo);

struct tm * display_day(struct tm * timeinfo);

event * make_event(int year, int mon, int day, char * text);

void write_event(event * new_event);

int display_event(struct tm * timeinfo);

void day_make_event(struct tm * timeinfo);

void month_make_event(struct tm * timeinfo);

void remove_event(event * old_event, int index);

void day_remove_event(struct tm * timeinfo);

void month_remove_event(struct tm * timeinfo);

int runcal();