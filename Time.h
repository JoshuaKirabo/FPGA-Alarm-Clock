#ifndef TIME_H
#define TIME_H

typedef struct
	{
		int hour;
		int minute;
		int second;
	} Time;

typedef struct
	{
		int hourTens;
		int hourOnes;
		int minuteTens;
		int minuteOnes;
		int secondTens;
		int secondOnes;
	} TimeOnBCD;

void fillTimeStruct(Time* time);
void fillTimeOnBCDStruct(Time* time);

#endif // TIME_H
