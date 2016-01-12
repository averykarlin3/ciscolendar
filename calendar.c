#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>

struct tm * today() {
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	timeinfo->tm_isdst = -1;  //daylight saving
	return timeinfo;
}


int dayoweek(int * ymd) {
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

char * day(int i) {
	char * pool[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
	return pool[i];
}

void display_month(int * ym) {



	//pulls reminder stuff
}

void display_day() {
	printf("Input the date (type 'today', date, mm/dd, or yyyy/mm/dd)\n");
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
	if (!strcmp(input,"today")) {
	}
	else {
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
		if (count == 0) {
			timeinfo->tm_mday = atoi(copy);
		}
	}
	if (mktime(timeinfo) == -1) {
		printf("Invalid time.\n");
		return;
	}
	printf("%d/%d/%d\n",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday);
	printf("%s\n",day(timeinfo->tm_wday));


	//pulls reminder stuff
}

int main() {

	printf("It's TIME for the Ciscolendar\n");
	struct tm * timeinfo = today();
	printf("year: %d\n",timeinfo->tm_year+1900);
	printf("month: %d\n",timeinfo->tm_mon+1);
	printf("date: %d\n",timeinfo->tm_mday);
	printf("day: %s\n",day(timeinfo->tm_wday));

	display_day();
	
	// int ymd[3] = {2016,0,0};
	// int day = dayoweek(ymd);
	// printf("day of week: %d\n",day);
	


	return 0;
}