//gcc main.c rendering.c my_math.c resource.o -o EepyAlarm.exe -IC:\raylib\w64devkit\include -LC:\raylib\w64devkit\lib -lraylib -lopengl32 -lgdi32 -lwinmm -lole32 -mwindows

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <direct.h>

#include <shlobj.h>
#include <shellapi.h>
#include <combaseapi.h>
#include <windows.h>
#include <tlhelp32.h>
#define WM_ICON (WM_USER + 1)
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

bool dst;


void initializeConfig(int argc);
int findProccessName(const char *name);

LRESULT Wndproc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam, 
	LPARAM lParam) {
	switch(uMsg) {
	case WM_ICON:
		printWindow("UwU");
		return 0;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	CreateMutex(NULL, false, "EepyAlarmMutex");
	bool exists = GetLastError() == ERROR_ALREADY_EXISTS;
	HANDLE config_open = CreateEvent(NULL, true, false, "EepyAlarmConfigOpened");

	int argc = 0;
	CommandLineToArgvW(GetCommandLineW(), &argc);

	if(exists) {
		if(argc != 1) {
			exit(0);
		}
		SetEvent(config_open);
	}

	initializeConfig(argc);
	ResetEvent(config_open);

	NOTIFYICONDATA notify_data = {};
	
	WNDCLASSEX window_class = {};
	window_class.cbSize = sizeof(WNDCLASSEX);
	window_class.lpszClassName = "trayClass";
	window_class.hIcon = LoadIcon(hInstance, "MAINICON");
	window_class.hInstance = hInstance;
	window_class.lpfnWndProc = Wndproc;
	window_class.style = 0;
	RegisterClassEx(&window_class);

	notify_data.cbSize = sizeof(NOTIFYICONDATA);
	notify_data.hWnd = CreateWindowEx(0, "trayClass", "tray", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	notify_data.uID = 100;
	notify_data.uVersion = NOTIFYICON_VERSION;
	notify_data.uCallbackMessage = WM_ICON;
	notify_data.hIcon = LoadIcon(hInstance, "MAINICON");

	strncpy(notify_data.szTip, "Open configuration", sizeof(notify_data.szTip));
	notify_data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

	Shell_NotifyIcon(NIM_ADD, &notify_data);
	Shell_NotifyIcon(NIM_SETVERSION, &notify_data);

	while(1) {
		if(WaitForSingleObject(config_open, 30000) == WAIT_OBJECT_0) {
			exit(0);
		} else {
			SetEvent(config_open);
			ResetEvent(config_open);
		}


		time(&current_time_t);
		struct tm current_time_tm = *localtime(&current_time_t);

		int dst_change = 0.0;
		if(current_time_tm.tm_isdst != 0 && !dst) {
			dst_change = 1.0;
		} else if(current_time_tm.tm_isdst == 0 && dst) {
			dst_change = -1.0;
		}

		if(difftime(current_time_t, last_increment) < trigger_interval + 3600 * dst_change) {
			continue;
		}


		if(difftime(current_time_t, last_increment) < trigger_interval + 10800) {
			alarm();
		}
		
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

		dst = current_time_tm.tm_isdst != 0;
		fseek(config_ptr, sizeof(time_t) + sizeof(double) + sizeof(time_hr) + 2 * sizeof(bool), SEEK_SET);
		fwrite(&dst, sizeof(bool), 1, config_ptr);

		fclose(config_ptr);	
	}

	return 0;
}


void initializeConfig(int argc) {
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
		fread(&last_increment,   sizeof(time_t), 1, config_ptr);
		fread(&trigger_interval, sizeof(double), 1, config_ptr);
		fread(&target_time, 	 sizeof(time_hr), 1, config_ptr);
		fread(&current_tfhr, 	 sizeof(bool), 1, config_ptr);
		fread(&target_tfhr, 	 sizeof(bool), 1, config_ptr);
		fread(&dst, 			 sizeof(bool), 1, config_ptr);

		if(argc == 1) {
			openConfigWindow(config_path, true);
		}
	}

	CoTaskMemFree(userdata);
	fclose(config_ptr);
}

int findProccessName(const char *name) {
	HANDLE hSnapshot;
	PROCESSENTRY32 process_entry;
	int pid = 0;
	BOOL hResult;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) return -1;

	process_entry.dwSize = sizeof(PROCESSENTRY32);

	hResult = Process32First(hSnapshot, &process_entry);

	while(hResult) {
		if(strstr(process_entry.szExeFile, name) != NULL) {
			pid = process_entry.th32ProcessID;
			break;
		}
		hResult = Process32Next(hSnapshot, &process_entry);
	}
	CloseHandle(hSnapshot);
	return pid;
}