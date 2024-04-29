#ifndef ClockFace_h
#define ClockFace_h

#include <stddef.h>
#include "Time.h"

typedef struct
	{
		int day;
		int month;
		int year;
	} Date;

void getCurrentDate(Date* date);
void getCurrentTime(Time* currentTime);
void getTimeOfDay(char* buffer, size_t bufferSize, const Time* time);
const char* getDayOfWeek(char* buffer, size_t bufferSize);
void getDate(char* buffer, size_t bufferSize);
const char* getMonth(char* buffer, size_t bufferSize);
void getYear(char* buffer, size_t bufferSize);

#endif // ClockFace_h

