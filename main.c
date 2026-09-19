#include <stdio.h>
#include <stdlib.h>
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

int main(int argc, char* argv) {
	time(&current_time_t);
	char* userdata = getenv("USERPROFILE");
	char config_path[256];
	char folder_path[256];
	sprintf(config_path, "%s\\EepyAlarm\\config.bin", userdata);
	sprintf(folder_path, "%s\\EepyAlarm", userdata);

	if((config_ptr = fopen(config_path, "rb+")) == NULL || argc > 1) {
		if(errno != ENOENT && errno != 0) {
			printf("Can't open configuration file: %s, %d", strerror(errno), errno);
			exit(1);
		}
		if(_mkdir(folder_path) != 0 && errno != EEXIST) {
			printf("Can't make new folder: %s, %d", strerror(errno), errno);
			exit(1);
		}
		if((config_ptr = fopen(config_path, "wb")) == NULL) {
			printf("Can't make new configuration file: %s, %d", strerror(errno), errno);
			exit(1);
		}


		target_time = (time_hr){ -1, -1 };

		time_hr start_time = (time_hr){ -1, -1 };

		int increment = -1; //seconds

		trigger_interval = 86400 - increment; //seconds

		struct tm current_time_tm = *localtime(&current_time_t);
		current_time_tm.tm_min = start_time.tm_min;
		current_time_tm.tm_hour = start_time.tm_hour;
		last_increment = mktime(&current_time_tm);

		fwrite(&last_increment, sizeof(time_t), 1, config_ptr);
		fwrite(&trigger_interval, sizeof(double), 1, config_ptr);
		fwrite(&target_time, sizeof(time_hr), 1, config_ptr);
		
	} else {
		fread(&last_increment, sizeof(time_t), 1, config_ptr);
		fread(&trigger_interval, sizeof(double), 1, config_ptr);
		fread(&target_time, sizeof(time_hr), 1, config_ptr);
	}

	fclose(config_ptr);
	printf("Good to go!");

	while(1) {
		time(&current_time_t);
		if(difftime(current_time_t, last_increment) > trigger_interval) {
			printf("Alarm!");
			while(last_increment < current_time_t) {
				last_increment += trigger_interval;
			}
			rewind(config_ptr);
			fwrite(&last_increment, sizeof(time_t), 1, config_ptr);

			struct tm local = *localtime(&last_increment);
			if(local.tm_hour == target_time.tm_hour && local.tm_min == target_time.tm_min) {
				trigger_interval = 86400;
				fseek(config_ptr, sizeof(time_t), SEEK_SET);
				fwrite(&trigger_interval, sizeof(double), 1, config_ptr);
				printf("You hit your goal!");
			}
		} else {
			printf("%f ", difftime(current_time_t, last_increment));
		}
		Sleep(30000);
	}

	
	return 0;
}