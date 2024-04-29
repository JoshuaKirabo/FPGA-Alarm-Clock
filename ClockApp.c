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




