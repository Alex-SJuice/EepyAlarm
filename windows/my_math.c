#include <math.h>
#include "my_math.h"
#include "rendering.h"

float getAngle(int position, float mod) {
	return position * 2.0f * M_PI / mod - M_PI / 2.0f;
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


int determineClockHandCollision(struct Vector2 point, Clock clock) {
	if (clock.hour > M_PI) {
		clock.hour -= 2.0f * M_PI;
	}
	if (clock.minute > M_PI) {
		clock.minute -= 2.0f * M_PI;
	}
	float point_angle = atan2((double)(point.y - clock.center.y), (double)(point.x - clock.center.x));
	float distance = (point.y - clock.center.y) * (point.y - clock.center.y) + (point.x - clock.center.x) * (point.x - clock.center.x);
	if (distance == 0) {
		return 1;
	}
	if (clock.radius * clock.radius * 0.9025f > distance && fabsf(clock.minute - point_angle) < M_PI / sqrt(distance + 1.0f)) {
		return 2;
	} 
	if (clock.radius * clock.radius * 0.25f > distance && fabsf(clock.hour - point_angle) < M_PI / sqrt(distance + 1.0f)) {
		return 1;
	}
	return 0;
}

int nearestPositionClock(float angle, float mod) {
	float ang = angle + M_PI / 2.0;
	if(ang < 0) {
		ang += 2.0 * M_PI;
	} else if (ang >= 2.0 * M_PI){
		ang -= 2.0 * M_PI;
	}
	return (int)(ang * mod / 2.0 / M_PI + 0.5);
}

int nearestPositionLine(float angle, HBNumber_line line) {
	float position = angle * line.ticks / 2.0 / M_PI;
	return (int)(position + 0.5);
}

//now returns angle on imaginary cylinder
float getPosition(int tick, HBNumber_line line) {
	float angle = (tick - line.selected) * 2.0 * M_PI / line.ticks;
	if(angle < -M_PI / 2.0) {
		angle += 2.0 * M_PI;
	} else if (angle >= M_PI / 2.0){
		angle -= 2.0 * M_PI;
	}
	return angle;
}
