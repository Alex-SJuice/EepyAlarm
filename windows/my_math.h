#pragma once
#include "rendering.h"

float getAngle(int position, float mod);

float floatClamp(float value, float min, float max);

int intClamp(int value, int min, int max);


int determineClockHandCollision(struct Vector2 point, Clock clock);

int nearestPositionClock(float angle, float mod);

int nearestPositionLine(float x, HBNumber_line line);

float getPosition(int tick, HBNumber_line line);
