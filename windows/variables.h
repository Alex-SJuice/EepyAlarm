#pragma once

#include <minwindef.h>

typedef struct {
	int tm_hour;
	int tm_min;
} time_hr;

extern time_t current_time_t;
extern time_hr target_time;

extern time_t last_increment;
extern double trigger_interval;

extern FILE* config_ptr;
extern wchar_t config_path[MAX_PATH];
