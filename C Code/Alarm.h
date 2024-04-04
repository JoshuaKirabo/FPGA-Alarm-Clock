#ifndef ALARM_H
#define ALARM_H

#define MAX_ALARMS 3

#include <stdio.h>

// Storing Alarm info
typedef struct
	{
	    int hour;
	    int minute;
	    int second;
	    char label[256];
	    int enabled;
	} Alarm;

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
int setAlarm(int hour, int minute, int second);
void editExistingAlarm(int alarmIndex, int newHour, int newMinute, int newSecond);
void deleteExistingAlarm(int alarmIndex);
void activateAlarm(int alarmIndex);
void deactivateAlarm(int alarmIndex);

#endif // CLOCKAPP_H
