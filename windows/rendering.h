#pragma once

typedef enum {
	TARGET_HOUR,
	TARGET_MIN,
	CURRENT_HOUR,
	CURRENT_MIN,
	INCREMENT,
	IDLE
} User_state;

void printWindow(char *str);

void openConfigWindow(wchar_t* config_path, bool prevAvailable);

void alarm();
