#pragma once

struct Vector2 {
    float x;
    float y;
};

typedef enum {
	TARGET_HOUR,
	TARGET_MIN,
	CURRENT_HOUR,
	CURRENT_MIN,
	INCREMENT,
	IDLE
} User_state;

typedef enum {
	AM,
	PM,
	TFHR
} Time_mode;

typedef struct {
	struct Vector2 center;
	float radius;
	float hour;
	float minute;
	Time_mode mode;
} Clock;

typedef struct {
	struct Vector2 range;
	int y;
	int ticks;
	int selected;
} HBNumber_line;

void printWindow(char *fmt, ...);

void openConfigWindow(wchar_t* config_path, bool prevAvailable);

//Only run in a valid raylib window
void DrawHBLine(HBNumber_line line);

//Only run in a valid raylib window
void DrawClock(Clock clock);

int determineClockHandCollision(struct Vector2 point, Clock clock);

int nearestPosition(float angle, float mod);

float getAngle(int position, float mod);

void alarm();
