//gcc main.c rendering.c resource.o -o EepyAlarm.exe -IC:\raylib\w64devkit\include -LC:\raylib\w64devkit\lib -lraylib -lopengl32 -lgdi32 -lwinmm -lole32 -mwindows

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <direct.h>


#include <shlobj.h>
#include <combaseapi.h>
#include <windows.h>
#define MAINICON 1

#define INITGUID
#include <knownfolders.h>

#include "rendering.h"
#include "variables.h"


time_t current_time_t;
time_hr target_time;

time_t last_increment;
double trigger_interval;

FILE* config_ptr;
wchar_t config_path[MAX_PATH];


void initializeConfig();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	CreateMutex(NULL, false, "EepyAlarmMutex");

	if(GetLastError() == ERROR_ALREADY_EXISTS) {
		printWindow("EepyAlarm is already running!");
		exit(0);
	}

	initializeConfig();

	while(1) {
		Sleep(30000);
		time(&current_time_t);

		if(difftime(current_time_t, last_increment) < trigger_interval) {
			// struct tm current_time_tm = *localtime(&current_time_t);
			// struct tm li = *localtime(&last_increment);

			// char s[64];
			// snprintf(s, sizeof(s), "%d:%d, %d:%d, %f ", current_time_tm.tm_hour, current_time_tm.tm_min, li.tm_hour, li.tm_min, difftime(current_time_t, last_increment));
			// printWindow(s);
			continue;
		}

		alarm();
		
		if((config_ptr = _wfopen(config_path, L"rb+")) == NULL) {
			char s[64];
			snprintf(s, sizeof(s), "Configuration File Error: %s, %d", strerror(errno), errno);
			printWindow(s);
			exit(1);
		}

		struct tm local = *localtime(&last_increment);
		if(local.tm_hour == target_time.tm_hour && local.tm_min == target_time.tm_min) {
			trigger_interval = 86400;
			fseek(config_ptr, sizeof(time_t), SEEK_SET);
			fwrite(&trigger_interval, sizeof(double), 1, config_ptr);
			printWindow("You hit your goal!");
		}

		last_increment += trigger_interval;
		fwrite(&last_increment, sizeof(time_t), 1, config_ptr);
		fclose(config_ptr);	
	}

	return 0;
}


void initializeConfig() {
	time(&current_time_t);

	PWSTR userdata = NULL;
	if(SHGetKnownFolderPath(&FOLDERID_LocalAppData, 0, NULL, &userdata) < 0) {
		char s[256];
		snprintf(s, sizeof(s), "Can't get path to config");
		printWindow(s);
		exit(1);
	}

	_snwprintf(config_path, sizeof(config_path), L"%ls\\EepyAlarm\\config.bin", userdata);

	
	if((config_ptr = _wfopen(config_path, L"rb+")) == NULL) {
		if(errno != ENOENT && errno != 0) {
			char s[256];
			snprintf(s, sizeof(s), "Can't open configuration file: %s, %d", strerror(errno), errno);
			printWindow(s);
			exit(1);
		}

		wchar_t folder_path[256];
		_snwprintf(folder_path, sizeof(folder_path), L"%ls\\EepyAlarm", userdata);
		if(!CreateDirectoryW(folder_path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
			char s[256];
			snprintf(s, sizeof(s), "Can't make new folder");
			printWindow(s);
			exit(1);
		}

		if((config_ptr = _wfopen(config_path, L"wb+")) == NULL) {
			char s[256];
			snprintf(s, sizeof(s), "Can't make new configuration file: %s, %d", strerror(errno), errno);
			printWindow(s);
			exit(1);
		}

		openConfigWindow(config_path, false);
	} else {
		fread(&last_increment, sizeof(time_t), 1, config_ptr);
		fread(&trigger_interval, sizeof(double), 1, config_ptr);
		fread(&target_time, sizeof(time_hr), 1, config_ptr);

		openConfigWindow(config_path, true);
	}

	CoTaskMemFree(userdata);
	fclose(config_ptr);
}
