#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <combaseapi.h>
#include <shlobj.h>

#define INITGUID
#include <knownfolders.h>

typedef struct {
	int tm_hour;
	int tm_min;
} time_hr;

time_hr target_time;
time_t last_increment;
double trigger_interval;

FILE* config_ptr;
wchar_t config_path[MAX_PATH];

void main() {
	PWSTR userdata = NULL;
	if(SHGetKnownFolderPath(&FOLDERID_LocalAppData, 0, NULL, &userdata) < 0) {
		printf("Can't get path to config");
		exit(1);
	}

	_snwprintf(config_path, sizeof(config_path), L"%ls\\EepyAlarm\\config.bin", userdata);
	printf("%ls\n", config_path);

	config_ptr = _wfopen(config_path, L"rb");

	fread(&last_increment, sizeof(time_t), 1, config_ptr);
	fread(&trigger_interval, sizeof(double), 1, config_ptr);
	fread(&target_time, sizeof(time_hr), 1, config_ptr);

	struct tm li = *localtime(&last_increment);
	printf("%d:%d, %d:%d, %f", target_time.tm_hour, target_time.tm_min, li.tm_hour, li.tm_min, trigger_interval);

	fclose(config_ptr);

	getchar();
}
