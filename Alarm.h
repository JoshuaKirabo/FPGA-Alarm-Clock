#ifndef ALARM_H
#define ALARM_H
#include <stdio.h>

#define MAX_ALARMS 3

// Storing Alarm info
typedef struct
	{
	    int hour;
	    int minute;
	    char AMorPM[3]; // Adding AM/PM field
	    char label[256]; // Label for the alarm
	} Alarm;

Alarm alarms[MAX_ALARMS];  // External declaration
int num_alarms;            // External declaration of the alarm counter

typedef enum
	{
	    SetAlarmHour,
	    SetAlarmMinute,
	    SetAlarmAMPM
	} SetAlarmState;


// Setting an Alarm
void checkAndCreateAlarmFile();
void writeAlarmToFile(Alarm alarm);
int countAlarms();
int setAlarm(int alarmHour, int alarmMinute, const char *AMorPM);
void editExistingAlarm(int alarmIndex, int newHour, int newMinute, int newSecond);
void deleteExistingAlarm(int alarmIndex);
void activateAlarm(int alarmIndex);
void deactivateAlarm(int alarmIndex);

#endif // CLOCKAPP_H
