#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "raylib.h"
#include "rendering.h"
#include "variables.h"

void alarm() {
	printWindow("Go to bed!");
}

void openConfigWindow(wchar_t* config_path, bool prevAvailable) {
	int inputs[5] = {0};
    int buffer_position[5] = {0};
    char input_buffer[5][16];

    bool state_change = false;
    bool saved = false;
    User_state state = IDLE;
    User_state prev_state = IDLE;

    if(prevAvailable) {
    	inputs[0] = target_time.tm_hour;
    	inputs[1] = target_time.tm_min;

    	struct tm current_time = *localtime(&last_increment);
    	inputs[2] = current_time.tm_hour;
    	inputs[3] = current_time.tm_min;

    	inputs[4] = (86400 - trigger_interval) / 60;

    	for(int i = 0; i < 5; i++) {
	    	buffer_position[i] = sprintf(input_buffer[i], "%d", inputs[i]);
	    	for(int j = buffer_position[i]; j < 16; j++) {
	    		input_buffer[i][j] = '\0';
	    	}
	    }
    } else {
    	for(int i = 0; i < 5; i++) {
	    	buffer_position[i] = 0;
	    	for(int j = 0; j < 16; j++) {
	    		input_buffer[i][j] = '\0';
	    	}
	    }
    }

	InitWindow(500, 500, "EepyAlarm Configuration");
    SetTargetFPS(60);

    Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);

    while (!WindowShouldClose()) {
    	if(IsMouseButtonPressed(0) || IsMouseButtonPressed(1)) {
    		int y = GetMouseY();
    		if(y >= 130 && y <= 150) {
    			prev_state = state;
    			state = TARGET_HOUR;
    			state_change = true;
    		} else if (y >= 160 && y <= 180) {
    			prev_state = state;
    			state = TARGET_MIN;
    			state_change = true;
    		} else if (y >= 230 && y <= 250) {
    			prev_state = state;
    			state = CURRENT_HOUR;
    			state_change = true;
    		} else if (y >= 260 && y <= 280) {
    			prev_state = state;
    			state = CURRENT_MIN;
    			state_change = true;
    		} else if (y >= 300 && y <= 320) {
    			prev_state = state;
    			state = INCREMENT;
    			state_change = true;
    		} else {
    			prev_state = state;
    			state = IDLE;
    			state_change = true;
    		}
    	}

    	if(state != IDLE) {
			char c = GetCharPressed();
			if(IsKeyPressed(KEY_BACKSPACE) && buffer_position[state] > 0) {
				for(int i = buffer_position[state]-1; i < 16; i++) {
					input_buffer[state][i] = '\0';
				}
				buffer_position[state]--;
				saved = true;
			} else if(buffer_position[state] < 8 && c != 0 && c - '0' > -1 && c - '0' < 10) {
	    		input_buffer[state][buffer_position[state]] = c;
	    		buffer_position[state]++;
	    		saved = true;
	    	}	    	
	    }

	    if(state_change) {
	    	inputs[prev_state] = atoi(input_buffer[prev_state]);
	    	state_change = false;
	    	saved = false;
	    }

    	BeginDrawing();

    	ClearBackground(RAYWHITE);
    	DrawText("Target time: ", 100, 100, 20, BLACK);
    	DrawText("Current time: ", 100, 200, 20, BLACK);
    	DrawText("Close the window after saving settings.", 50, 400, 20, RED);
    	DrawText("Invalid settings will be set to zero upon", 50, 420, 20, RED);
    	DrawText("closing the window.", 50, 440, 20, RED);

    	switch(state) {
    	case TARGET_HOUR:
    		DrawRectangleLines(95, 125, 300, 30, DARKGRAY);
    		break;
    	case TARGET_MIN:
    		DrawRectangleLines(95, 155, 300, 30, DARKGRAY);
    		break;
    	case CURRENT_HOUR:
    		DrawRectangleLines(95, 225, 300, 30, DARKGRAY);
    		break;
    	case CURRENT_MIN:
    		DrawRectangleLines(95, 255, 300, 30, DARKGRAY);
    		break;
    	case INCREMENT:
    		DrawRectangleLines(95, 295, 300, 30, DARKGRAY);
    		break;
    	default:
    	}


    	char num[64];
    	snprintf(num, sizeof(num), "Hour: %s", input_buffer[0]);
    	DrawText(num, 100, 130, 20, BLACK);
    		
		snprintf(num, sizeof(num), "Minute: %s", input_buffer[1]);
   		DrawText(num, 100, 160, 20, BLACK);
		
		snprintf(num, sizeof(num), "Hour: %s", input_buffer[2]);
		DrawText(num, 100, 230, 20, BLACK);
 
		snprintf(num, sizeof(num), "Minute: %s", input_buffer[3]);
		DrawText(num, 100, 260, 20, BLACK);
    		
		snprintf(num, sizeof(num), "Daily increment (minutes): %s", input_buffer[4]);
    	DrawText(num, 100, 300, 20, BLACK);

    	if(!saved) {
    		DrawText("Saved!", 300, 200, 50, GREEN);
    	}
    	EndDrawing();
    }

    UnloadImage(icon);
    CloseWindow();	

    target_time.tm_hour = inputs[0];
	target_time.tm_min = inputs[1];

	time(&current_time_t);
	struct tm current_time_tm = *localtime(&current_time_t);
	current_time_tm.tm_hour = inputs[2];
	current_time_tm.tm_min = inputs[3];
	//--current_time_tm.tm_mday;
	last_increment = mktime(&current_time_tm);

	trigger_interval = 86400 - inputs[4] * 60; //seconds

	rewind(config_ptr);
	fwrite(&last_increment, sizeof(time_t), 1, config_ptr);
	fwrite(&trigger_interval, sizeof(double), 1, config_ptr);
	fwrite(&target_time, sizeof(time_hr), 1, config_ptr);

	char s[256];
	snprintf(s, sizeof(s), "%d:%d, %d:%d, %f", current_time_tm.tm_hour, current_time_tm.tm_min, target_time.tm_hour, target_time.tm_min, trigger_interval);
	printWindow(s);
}

void printWindow(char* str) {
	InitWindow(500, 40, "Message from EepyAlarm");
	SetTargetFPS(60);

	Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);

	
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText(str, 10, 10, 20, BLACK);
		EndDrawing();
	}

	UnloadImage(icon);
	CloseWindow();
}
