#include <stdio.h>
#include <time.h>

int main(void) {
	time_t current_time_t;
	time(&current_time_t);
	struct tm current_time_tm;
	current_time_tm = *localtime(&current_time_t);

	printf("%d", current_time_tm.tm_min);

	return 0;
}