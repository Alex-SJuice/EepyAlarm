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
#include "my_math.h"

bool current_tfhr;
bool target_tfhr;
Font font;

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
			DrawText(message, GetScreenWidth() / 2 - MeasureText(message, 100) / 2, 100 , 100, (Color){ 255, 255, 255, a1 });
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
	Clock current =  { { 180, 180 }, 120.0, getAngle(0, 12.0f), getAngle(0, 60.0f), AM };
    Clock target = { { 180, 520 }, 120.0, getAngle(0, 12.0f), getAngle(0, 60.0f), AM };
    HBNumber_line increment = { { 330.0, 655.0 }, 530, 100 , 0.0 };
    increment.selected = getPosition(0, increment);
    Rectangle preview = { 430.0, 410.0, 100.0, 40.0 };

	int inputs[5] = {0};

    if(prevAvailable) {
    	inputs[0] = target_time.tm_hour;
    	inputs[1] = target_time.tm_min;
    	if(target_tfhr){
    		target.mode = TFHR;
	    	target.hour = getAngle(target_time.tm_hour, 24);	
	    } else {
	    	if(target_time.tm_hour > 12) { 
		    	target.mode = PM;
		    }
	    	target.hour = getAngle(target_time.tm_hour % 12, 12);
	    }
	    target.minute = getAngle(target_time.tm_min, 60);

    	struct tm current_time = *localtime(&last_increment);
    	inputs[2] = current_time.tm_hour;
    	inputs[3] = current_time.tm_min;
    	if(current_tfhr){
    		current.mode = TFHR;
	    	current.hour = getAngle(current_time.tm_hour, 24);	
	    } else {
	    	if(current_time.tm_hour > 12) { 
		    	current.mode = PM;
		    }
	    	current.hour = getAngle(current_time.tm_hour % 12, 12);
	    }
	    current.minute = getAngle(current_time.tm_min, 60);

    	inputs[4] = (86400 - trigger_interval) / 60;
    	increment.selected = inputs[4];
    }

    User_state state = IDLE;
    bool change = false;

	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(700, 700, "EepyAlarm Configuration");
    SetTargetFPS(60);
    

    Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);
    Texture2D icon_texture = LoadTextureFromImage(icon);

    font = LoadFont("ShareTech-Regular.ttf");
    SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);

    while (!WindowShouldClose()) {
    	char s[256];
		snprintf(s, sizeof(s), "%d", inputs[4]);

		bool hovering = false;
		if (CheckCollisionPointRec((Vector2) { GetMouseX(), GetMouseY() }, preview)) {
			hovering = true;
		}
		if (IsMouseButtonPressed(0)) {
			if (hovering) {
				UnloadFont(font);
				UnloadTexture(icon_texture);
				UnloadImage(icon);
    			CloseWindow();
				alarm();
				SetConfigFlags(FLAG_MSAA_4X_HINT);
				InitWindow(700, 700, "EepyAlarm Configuration");
			    SetTargetFPS(60);
			    icon = LoadImage("icon.png");
			    SetWindowIcon(icon);
			    icon_texture = LoadTextureFromImage(icon);
			    font = LoadFont("ShareTech-Regular.ttf");
			    SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
			}
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
				if(CheckCollisionPointRec((Vector2) { GetMouseX(), GetMouseY() }, (Rectangle) { 
					increment.range.x + (increment.range.y - increment.range.x) / 6.0, 
					increment.y - 10.0, (increment.range.y - increment.range.x) * 2.0 / 3.0, 50.0 })) {
					state = INCREMENT;
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
				inputs[4] = (int)(increment.selected + 0.5);
				increment.selected = (int)(increment.selected + 0.5);
				
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
			increment.selected -= GetMouseDelta().x / 45.0;
			if(increment.selected < 0.0) {
				increment.selected += increment.ticks;
			} else if(increment.selected > increment.ticks) {
				increment.selected -= increment.ticks;
			}
		default:
			break;
		}
		
		BeginDrawing();		
    	ClearBackground((Color) { 0, 29, 53, 255 });

    	DrawTextEx(font, "Current Time", (Vector2) { 110.0, 15.0 }, 30, 1, WHITE);
    	DrawTextEx(font, "Target Time", (Vector2) { 115.0, 655.0 }, 30, 1, WHITE);
    	DrawTextEx(font, "Increment", (Vector2) { 440.0, 590.0 }, 30, 1, WHITE);
    	DrawTextEx(font, "Alarm", (Vector2) { 320.0, 360.0 }, 30, 1, WHITE);
    	DrawTextEx(font, "Default Alarm tm r cc", (Vector2) { 400.0, 360.0 }, 30, 1, WHITE);
    	DrawTextPro(font, "EepyAlarm", (Vector2) { 320.0, 60.0 }, (Vector2) { 0.0, 0.0 }, 0, 50, 1, WHITE);
    	DrawTextureEx(icon_texture, (Vector2) { 400.0, 60.0 }, 0.0, 0.5, WHITE);

    	DrawRectangleRec(preview, (Color) { 255, 255, 255, hovering ? 255 : 200});
    	DrawLineEx(
    		(Vector2) { preview.x - 2.5, preview.y }, 
    		(Vector2) { preview.x - 2.5, preview.y + preview.height }, 5, 
    		(Color) { 255, 255, 255, hovering ? 255 : 200});
    	DrawLineEx(
    		(Vector2) { preview.x, preview.y + preview.height + 2.5 }, 
    		(Vector2) { preview.x + preview.width, preview.y + preview.height + 2.5 }, 5, 
    		(Color) { 255, 255, 255, hovering ? 255 : 200});
    	DrawLineEx(
    		(Vector2) { preview.x, preview.y - 2.5 }, 
    		(Vector2) { preview.x + preview.width, preview.y - 2.5 }, 5, 
    		(Color) { 255, 255, 255, hovering ? 255 : 200});
    	DrawLineEx(
    		(Vector2) { preview.x + preview.width + 2.5, preview.y }, 
    		(Vector2) { preview.x + preview.width + 2.5, preview.y + preview.height }, 5, 
    		(Color) { 255, 255, 255, hovering ? 255 : 200});
    	DrawCircleSector((Vector2) { preview.x, preview.y }, 5, 180, 270, 10, (Color) { 255, 255, 255, hovering ? 255 : 200});
    	DrawCircleSector((Vector2) { preview.x + preview.width, preview.y }, 5, 270, 360, 10, (Color) { 255, 255, 255, hovering ? 255 : 200});
    	DrawCircleSector((Vector2) { preview.x, preview.y + preview.height }, 5, 90, 180, 10, (Color) { 255, 255, 255, hovering ? 255 : 200});
    	DrawCircleSector((Vector2) { preview.x + preview.width, preview.y + preview.height }, 5, 0, 90, 10, (Color) { 255, 255, 255, hovering ? 255 : 200});
    	DrawTextEx(font, "Preview", (Vector2) { preview.x + 12.0, preview.y + 8.0 }, 25, 1, (Color) { 0, 29, 53, hovering ? 255 : 200 });

    	DrawTextEx(font, "AM", (Vector2) { 20.0, 235.0 }, 45, 1, (Color) { 255, 255, 255, current.mode == AM ? 255 : 100 });
    	DrawTextEx(font, "PM", (Vector2) { 60.0, 280.0 }, 45, 1, (Color) { 255, 255, 255, current.mode == PM ? 255 : 100 });
    	DrawTextEx(font, "24HR", (Vector2) { 120.0, 300.0 }, 45, 1, (Color) { 255, 255, 255, current.mode == TFHR ? 255 : 100 });

    	DrawTextEx(font, "AM", (Vector2) { 20.0, 420.0 }, 45, 1, (Color) { 255, 255, 255, target.mode == AM ? 255 : 100 });
    	DrawTextEx(font, "PM", (Vector2) { 60.0, 375.0 }, 45, 1, (Color) { 255, 255, 255, target.mode == PM ? 255 : 100 });
    	DrawTextEx(font, "24HR", (Vector2) { 120.0, 355.0 }, 45, 1, (Color) { 255, 255, 255, target.mode == TFHR ? 255 : 100 });
    	
    	DrawClock(target);
    	DrawClock(current);
    	DrawHBLine(increment);

    	//DrawTextEx(font, s, (Vector2){ GetMouseX() + 10, GetMouseY() }, 20, 1, GREEN);
    	EndDrawing();
    }

    UnloadFont(font);
    UnloadTexture(icon_texture);
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

	bool current_tfhr = current.mode == TFHR;
	bool target_tfhr = current.mode == TFHR;
	dst = current_time_tm.tm_isdst != 0;

	rewind(config_ptr);
	fwrite(&last_increment,   sizeof(time_t), 1, config_ptr);
	fwrite(&trigger_interval, sizeof(double), 1, config_ptr);
	fwrite(&target_time,      sizeof(time_hr), 1, config_ptr);
	fwrite(&current_tfhr, 	  sizeof(bool), 1, config_ptr);
	fwrite(&target_tfhr, 	  sizeof(bool), 1, config_ptr);
	fwrite(&dst, 			  sizeof(bool), 1, config_ptr);


	#ifdef DEBUG
	char s[256];
	snprintf(s, sizeof(s), "%d:%d, %d:%d, %f", current_time_tm.tm_hour, current_time_tm.tm_min, target_time.tm_hour, target_time.tm_min, trigger_interval);
	printWindow(s);
	#endif
}

void DrawHBLine(HBNumber_line line) {
	DrawRectangle(
		line.range.x + (line.range.y - line.range.x) / 6.0, 
		line.y - 10.0, (line.range.y - line.range.x) * 2.0 / 3.0, 50.0, WHITE);
	DrawTriangle(
    		(Vector2) { line.range.x + (line.range.y - line.range.x) / 2 - 7.5, line.y - 20 },
    		(Vector2) { line.range.x + (line.range.y - line.range.x) / 2, line.y - 10 }, 
    		(Vector2) { line.range.x + (line.range.y - line.range.x) / 2 + 7.5, line.y - 20 },
    		WHITE);
	DrawTriangle(
			(Vector2) { line.range.x + (line.range.y - line.range.x) / 2, line.y + 40 }, 
    		(Vector2) { line.range.x + (line.range.y - line.range.x) / 2 - 7.5, line.y + 50 },
    		(Vector2) { line.range.x + (line.range.y - line.range.x) / 2 + 7.5, line.y + 50 },
    		WHITE);
	char num[3];
	for(int i = floorf(line.selected) - 2.0; i < floorf(line.selected) + 4.0; i++) {
		int index = i;
		if(i < 0) {
			index += line.ticks;
		} else if(i >= line.ticks) {
			index -= line.ticks;
		}
		float position = getPosition(index, line);

		snprintf(num, sizeof(num), "%d", index);
		DrawTextEx(font, num, 
			(Vector2) { 
				sin(position) * 700.0
				 + line.range.x + (line.range.y - line.range.x) / 2
				 - MeasureTextEx(font, num, 40.0, 1.0).x / 2.0, line.y - 5.0 },
			 40.0, 1.0, BLACK);
	}
	DrawRectangle(line.range.x, line.y - 10.0, (line.range.y - line.range.x) / 6.0, 50.0, (Color) { 0, 29, 53, 255 });
	DrawRectangle(
		line.range.x + (line.range.y - line.range.x) * 5.0 / 6.0, 
		line.y - 10.0, (line.range.y - line.range.x) / 6.0, 50.0, (Color) { 0, 29, 53, 255 });
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

	char *n = NULL;
	while((n = strchr(str, '\n')) != NULL) {
		*n = ' ';
	}

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

