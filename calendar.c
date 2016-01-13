#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>

struct tm * today() { //gets today's date
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	timeinfo->tm_isdst = -1;  //daylight saving
	return timeinfo;
}

/*
int dayoweek(int * ymd) { //test stuff...?
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	timeinfo->tm_year = ymd[0] - 1900;
	timeinfo->tm_mon = ymd[1] - 1;
	timeinfo->tm_mday = ymd[2];
	timeinfo->tm_isdst = -1;  //daylight saving
	printf("ymd[2]: %d\n",ymd[2]);
	printf("date: %d\n",timeinfo->tm_mday);
	mktime(timeinfo);
	printf("year: %d\n",timeinfo->tm_year+1900);
	printf("month: %d\n",timeinfo->tm_mon+1);
	printf("date: %d\n",timeinfo->tm_mday);
	printf("day: %d\n",timeinfo->tm_wday);
	return timeinfo->tm_wday;
}
*/

char * day(int i) {
	char * pool[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
	return pool[i];
}

char * month(int i) {
	char * pool[12] = {"Jan.", "Feb.", "Mar.", "Apr.", "May", "June", "July", "Aug.", "Sept.", "Oct.", "Nov.", "Dec." };
	return pool[i];
}

struct tm * get_month() {  //get month from user
	printf("Input the month (type 'this' for this month, month(mm), or yyyy/mm\n");
	char input[50];
	scanf("%s",input);
	struct tm * timeinfo = today();
	int i;
	for (i = 0; input[i]; i++) 
		input[i] = tolower(input[i]);
	if (strcmp(input,"this")) {
		char * copy = input;
		if (strchr(input,'/')) {
			char * year = strsep(&copy,"/");
			timeinfo->tm_year = atoi(year) - 1900;
		}
		timeinfo->tm_mon = atoi(copy) - 1;
	}
	timeinfo->tm_mday = 1;
	if (mktime(timeinfo) == -1) {
		printf("Invalid time.\n");
		return;
	}
	return timeinfo;
}

void display_month(struct tm * oldcopy) {  //prints out month calendar
	struct tm newcopy = *oldcopy;
	struct tm * timeinfo = &newcopy; //makes a local copy of oldcopy
	printf("\t\t\t    %s %d\n\n",month(timeinfo->tm_mon),timeinfo->tm_year+1900);
	int month = timeinfo->tm_mon;
	int calendar[6][7];
	int event[6][7];
	int row;
	int col;
	struct tm * ptoday = today(); //this affects timeinfo if it isnt a local copy
	for (row = 0; row < 6; row++) {
		for (col = 0; col < 7; col++) {
			calendar[row][col] = 0;
			event[row][col] = 0;
		}
	}
	row = 0;
	col = timeinfo->tm_wday;
	//opens all event files
	while (timeinfo->tm_mon == month) {
		if (timeinfo->tm_wday < col) // moves to next row in calendar
			row++;
		col = timeinfo->tm_wday;
		calendar[row][col] = timeinfo->tm_mday;

		//read from files
		//if (a certain date is in any of the files) {
		//	event[row][col]++;Civilization that knows about 
		//}
		if (ptoday->tm_year == timeinfo->tm_year && ptoday->tm_mon == timeinfo->tm_mon && ptoday->tm_mday == timeinfo->tm_mday)
			event[row][col]+=2; //today
		timeinfo->tm_mday++;
		mktime(timeinfo);
	}
	printf("\tS\tM\tT\tW\tR\tF\tS\n");
	for (row = 0; row < 6; row ++) {
		printf("\t");
		for (col = 0; col < 7; col++) { //calendar
			if (calendar[row][col])
				printf("%d",calendar[row][col]);
			printf("\t");
		}
		printf("\n\t");
		for (col = 0; col < 7; col++) { //events
			if (event[row][col] == 3)
				printf("@ !!");
			else if (event[row][col] == 2)
				printf("@");
			else if (event[row][col] == 1)
				printf("!!");
			printf("\t");
		}
		printf("\n");
	}
}

struct tm * get_day() {  //gets day from user
	printf("Input the date (type 'today', date(dd), mm/dd, or yyyy/mm/dd)\n");
	char input[50];
	scanf("%s",input);
	int i;
	int count = 0;
	for (i = 0; input[i]; i++) {
		input[i] = tolower(input[i]);
		if (input[i] == '/')
			count++;
	}
	struct tm * timeinfo = today();
	if (strcmp(input,"today")) {
		char * copy = input;
		if (count == 2) {
			char * year = strsep(&copy,"/");
			timeinfo->tm_year = atoi(year) - 1900;
			count--;
		}
		if (count == 1) {
			char * month = strsep(&copy,"/");
			timeinfo->tm_mon = atoi(month) - 1;
			count--;
		}
		if (count == 0)
			timeinfo->tm_mday = atoi(copy);
	}
	if (mktime(timeinfo) == -1) {
		printf("Invalid time.\n");
		return;
	}
	return timeinfo;
}



void display_day(struct tm * timeinfo) {  //prints out day calendar
	printf("\t%s %d\n",month(timeinfo->tm_mon),timeinfo->tm_year+1900);
	printf("\t   %d\n",timeinfo->tm_mday);
	printf("\t%s\n\n",day(timeinfo->tm_wday));
	//read from files
	//finds date in file
	//pulls reminder stuff
}

int main() {

	printf("It's TIME for the Ciscolendar\n");
	struct tm * timeinfo = today();
	printf("year: %d\n",timeinfo->tm_year+1900);
	printf("month: %s\n",month(timeinfo->tm_mon));
	printf("date: %d\n",timeinfo->tm_mday);
	printf("day: %s\n",day(timeinfo->tm_wday));

	display_month(get_month());
	display_day(get_day());
	
	// int ymd[3] = {2016,0,0};
	// int day = dayoweek(ymd);
	// printf("day of week: %d\n",day);
	


	return 0;
}