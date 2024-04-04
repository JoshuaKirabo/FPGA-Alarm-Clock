#include "ClockFace.h"
#include <stdio.h>
#include <time.h>


void getCurrentDate(Date* date)
	{
		time_t rawtime;
		struct tm *timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		date->day = timeinfo->tm_mday;
		date->month = timeinfo->tm_mon + 1;
		date->year = timeinfo->tm_year + 1900;
	}

void getCurrentTime(Time* currentTime)
	{
		time_t rawtime;
		struct tm *timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		currentTime->hour = timeinfo->tm_hour;
		currentTime->minute = timeinfo->tm_min;
		currentTime->second = timeinfo->tm_sec;
	}


void getTimeOfDay(char* buffer, size_t bufferSize, const Time* time)
	{
		 const char* greeting;
			if (time->hour < 12)
				{
					greeting = "Good Morning";
				}
			else if (time->hour >= 12 && time->hour < 18)
				{
					greeting = "Good Afternoon";
				}
			else
				{
					greeting = "Good Evening";
				}

			snprintf(buffer, bufferSize, "%s", greeting);
	}


const char* getDayOfWeek(char* buffer, size_t bufferSize)
	{
		time_t rawtime;
		struct tm *timeinfo;

		// Array of day names
		const char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

		// Getting current time and localizing it
		time(&rawtime);
		timeinfo = localtime(&rawtime);

		// Generating a string to display in the LCD for day of the week
		if (timeinfo->tm_wday >= 0 && timeinfo->tm_wday < 7)
			{
				// Returning to the days buffer
				snprintf(buffer, bufferSize, "%s", days[timeinfo->tm_wday]);
			}
		else
			{
				// Error Handling
				return "Unknown";
			}

		return buffer;
	}


void getDate(char* buffer, size_t bufferSize)
	{
		time_t rawtime;
		struct tm *timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		int day = timeinfo->tm_mday;
		snprintf(buffer, bufferSize, "%d", day);
	}


const char* getMonth(char* buffer, size_t bufferSize)
	{
		time_t rawtime;
		struct tm *timeinfo;

		// Getting current time and localizing it
		time(&rawtime);
		timeinfo = localtime(&rawtime);

		const char* months[] = {"January", "February", "March", "April", "May", "June","July", "August", "September", "October", "November", "December"};

		// No need to add 1 when using tm_mon to index the months array
		if (timeinfo->tm_mon >= 0 && timeinfo->tm_mon < 12)
			{
				snprintf(buffer, bufferSize, "%s", months[timeinfo->tm_mon]);
			}
		else
			{
				snprintf(buffer, bufferSize, "%s", "Unknown"); // Error handling
			}

		return buffer;
	}

void getYear(char* buffer, size_t bufferSize)
	{
		time_t rawtime;
		struct tm *timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		int year = timeinfo->tm_year + 1900; // tm_year is years since 1900

		snprintf(buffer, bufferSize, "%d", year);
	}







