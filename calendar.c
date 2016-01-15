#include "calendar.h"

void pstruct(struct tm * timeinfo) {
	printf("\nyear: %d\nmonth: %d\nday :%d\nwday: %d\nyday: %d\ndst: %d\n",timeinfo->tm_year,timeinfo->tm_mon,timeinfo->tm_mday,timeinfo->tm_wday,timeinfo->tm_yday,timeinfo->tm_isdst);
}

struct tm * today() { //gets today's date
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	timeinfo->tm_isdst = -1;  //daylight saving
	return timeinfo;
}

char * day(int i) {
	char * pool[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
	return pool[i];
}

char * month(int i) {
	char * pool[12] = {"Jan.", "Feb.", "Mar.", "Apr.", "May", "June", "July", "Aug.", "Sept.", "Oct.", "Nov.", "Dec." };
	return pool[i];
}

int month_choice(struct tm * timeinfo) {
	printf("Things to do...(Input a number)\n");
	char * pool[8] = {"Go to today","Go to another month","Go to a day","Add reminder","Remove reminder","Connect to server","Switch user","Exit"};
	int size = 8;
	int i;
	for (i = 0; i < size; i++) {
		printf("%d. %s\n",i,pool[i]);
	}
	int input;
	scanf("%d",&input);
	printf("\n\n");
	if (input < 0 || input >= size) {
		printf("Invalid input.\n");
		return 1;
	}
	if (input == 0) {
		timeinfo = today();
		return 0;
	}
	if (input == 1) {
		timeinfo = get_month(timeinfo);
		return 1;
	}
	if (input == 2) {
		timeinfo = get_day(timeinfo);
		return 0;
	}
	if (input == 3) {
		month_event(timeinfo);
	}
	return 0;	
}

int day_choice(struct tm * timeinfo) {
	printf("Things to do...(Input a number)\n");
	char * pool[8] = {"Go to month calendar","Go to another day","Go to another month","Add to this day's reminder", "Remove this day's reminder","Connect to server", "Switch user","Exit"};
	int size = 8;
	int i;
	for (i = 0; i < size; i++) {
		printf("%d. %s\n",i,pool[i]);
	}
	int input;
	scanf("%d",&input);
	printf("\n\n");
	if (input < 0 || input >= size) {
		printf("Invalid input.\n");
		return 0;
	}
	if (input == 0) {
		return 1;
	}
	if (input == 1) {
		timeinfo = get_day(timeinfo);
		return 0;
	}
	if (input == 2) {
		timeinfo = get_month(timeinfo);
		return 1;
	}
	if (input == 3) {
		day_event(timeinfo);
		return 0;
	}
	return 1;
}


struct tm * get_month(struct tm * timeinfo) {  //get month from user
	printf("Input the month (type 'this' for this month, month(mm), or yyyy/mm\n");
	char input[50];
	scanf("%s",input);
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
	else {
		timeinfo = today();
	}
	timeinfo->tm_mday = 1;
	mktime(timeinfo);
	return timeinfo;
}

struct tm * display_month(struct tm * timeinfo) {  //prints out month calendar
	printf("\t\t\t    %s %d\n\n",month(timeinfo->tm_mon),timeinfo->tm_year+1900);
	int month = timeinfo->tm_mon;
	int calendar[6][7];
	int event_grid[6][7];
	int row;
	int col;
	//<the bane of my existence>
	int temp_year = timeinfo->tm_year;
	timeinfo = today();
	int today_year = timeinfo->tm_year;
	int today_mon = timeinfo->tm_mon;
	int today_day = timeinfo->tm_mday;
	timeinfo->tm_year = temp_year;
	timeinfo->tm_mon = month;
	timeinfo->tm_mday = 1;
	mktime(timeinfo);
	//</the bane of my existence>
	for (row = 0; row < 6; row++) {
		for (col = 0; col < 7; col++) {
			calendar[row][col] = 0;
			event_grid[row][col] = 0;
		}
	}
	row = 0;
	col = timeinfo->tm_wday;
	//opens all event files
	int reading = open("event.dat",O_CREAT|O_RDONLY,0666);
	event * read_event = (event *)malloc(sizeof(event));
	while (timeinfo->tm_mon == month) {
		int error = read(reading, read_event, sizeof(event));
		if (timeinfo->tm_wday < col) // moves to next row in calendar
			row++;
		col = timeinfo->tm_wday;
		calendar[row][col] = timeinfo->tm_mday;
		while (error) {
			if (read_event->year > timeinfo->tm_year || read_event->mon > timeinfo->tm_mon || read_event->day > timeinfo->tm_mday) {
				lseek(reading, sizeof(event) * -1, SEEK_CUR);
				break;
			}
			if (read_event->year == timeinfo->tm_year && read_event->mon == timeinfo->tm_mon && read_event->day == timeinfo->tm_mday) {
				//lseek(reading, sizeof(event) * -1, SEEK_CUR); 
				event_grid[row][col]++;
				break;
			}
			error = read(reading, read_event, sizeof(event));
		}
		//if (a certain date is in any of the files)
		//	event[row][col]++;
		if (today_year == timeinfo->tm_year && today_mon == timeinfo->tm_mon && today_day == timeinfo->tm_mday)
			event_grid[row][col]+=2; //today
		timeinfo->tm_mday++;
		mktime(timeinfo);
	}
	close(reading);
	free(read_event);
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
			if (event_grid[row][col] == 3)
				printf("@ !!");
			else if (event_grid[row][col] == 2)
				printf("@");
			else if (event_grid[row][col] == 1)
				printf("!!");
			printf("\t");
		}
		printf("\n");
	}
	timeinfo->tm_mon--;
	timeinfo->tm_mday = 1;
	mktime(timeinfo);
	return timeinfo;
}

struct tm * get_day(struct tm * timeinfo) {  //gets day from user
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
	else {
		timeinfo = today();
	}
	mktime(timeinfo);
	return timeinfo;
}



struct tm * display_day(struct tm * timeinfo) {  //prints out day calendar
	printf("\t%s %d\n",month(timeinfo->tm_mon),timeinfo->tm_year+1900);
	printf("\t   %d\n",timeinfo->tm_mday);
	printf("\t%s\n\n",day(timeinfo->tm_wday));
	int reading = open("event.dat",O_CREAT|O_RDONLY,0666);
	event * read_event = (event *)malloc(sizeof(event));
	int error = read(reading, read_event ,sizeof(event));
	printf("Events:\n");
	int i = 0;
	while (error) {
		if (read_event->year > timeinfo->tm_year || read_event->mon > timeinfo->tm_mon || read_event->day > timeinfo->tm_mday) {
			break;
		}
		if (read_event->year == timeinfo->tm_year && read_event->mon == timeinfo->tm_mon && read_event->day == timeinfo->tm_mday)
			printf("%d. %s\n", i++, read_event->text);
		error = read(reading, read_event, sizeof(event));
	}
	printf("\n");
	close(reading);
	free(read_event);
	return timeinfo;
}

event * make_event(int year, int mon, int day, char * text) {
	event * output = (event *)malloc(sizeof(event));
	output->year = year;
	output->mon = mon;
	output->day = day;
	strcpy(output->text,text);
	return output;
}

void write_event(event * new_event) {
	int writing = open("event.dat",O_CREAT|O_RDONLY,0666);
	int writing2 = open(".temp",O_CREAT|O_TRUNC|O_RDWR,0666);
	event * read_event = (event *)malloc(sizeof(event));
	int error = read(writing, read_event ,sizeof(event));
	while (error) {
		write(writing2, read_event, sizeof(event));
		error = read(writing,read_event,sizeof(event));
	}
	close(writing);
	lseek(writing2,0,SEEK_SET);

	writing = open("event.dat",O_CREAT|O_TRUNC|O_WRONLY);
	error = read(writing2, read_event, sizeof(event));
	while (error) {
		if (read_event->year >= new_event->year && read_event->mon >= new_event->mon && read_event->day > new_event->day) {
			lseek(writing2, sizeof(event) * -1,SEEK_CUR);
			break;
		}
		write(writing, read_event, sizeof(event));
		error = read(writing2, read_event, sizeof(event));
	}
	write(writing, new_event, sizeof(event));
	error = read(writing2, read_event, sizeof(event));
	while (error) {
		write(writing, read_event, sizeof(event));
		error = read(writing2, read_event, sizeof(event));
	}

	close(writing);
	close(writing2);
	free(read_event);
	printf("Writing event done.\n");
}

void day_event(struct tm * timeinfo) {
	printf("New event for %d/%d/%d: ",timeinfo->tm_year+1900, timeinfo->tm_mon+1,timeinfo->tm_mday);
	char input[100];
	scanf("%s",input);
	event * new_event = make_event(timeinfo->tm_year,timeinfo->tm_mon,timeinfo->tm_mday,input);
	write_event(new_event);
}

void month_event(struct tm * timeinfo) {
	printf("Choose a date: (input dd, mm/dd, or yyyy/mm/dd)\n");
	char input[50];
	scanf("%s",input);
	int i;
	int count = 0;
	for (i = 0; input[i]; i++) {
		if (input[i] == '/')
			count++;
	}
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
	printf("New event for %d/%d/%d: ",timeinfo->tm_year+1900, timeinfo->tm_mon+1,timeinfo->tm_mday);
	char input2[100];
	scanf("%s",input2);
	event * new_event = make_event(timeinfo->tm_year,timeinfo->tm_mon,timeinfo->tm_mday,input);
	write_event(new_event);
}

int main() {
	umask(0111);
	printf("It's TIME for the Ciscolendar!!\n");
	struct tm * timeinfo = today();
	int mode = 0; //0 for day calendar, 1 for month calendar
	while (1) {
		if (mode) { //month calendar
			timeinfo->tm_mday = 1;
			mktime(timeinfo);
			timeinfo = display_month(timeinfo);
			mode = month_choice(timeinfo);
		}
		else { //day calendar
			timeinfo = display_day(timeinfo);
			mode = day_choice(timeinfo);
		}
	}
	return 0;
}