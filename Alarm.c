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
				fprintf(file, "%02d:%02d:%02d\n", alarm.hour, alarm.minute);
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

		return num_alarms;
	}

void addAlarmToArray(int alarmHour, int alarmMinute) {
	printf("Num Alarms: %d",num_alarms);
    if (num_alarms < MAX_ALARMS)
    {

        alarms[num_alarms].hour = alarmHour;
        alarms[num_alarms].minute = alarmMinute;
        printf("Your alarm has been set for %d%d",alarms[num_alarms].hour ,alarms[num_alarms].minute);
        num_alarms++;
    }
    else {
        printf("Alarm array is full.\n");
    }
}

int setAlarm(int alarmHour, int alarmMinute, const char *AMorPM) {
	printf("Setting Alarm\n");
    // Convert 12-hour time to 24-hour time based on AM/PM
    if (AMorPM[0] == 'P' && alarmHour != 12) {
        alarmHour += 12;
    } else if (AMorPM[0] == 'A' && alarmHour == 12) {
        alarmHour = 0;
    }

    // Add the converted time to the global array
    addAlarmToArray(alarmHour, alarmMinute);

    // Print success message
    printf("New alarm set for %02d:%02d\n", alarmHour, alarmMinute);
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
