#include "ClockApp.h"
#include "Time.h"

#include <stdio.h>

// Global variable to store the current state of the Clock App
enum state state = HomeScreen;

// Global variable to represent the current time
TimeOfDay currentTime;

void initializeClockApp()
	{
		// Initialize Clock App and set default time
		currentTime.hour = 12;
		currentTime.minute = 0;
		currentTime.second = 0;
	}



void changeTime() {
    // Implementation for changing the current time
    printf("Changing the current time...\n");
}

void displayHomeScreen() {
    // Implementation for displaying the home screen
    printf("Displaying Home Screen...\n");
}

void displayOptionsMenu() {
    // Implementation for displaying the options menu
    printf("Displaying Options Menu...\n");
}

void displayEditAlarmMenu() {
    // Implementation for displaying the edit alarm menu
    printf("Displaying Edit Alarm Menu...\n");
}

void displayChangeTimeMenu() {
    // Implementation for displaying the change time menu
    printf("Displaying Change Time Menu...\n");
}

int convertToBCD(int decimalValue)
	{
		return ((decimalValue / 10) << 4) | (decimalValue % 10);
	}

void sendTimeToHardware()
	{
		Time currentTime;
		fillTimeStruct(&currentTime); // Get current time

		// Assuming BCD conversion and sending to hardware is implemented here.
		// Convert hours, minutes, and seconds to BCD
		int bcdHour = convertToBCD(currentTime.hour);
		int bcdMinute = convertToBCD(currentTime.minute);
		int bcdSecond = convertToBCD(currentTime.second);

		// Send BCD values to hardware
		writeToGPIO(bcdHour, bcdMinute, bcdSecond);
	}
