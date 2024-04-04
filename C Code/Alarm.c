#include "Alarm.h"
#include <stdio.h>
#include <time.h>

void checkAndCreateAlarmFile()
	{
		FILE *file;
		if ((file = fopen("alarms.txt", "r")))
			{
				// When the File exists
				fclose(file);
			}
		else
			{
				// File doesn't exist, create it
				file = fopen("alarms.txt", "w");
				if (file)
					{
						printf("alarms.txt created.\n");
						fclose(file);
					}
				else
					{
						printf("Failed to create alarms.txt.\n");
					}
			}
	}

void writeAlarmToFile(Alarm alarm)
	{
		FILE *file = fopen("alarms.txt", "a"); // Open for appending, to not overwrite existing content
		if (file)
			{
				fprintf(file, "%02d:%02d:%02d\n", alarm.hour, alarm.minute, alarm.second);
				fclose(file);
			}
		else
			{
				printf("Failed to open alarms.txt for writing.\n");
			}
	}

int countAlarms()
	{
		FILE *file = fopen("alarms.txt", "r");
		if (!file)
			{
				// If the file doesn't exist, there are no alarms set
				return 0;
			}

		int count = 0;
		int hour, minute, second;
		while (fscanf(file, "%d:%d:%d\n", &hour, &minute, &second) == 3)
			{
				count++;
			}
		fclose(file);

		return count;
	}

int setAlarm(int hour, int minute, int second)
	{
		int currentAlarms = countAlarms();
		if (currentAlarms >= MAX_ALARMS)
			{
				printf("Alarms full\n");
				return 0; // Indicate failure or "Alarms full"
			}

		FILE *file = fopen("alarms.txt", "a"); // Append mode
		if (!file)
			{
				printf("Failed to open alarms.txt\n");
				return 0; // Indicate failure
			}

		fprintf(file, "%02d:%02d:%02d\n", hour, minute, second);
		fclose(file);
		printf("New alarm set for %02d:%02d:%02d\n", hour, minute, second);

		return 1; // Indicate success
	}


void editAlarm()
	{
		// Open the txt file, and edit the chosen alarm
	}

void deleteAlarm()
	{
		// remove the alarm form where it is being stored
	}
