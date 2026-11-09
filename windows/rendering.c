#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#include "raylib.h"
#include "rendering.h"
#include "variables.h"



void alarm() {
	InitWindow(500, 500, "ALARM!");
	ToggleFullscreen();
    HideCursor();

    Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);


    Vector2 origin = { 0.0f, 0.0f };
    Rectangle rect = { GetScreenWidth() / 2 - 300, GetScreenHeight() - 620, 600.0f, 600.0f };
    Texture2D orph = LoadTextureFromImage(icon);
    NPatchInfo patchInfo = { (Rectangle){ 0.0f, 0.0f, 512.0f, 512.0f }, 0, 0, 0, 0, NPATCH_NINE_PATCH };

    SetTargetFPS(60);

    while (!WindowShouldClose() || GetTime() < 12.0f) {
    	if(!IsWindowFocused()) {
			SetWindowFocused();
		}

		time(&current_time_t);
		double t = GetTime();
		struct tm current_time_tm = *localtime(&current_time_t);

		rect.width = 	600.0f * (cos(t / 1.0f) / 30.0f + 1.0f);
		rect.height = 	600.0f * (-cos(t / 1.0f) / 30.0f + 1.0f);
		rect.x = 		(GetScreenWidth() / 2 - 300) - (rect.width - 600.0f) / 2;
		rect.y = 		(GetScreenHeight() - 620) - (rect.height - 600.0f) / 2;

		double alpha = t * 255.0f / 4.0f;
		char a1;
		char a2;
		char a3;
		if(alpha > 255.0f) {
			a1 = 255;
		} else {
			a1 = (char)alpha;			
		}
		if(alpha - 255.0f > 255.0f) {
			a2 = 255;
		} else if(alpha - 255.0f < 0.0f) {
			a2 = 0;
		} else {
			a2 = (char)(alpha - 255.0f);
		}
		if(alpha - 510.0f > 255.0f) {
			a3 = 255;
		} else if(alpha - 510.0f < 0.0f) {
			a3 = 0;
		} else {
			a3 = (char)(alpha - 510.0f);
		}

		char message[32];
		snprintf(message, sizeof(message), "It's %d:%d.", current_time_tm.tm_hour, current_time_tm.tm_min);

		BeginDrawing();
			ClearBackground(BLACK);
			DrawText(message, GetScreenWidth() / 2 - MeasureText(message, 100) / 2, 100, 100, (Color){ 255, 255, 255, a1 });
			DrawText("Go to bed.", GetScreenWidth() / 2 - MeasureText("Go to bed.", 100) / 2, 200, 100, (Color){ 255, 255, 255, a2 });
			DrawText("Press ESC to close.", GetScreenWidth() / 2 - MeasureText("Press ESC to close", 50) / 2, 300, 50, (Color){ 255, 255, 255, a3 });

			DrawTextureNPatch(orph, patchInfo, rect, origin, 0.0f, WHITE);
		EndDrawing();
    }

    ToggleFullscreen();
    UnloadImage(icon);
    UnloadTexture(orph);
	CloseWindow();
}

void openConfigWindow(wchar_t* config_path, bool prevAvailable) {
	int inputs[5] = {0};
    int buffer_position[5] = {0};
    char input_buffer[5][3];

    Time_mode time_mode[2] = { TFHR, TFHR };

    if(prevAvailable) {
    	inputs[0] = target_time.tm_hour;
    	inputs[1] = target_time.tm_min;

    	struct tm current_time = *localtime(&last_increment);
    	inputs[2] = current_time.tm_hour;
    	inputs[3] = current_time.tm_min;

    	inputs[4] = (86400 - trigger_interval) / 60;

    	for(int i = 0; i < 5; i++) {
	    	buffer_position[i] = snprintf(input_buffer[i], sizeof(input_buffer[i]), "%d", inputs[i]);
	    	for(int j = buffer_position[i]; j < 3; j++) {
	    		input_buffer[i][j] = '\0';
	    	}
	    }
    } else {
    	for(int i = 0; i < 5; i++) {
	    	buffer_position[i] = 0;
	    	for(int j = 0; j < 3; j++) {
	    		input_buffer[i][j] = '\0';
	    	}
	    }
    }

    User_state state = IDLE;
    bool change = false;

    Clock current =  { { 180, 180 }, 120.0, getAngle(0, 12.0f), getAngle(0, 60.0f), TFHR };
    Clock target = { { 180, 520 }, 120.0, getAngle(0, 12.0f), getAngle(0, 60.0f), AM };
    HBNumber_line increment = { { 330.0, 630.0}, 550.0 , 100 , 0 };
    increment.selected = getPosition(0, increment);

	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(700, 700, "EepyAlarm Configuration");
    SetTargetFPS(60);
    

    Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);

    Font font = LoadFont("ShareTech-Regular.ttf");
    SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);

    while (!WindowShouldClose()) {
    	char s[256];
		snprintf(s, sizeof(s), "%f", increment.selected);
		if (IsMouseButtonPressed(0)) {
			switch(determineClockHandCollision((Vector2) { GetMouseX(), GetMouseY() }, target)) {
			case 1:
				state = TARGET_HOUR;
				break;
			case 2:
				state = TARGET_MIN;
				break;
			default:
				break;
			}
			switch(determineClockHandCollision((Vector2) { GetMouseX(), GetMouseY() }, current)) {
			case 1:
				state = CURRENT_HOUR;
				break;
			case 2:
				state = CURRENT_MIN;
				break;
			default:
				break;
			}
			if(state == IDLE) {
				if(CheckCollisionPointRec((Vector2) { GetMouseX(), GetMouseY() }, (Rectangle) { 20.0, 235.0, 45.0, 45.0 })) {
					current.mode = AM;
					state = CURRENT_HOUR;
					change = true;
				}
				if(CheckCollisionPointRec((Vector2) { GetMouseX(), GetMouseY() }, (Rectangle) { 60.0, 280.0, 44.0, 45.0 })) {
					current.mode = PM;
					state = CURRENT_HOUR;
					change = true;
				}
				if(CheckCollisionPointRec((Vector2) { GetMouseX(), GetMouseY() }, (Rectangle) { 120.0, 300.0, 81.0, 45.0 })) {
					current.mode = TFHR;
					state = CURRENT_HOUR;
					change = true;
				}
				if(CheckCollisionPointRec((Vector2) { GetMouseX(), GetMouseY() }, (Rectangle) { 20.0, 420.0, 45.0, 45.0 })) {
					target.mode = AM;
					state = TARGET_HOUR;
					change = true;
				}
				if(CheckCollisionPointRec((Vector2) { GetMouseX(), GetMouseY() }, (Rectangle) { 60.0, 375.0, 44.0, 45.0 })) {
					target.mode = PM;
					state = TARGET_HOUR;
					change = true;
				}
				if(CheckCollisionPointRec((Vector2) { GetMouseX(), GetMouseY() }, (Rectangle) { 120.0, 355.0, 81.0, 45.0 })) {
					target.mode = TFHR;
					state = TARGET_HOUR;
					change = true;
				}
				if(CheckCollisionPointRec((Vector2) { GetMouseX(), GetMouseY() }, (Rectangle) { increment.selected - 7.5, increment.y - 35, 15, 20 })) {
					snprintf(s, sizeof(s), "ah");
					state = INCREMENT;
					//change = true;
				}

			}
		} else if (IsMouseButtonUp(0)) {
			change = true;
		}
		if (change) {
			switch(state) {
			case TARGET_HOUR:
				int mod = 12;
				if (target.mode == TFHR) {
					mod = 24;
				}
				target.hour = getAngle(nearestPositionClock(target.hour, mod), mod);
				switch(target.mode) {
				case AM:
					inputs[0] = nearestPositionClock(target.hour - 2.0 * PI, mod) % 12;
					break;
				case PM:
					inputs[0] = nearestPositionClock(target.hour - 2.0 * PI, mod) % 12 + 12;
					break;
				case TFHR:
					inputs[0] = nearestPositionClock(target.hour - 2.0 * PI, mod) % 24;
					break;
				}
				break;
			case TARGET_MIN:
				target.minute = getAngle(nearestPositionClock(target.minute, 60), 60);
				inputs[1] = nearestPositionClock(target.minute - 2.0 * PI, 60);
				break;
			case CURRENT_HOUR:
				mod = 12;
				if (current.mode == TFHR) {
					mod = 24;
				}
				current.hour = getAngle(nearestPositionClock(current.hour, mod), mod);
				switch(current.mode) {
				case AM:
					inputs[2] = nearestPositionClock(current.hour - 2.0 * PI, mod) % 12;
					break;
				case PM:
					inputs[2] = nearestPositionClock(current.hour - 2.0 * PI, mod) % 12 + 12;
					break;
				case TFHR:
					inputs[2] = nearestPositionClock(current.hour - 2.0 * PI, mod) % 24;
					break;
				}
				break;
			case CURRENT_MIN:
				current.minute = getAngle(nearestPositionClock(current.minute, 60), 60);
				inputs[3] = nearestPositionClock(current.minute - 2.0 * PI, 60);
				break;
			case INCREMENT:
				increment.selected = getPosition(nearestPositionLine(increment.selected, increment), increment);
				inputs[4] = nearestPositionLine(increment.selected, increment);
				break;
			}
			change = false;
			state = IDLE;
		}

		switch (state) {
		case TARGET_HOUR:
			target.hour = atan2((double)(GetMouseY() - target.center.y), (double)(GetMouseX() - target.center.x));
			break;
		case TARGET_MIN:
			target.minute = atan2((double)(GetMouseY() - target.center.y), (double)(GetMouseX() - target.center.x));
			break;
		case CURRENT_HOUR:
			current.hour = atan2((double)(GetMouseY() - current.center.y), (double)(GetMouseX() - current.center.x));
			break;
		case CURRENT_MIN:
			current.minute = atan2((double)(GetMouseY() - current.center.y), (double)(GetMouseX() - current.center.x));
			break;
		case INCREMENT:
			increment.selected = floatClamp(GetMouseX(), increment.range.x, increment.range.y);
		default:
			break;
		}
		
		BeginDrawing();		
    	ClearBackground((Color) { 0, 29, 53, 255 });

    	switch(current.mode) {
    	case AM:
    		DrawTextEx(font, "AM", (Vector2) { 20.0, 235.0 }, 45, 1, WHITE);
    		DrawTextEx(font, "PM", (Vector2) { 60.0, 280.0 }, 45, 1, (Color) { 255, 255, 255, 100 });
    		DrawTextEx(font, "24HR", (Vector2) { 120.0, 300.0 }, 45, 1, (Color) { 255, 255, 255, 100 });
    		break;
    	case PM:
    		DrawTextEx(font, "AM", (Vector2) { 20.0, 235.0 }, 45, 1, (Color) { 255, 255, 255, 100 });
    		DrawTextEx(font, "PM", (Vector2) { 60.0, 280.0 }, 45, 1, WHITE);
    		DrawTextEx(font, "24HR", (Vector2) { 120.0, 300.0 }, 45, 1, (Color) { 255, 255, 255, 100 });
    		break;
    	case TFHR:
    		DrawTextEx(font, "AM", (Vector2) { 20.0, 235.0 }, 45, 1, (Color) { 255, 255, 255, 100 });
    		DrawTextEx(font, "PM", (Vector2) { 60.0, 280.0 }, 45, 1, (Color) { 255, 255, 255, 100 });
    		DrawTextEx(font, "24HR", (Vector2) { 120.0, 300.0 }, 45, 1, WHITE);
    		break;
    	}
    	switch(target.mode) {
    	case AM:
    		DrawTextEx(font, "AM", (Vector2) { 20.0, 420.0 }, 45, 1, WHITE);
    		DrawTextEx(font, "PM", (Vector2) { 60.0, 375.0 }, 45, 1, (Color) { 255, 255, 255, 100 });
    		DrawTextEx(font, "24HR", (Vector2) { 120.0, 355.0 }, 45, 1, (Color) { 255, 255, 255, 100 });
    		break;
    	case PM:
    		DrawTextEx(font, "AM", (Vector2) { 20.0, 420.0 }, 45, 1, (Color) { 255, 255, 255, 100 });
    		DrawTextEx(font, "PM", (Vector2) { 60.0, 375.0 }, 45, 1, WHITE);
    		DrawTextEx(font, "24HR", (Vector2) { 120.0, 355.0 }, 45, 1, (Color) { 255, 255, 255, 100 });
    		break;
    	case TFHR:
    		DrawTextEx(font, "AM", (Vector2) { 20.0, 420.0 }, 45, 1, (Color) { 255, 255, 255, 100 });
    		DrawTextEx(font, "PM", (Vector2) { 60.0, 375.0 }, 45, 1, (Color) { 255, 255, 255, 100 });
    		DrawTextEx(font, "24HR", (Vector2) { 120.0, 355.0 }, 45, 1, WHITE);
    		break;
    	}
    	
    	DrawClock(target);
    	DrawClock(current);
    	DrawHBLine(increment);

    	DrawTextEx(font, s, (Vector2){ GetMouseX() + 10, GetMouseY() }, 20, 1, GREEN);
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
	if(difftime(mktime(&current_time_tm), current_time_t) > 0) {
		--current_time_tm.tm_mday;
	}
	last_increment = mktime(&current_time_tm);

	trigger_interval = 86400 - inputs[4] * 60; //seconds

	rewind(config_ptr);
	fwrite(&last_increment, sizeof(time_t), 1, config_ptr);
	fwrite(&trigger_interval, sizeof(double), 1, config_ptr);
	fwrite(&target_time, sizeof(time_hr), 1, config_ptr);

	#ifdef DEBUG
	char s[256];
	snprintf(s, sizeof(s), "%d:%d, %d:%d, %f", current_time_tm.tm_hour, current_time_tm.tm_min, target_time.tm_hour, target_time.tm_min, trigger_interval);
	printWindow(s);
	#endif
}

void openConfigWindowOld(wchar_t* config_path, bool prevAvailable) {
	int inputs[5] = {0};
    int buffer_position[5] = {0};
    char input_buffer[5][16];

    Time_mode time_mode[2] = { TFHR, TFHR };

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
	    	buffer_position[i] = snprintf(input_buffer[i], sizeof(input_buffer[i]), "%d", inputs[i]);
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
    		int x = GetMouseX();
    		int y = GetMouseY();
    		if(y >= 100 && y <= 120) {
    			if(x >= 150 && x <= 160) {
    				time_mode[0] = AM;
    			} else if(x >= 170 && x <= 180) {
    				time_mode[0] = PM;
    			} else if(x >= 190 && x <= 200) {
    				time_mode[0] = TFHR;
    			}
    		} else if(y >= 130 && y <= 150) {
    			prev_state = state;
    			state = TARGET_HOUR;
    			state_change = true;
    		} else if (y >= 160 && y <= 180) {
    			prev_state = state;
    			state = TARGET_MIN;
    			state_change = true;
    		} else if(y >= 200 && y <= 220) {
    			if(x >= 150 && x <= 160) {
    				time_mode[1] = AM;
    			} else if(x >= 170 && x <= 180) {
    				time_mode[1] = PM;
    			} else if(x >= 190 && x <= 200) {
    				time_mode[1] = TFHR;
    			}
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
    	DrawText("AM  PM  24HR", 150, 100, 20, BLACK);
    	DrawText("Current time: ", 100, 200, 20, BLACK);
    	DrawText("AM  PM  24HR", 150, 200, 20, BLACK);
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

    	switch(time_mode[0]) {
    	case AM:
    		DrawRectangleLines(145, 95, 30, 30, DARKGRAY);
    		break;
    	case PM:
    		DrawRectangleLines(165, 95, 30, 30, DARKGRAY);
    		break;
    	case TFHR:
    		DrawRectangleLines(185, 95, 30, 30, DARKGRAY);
    		break;
    	}

    	switch(time_mode[1]) {
    	case AM:
    		DrawRectangleLines(145, 195, 30, 30, DARKGRAY);
    		break;
    	case PM:
    		DrawRectangleLines(165, 195, 30, 30, DARKGRAY);
    		break;
    	case TFHR:
    		DrawRectangleLines(185, 195, 30, 30, DARKGRAY);
    		break;
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
	if(difftime(mktime(&current_time_tm), current_time_t) > 0) {
		--current_time_tm.tm_mday;
	}
	last_increment = mktime(&current_time_tm);

	trigger_interval = 86400 - inputs[4] * 60; //seconds

	rewind(config_ptr);
	fwrite(&last_increment, sizeof(time_t), 1, config_ptr);
	fwrite(&trigger_interval, sizeof(double), 1, config_ptr);
	fwrite(&target_time, sizeof(time_hr), 1, config_ptr);

	#ifdef DEBUG
	char s[256];
	snprintf(s, sizeof(s), "%d:%d, %d:%d, %f", current_time_tm.tm_hour, current_time_tm.tm_min, target_time.tm_hour, target_time.tm_min, trigger_interval);
	printWindow(s);
	#endif
}

void DrawHBLine(HBNumber_line line) {
	DrawLineEx((Vector2) { line.range.x, line.y }, (Vector2) { line.range.y, line.y }, 5.0, WHITE);
    for(int i = 0; i < line.ticks; i++) {
    	float x = i;
    	
    	DrawLineEx(
    		(Vector2) { (line.range.y - line.range.x) / ((float)line.ticks - 1) * x + line.range.x, line.y - 10.0 }, 
    		(Vector2) { (line.range.y - line.range.x) / ((float)line.ticks - 1) * x + line.range.x, line.y + 10.0 }, 
    		1.0, WHITE);
    }
    DrawTriangle(
    		(Vector2) { line.selected - 7.5, line.y - 20 },
    		(Vector2) { line.selected, line.y - 10 }, 
    		(Vector2) { line.selected + 7.5, line.y - 20 },
    		WHITE);
    DrawRectangle(line.selected - 7.5, line.y - 35, 15, 15, WHITE);
}

int nearestPositionLine(float x, HBNumber_line line) {
	return (int)((x - line.range.x) / (line.range.y - line.range.x) * (line.ticks - 1) + 0.5);
}

float getPosition(int tick, HBNumber_line line) {
	return (float)tick / (line.ticks - 1) * (line.range.y - line.range.x) + line.range.x;
}

void DrawClock(Clock clock) {
	DrawCircleV(clock.center, clock.radius, RAYWHITE);
	DrawCircleV(clock.center, 5.0, BLACK);

	Vector2 base;
	Vector2 tip;

	int mod = 2;
	if (clock.mode == TFHR) {
		mod = 1;
	}
	for(int i = 0; i < 24; i++) {
		if (i % mod != 0) {
			continue;
		}	
		base = (Vector2) { 
			clock.center.x + clock.radius * cos(getAngle(i, 24)) * 0.7,  
			clock.center.y + clock.radius * sin(getAngle(i, 24)) * 0.7 };
		tip = (Vector2) { 
			clock.center.x + clock.radius * cos(getAngle(i, 24)) * 0.8,  
			clock.center.y + clock.radius * sin(getAngle(i, 24)) * 0.8 };
		DrawLineEx(base, tip, clock.radius * 0.02, BLACK);	
	}
	for(int i = 0; i < 60; i++) {
		base = (Vector2) { 
			clock.center.x + clock.radius * cos(getAngle(i, 60)) * 0.9,  
			clock.center.y + clock.radius * sin(getAngle(i, 60)) * 0.9 };
		tip = (Vector2) { 
			clock.center.x + clock.radius * cos(getAngle(i, 60)) * 0.95,  
			clock.center.y + clock.radius * sin(getAngle(i, 60)) * 0.95 };
		DrawLineEx(base, tip, clock.radius * 0.02, BLACK);
	}

	tip = (Vector2) { 
		clock.center.x + clock.radius * 0.45 * cos(clock.hour), 
		clock.center.y + clock.radius * 0.45 * sin(clock.hour) };
	DrawLineEx(clock.center, tip, clock.radius * 0.1, BLACK);
	DrawCircleV(tip, 5.0, BLACK);

	//angle = clock.minute * 2 * PI / 60 - PI / 2;
	tip = (Vector2) { 
		clock.center.x + clock.radius * 0.85 * cos(clock.minute), 
		clock.center.y + clock.radius * 0.85 * sin(clock.minute) };
	DrawLineEx(clock.center, tip, clock.radius * 0.1, BLACK);
	DrawCircleV(tip, 5.0, BLACK);
}

int determineClockHandCollision(Vector2 point, Clock clock) {
	if (clock.hour > PI) {
		clock.hour -= 2.0f * PI;
	}
	if (clock.minute > PI) {
		clock.minute -= 2.0f * PI;
	}
	float point_angle = atan2((double)(point.y - clock.center.y), (double)(point.x - clock.center.x));
	float distance = (point.y - clock.center.y) * (point.y - clock.center.y) + (point.x - clock.center.x) * (point.x - clock.center.x);
	if (distance == 0) {
		return 1;
	}
	if (clock.radius * clock.radius * 0.9025f > distance && fabsf(clock.minute - point_angle) < PI / sqrt(distance + 1.0f)) {
		return 2;
	} 
	if (clock.radius * clock.radius * 0.25f > distance && fabsf(clock.hour - point_angle) < PI / sqrt(distance + 1.0f)) {
		return 1;
	}
	return 0;
}

int nearestPositionClock(float angle, float mod) {
	float ang = angle + PI / 2.0;
	if(ang < 0) {
		ang += 2.0 * PI;
	} else if (ang >= 2.0 * PI){
		ang -= 2.0 * PI;
	}
	return (int)(ang * mod / 2.0 / PI + 0.5);
}

float getAngle(int position, float mod) {
	return position * 2.0f * PI / mod - PI / 2.0f;
}

float floatClamp(float value, float min, float max) {
	if(value < min) {
		return min;
	}
	if (value > max) {
		return max;
	}
	return value;
}

int intClamp(int value, int min, int max) {
	if(value < min) {
		return min;
	}
	if (value > max) {
		return max;
	}
	return value;
}

void printWindow(char *fmt, ...) {
	va_list ap, copy;

	va_start(ap, fmt);
	va_copy(copy, ap);
	int length = vsnprintf(NULL, 0, fmt, copy);
	va_end(copy);
	if(length < 0) {
		va_end(ap);
		return;
	}

	char *str = malloc((size_t)(length + 1) * sizeof(char));
	if(str == NULL) {
		va_end(ap);
		return;
	}
	vsprintf(str, fmt, ap);
	va_end(ap);

	InitWindow(500, 40, "Message from EepyAlarm");
	SetTargetFPS(60);

	Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetWindowSize(MeasureText(str, 20) + 20, 40);
	
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText(str, 10, 10, 20, BLACK);
		EndDrawing();
	}	
	CloseWindow();

	free(str);
}

