#ifndef CLOCKAPP_H
#define CLOCKAPP_H

#define MAX_ALARMS 3

// Enum for Clock App states
enum state
	{
		HomeScreen,
		OptionsMenu,
		EditAlarmMenu,
		ChangeTime,
		SetAlarm,
	};

// Structure to represent time
typedef struct
	{
		int hour;
		int minute;
		int second;
	} TimeOfDay;

extern enum state state;

// Function declarations
void initializeClockApp();  // Initialize Clock App
void changeTime();         // Change the current time
void displayHomeScreen();  // Display the home screen
void displayOptionsMenu(); // Display the options menu
void displayEditAlarmMenu();// Display the edit alarm menu
void displayChangeTimeMenu();// Display the change time menu
void handleButtonPress(int buttonValue); // Handle button press events
void sendTimeToHardware();


#endif // CLOCKAPP_H
