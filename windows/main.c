#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <Windows.h>
#include <direct.h>

typedef struct {
	int tm_hour;
	int tm_min;
} time_hr;


time_t current_time_t;
time_hr target_time;

time_t last_increment;
double trigger_interval;

FILE* config_ptr;

void clearInputBuffer() {
	fflush(stdout);
}

void printWin(char *fmt, ...) {
	if(AllocConsole() == 0) {
		return;
	}
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);

	va_list ap;
	char *p, *sval;
	int ival;
	double dval;

	va_start(ap, fmt);
	for(p = fmt; *p; p++) {
		if(*p != '%') {
			putchar(*p);
			continue;
		}
		switch (*++p) {
		case 's':
			for(sval = va_arg(ap, char *); *sval; sval++) {
				putchar(*sval);
			}
			break;
		case 'd':
			ival = va_arg(ap, int);
			printf("%d", ival);
			break;
		case 'f':
			dval = va_arg(ap, double);
			printf("%f", dval);
			break;
		default:
			putchar(*p);
			break;
		}
	}
	va_end(ap);
	printf("\nPress [ENTER] to close the window. Please don't X out the window!");

	clearInputBuffer();
	getchar();

	fclose(stdin);
	fclose(stdout);
	if(FreeConsole() == 0) {
		printf("Failed to detach! Error code: %d", GetLastError());
		exit(1);
	}
}

void createConfigFile(char* config_path) {
		if((config_ptr = fopen(config_path, "wb")) == NULL) {
			printf("Can't make new configuration file: %s, %d", strerror(errno), errno);
			exit(1);
		}

		printf("Target time (hr min): ");
		target_time = (time_hr){ -1, -1 };
		clearInputBuffer();
		while(target_time.tm_hour < 0 || target_time.tm_min < 0) {
			char input[10];
			fgets(input, sizeof(input), stdin);
			sscanf(input, "%d %d", &target_time.tm_hour, &target_time.tm_min);
			clearInputBuffer();
		}

		printf("Starting time (hr min): ");
		time_hr start_time = (time_hr){ -1, -1 };
		clearInputBuffer();
		while(start_time.tm_hour < 0 || start_time.tm_min < 0) {
			char input[10];
			fgets(input, sizeof(input), stdin);
			sscanf(input, "%d %d", &start_time.tm_hour, &start_time.tm_min);
			clearInputBuffer();
		}

		printf("Time increment (min): ");
		int increment = -1; //seconds
		clearInputBuffer();
		while(increment < 0) {
			char input[10];
			fgets(input, sizeof(input), stdin);
			sscanf(input, "%d", &increment);
			clearInputBuffer();
		}

		trigger_interval = 86400 - increment; //seconds

		struct tm current_time_tm = *localtime(&current_time_t);
		current_time_tm.tm_min = start_time.tm_min;
		current_time_tm.tm_hour = start_time.tm_hour;
		--current_time_tm.tm_mday;
		last_increment = mktime(&current_time_tm);

		fwrite(&last_increment, sizeof(time_t), 1, config_ptr);
		fwrite(&trigger_interval, sizeof(double), 1, config_ptr);
		fwrite(&target_time, sizeof(time_hr), 1, config_ptr);
}


int main(int argc) {
	printf("Initializing...\n");
	time(&current_time_t);
	char* userdata = getenv("USERPROFILE");
	char config_path[256];
	sprintf(config_path, "%s\\EepyAlarm\\config.bin", userdata);
	
	if((config_ptr = fopen(config_path, "rb+")) == NULL || argc > 1) {
		if(errno != ENOENT && errno != 0) {
			printf("Can't open configuration file: %s, %d", strerror(errno), errno);
			exit(1);
		}

		char folder_path[256];
		sprintf(folder_path, "%s\\EepyAlarm", userdata);
		if(_mkdir(folder_path) != 0 && errno != EEXIST) {
			printf("Can't make new folder: %s, %d", strerror(errno), errno);
			exit(1);
		}

		createConfigFile(config_path);
	} else {
		fread(&last_increment, sizeof(time_t), 1, config_ptr);
		fread(&trigger_interval, sizeof(double), 1, config_ptr);
		fread(&target_time, sizeof(time_hr), 1, config_ptr);

		if(	  trigger_interval <= 0
		   || last_increment <= 0
		   || target_time.tm_hour < 0
		   || target_time.tm_min < 0
		) {
			createConfigFile(config_path);
		}
	}


	fclose(config_ptr);
	printf("Good to go!");
	Sleep(2000);
	if(FreeConsole() == 0) {
		printf("Failed to detach! Error code: %d", GetLastError());
		exit(1);
	}
	printWin("PID: %d", GetCurrentProcessId());

	while(1) {
		time(&current_time_t);
		if(difftime(current_time_t, last_increment) > trigger_interval) {
			struct tm local = *localtime(&last_increment);
			if(local.tm_hour == target_time.tm_hour && local.tm_min == target_time.tm_min) {
				trigger_interval = 86400;
				fseek(config_ptr, sizeof(time_t), SEEK_SET);
				fwrite(&trigger_interval, sizeof(double), 1, config_ptr);
				printWin("You hit your goal!");
			}

			printWin("Go to bed!");
			
			last_increment += trigger_interval;

			if((config_ptr = fopen(config_path, "rb+")) == NULL) {
				printWin("Configuration File Error: %s, %d", strerror(errno), errno);
				exit(1);
			}
			fwrite(&last_increment, sizeof(time_t), 1, config_ptr);

			fclose(config_ptr);
		}
		//else {
		//	struct tm current_time_tm = *localtime(&current_time_t);
		//	struct tm li = *localtime(&last_increment);
		//	printWin("%d:%d, %d:%d, %f ", current_time_tm.tm_hour, current_time_tm.tm_min, li.tm_hour, li.tm_min, difftime(current_time_t, last_increment));
		//}
		Sleep(30000);
	}

	return 0;
}