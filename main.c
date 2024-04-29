#include "terasic_os_includes.h"
#include "LCD_Lib.h"
#include "lcd_graphic.h"
#include "font.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <math.h>


#include "./address_map_arm.h"
#include "./ClockFace.h"
#include "./Hardware.h"
#include "./ClockApp.h"
#include "./Alarm.h"


#define DEBOUNCE_TIME 100
#define DOT_THRESHOLD 100
#define DASH_THRESHOLD 300

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

volatile unsigned int *JP1_ptr;



int highlightedOption;

// Use sudo date 042115002024 to adjust the date on the board accordingly
int extractBit(int number, int bitPosition) {
    return (number >> bitPosition) & 0x1;  // Shift right by 'bitPosition' and mask with 1
}
int convertTimeToNumber(int hour, int minute, int second) {
    if (hour == 0) {  // Midnight case
        hour = 12;
    } else if (hour > 12) {  // Convert PM hours to 12-hour format
        hour -= 12;
    }

    // Print the time in a simpler format, omitting AM/PM
    printf("%d:%02d:%02d", hour, minute, second);

    // Return the time as a number in HHMMSS format
    return hour * 10000 + minute * 100 + second;
}

int main()
	{
		// Initialize Value
		void *LW_virtual;
		void *virtual_base;

		int fd;


		// Mapping the Peripherals
		//JP1_ptr = (unsigned int*)(LW_virtual + JP1_BASE);   //
		//*(JP1_ptr + 1) = 0x0000000F;

		// Initializing Buffers for different display elements
		char greetingBuffer[20];
		char dayOfWeekBuffer[20];
		char monthBuffer[20];
		char dateBuffer[10];
		char yearBuffer[10];
		char timeOfDayBuffer[20];
		Time currentTime;

		// Ensuring that the alarm file is being created
		checkAndCreateAlarmFile();

		// When the user goes to change their alarm, the hour section will be selected
		SetAlarmState setAlarmState = SetAlarmHour;

		// Initializing the alarm to default values
		int alarmHour = 12;
		int alarmMinute = 00;
		char AMorPM[3] = "AM";


		// Populate the current time
		getCurrentTime(&currentTime);


		int lastSecond = -1;

		// Populate buffers with formatted date and time information
		getTimeOfDay(timeOfDayBuffer, sizeof(timeOfDayBuffer), &currentTime);
		getDayOfWeek(dayOfWeekBuffer, sizeof(dayOfWeekBuffer));
		getDate(dateBuffer, sizeof(dateBuffer));
		getMonth(monthBuffer, sizeof(monthBuffer));
		getYear(yearBuffer, sizeof(yearBuffer));
		getTimeOfDay(greetingBuffer, sizeof(greetingBuffer), &currentTime);

		// Set highlighted Option to 1 so that the selector can begin from 1
		highlightedOption = 1;

		// Buffer to hold the concatenated string
		char monthDate[60];

		    // Concatenate the strings into monthDate
		    // Adjust the format as needed to include spaces or commas
		    snprintf(monthDate, sizeof(monthDate), "%s, %s", monthBuffer, dateBuffer);

				if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 )
					{
						printf( "ERROR: could not open \"/dev/mem\"...\n" );
						return( 1 );
					}

				if ((LW_virtual = map_physical (fd, LW_BRIDGE_BASE, LW_BRIDGE_SPAN)) == NULL)
					{
						printf( "ERROR: mmap() failed...\n" );
						close( fd );
						return( 1 );
					}

				if ((virtual_base = map_physical (fd, HW_REGS_BASE, HW_REGS_SPAN)) == NULL)
					{
						printf( "ERROR: mmap() failed...\n" );
						close( fd );
						return( 1 );
					}

				JP1_ptr = (unsigned int*)(LW_virtual + JP1_BASE);   //
				*(JP1_ptr + 1) = 0x0000000F;



				LCD_CANVAS LcdCanvas;
				LcdCanvas.Width = LCD_WIDTH;
				LcdCanvas.Height = LCD_HEIGHT;
				LcdCanvas.BitPerPixel = 1;
				LcdCanvas.FrameSize = LcdCanvas.Width * LcdCanvas.Height / 8;
				LcdCanvas.pFrame = (void *)malloc(LcdCanvas.FrameSize);

				if (LcdCanvas.pFrame == NULL)
					{
						printf("failed to allocate lcd frame buffer\r\n");
					}
				else
					{
						LCDHW_Init(virtual_base);
						LCDHW_BackLight(true); // turn on LCD backlight

						LCD_Init();

						// clear screen
						DRAW_Clear(&LcdCanvas, LCD_WHITE);

						// demo grphic api
						 // retangle
						//DRAW_Circle(&LcdCanvas, 10, 10, 6, LCD_BLACK);
						//DRAW_Circle(&LcdCanvas, LcdCanvas.Width-10, 10, 6, LCD_BLACK);
						//DRAW_Circle(&LcdCanvas, LcdCanvas.Width-10, LcdCanvas.Height-10, 6, LCD_BLACK);
						//DRAW_Circle(&LcdCanvas, 10, LcdCanvas.Height-10, 6, LCD_BLACK);



						// demo font
						DRAW_Refresh(&LcdCanvas);

						free(LcdCanvas.pFrame);
					}

				// gpio1_addr = (volatile unsigned int *) virtual_base;

				state = HomeScreen;


					DRAW_Clear(&LcdCanvas, LCD_WHITE);
					DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
					DRAW_PrintString(&LcdCanvas, 20, 5, greetingBuffer, LCD_BLACK, &font_16x16);
					DRAW_PrintString(&LcdCanvas, 30, 5+25, dayOfWeekBuffer, LCD_BLACK, &font_16x16);
					DRAW_PrintString(&LcdCanvas, 25, 5+35, monthDate, LCD_BLACK, &font_16x16);
					DRAW_PrintString(&LcdCanvas, 37, 5+45, yearBuffer, LCD_BLACK, &font_16x16);
					DRAW_Refresh(&LcdCanvas);


					volatile int *KEY_ptr = (unsigned int*)(LW_virtual + KEY_BASE);

					int prev_key_value = *KEY_ptr;
					int buttonValue = *KEY_ptr;

			 while(1)
				 {
				 	 getCurrentTime(&currentTime);
				 	//printf("I am always running");
					buttonValue = *KEY_ptr;

					if (currentTime.second != lastSecond)
						{
							lastSecond = currentTime.second;

							int hour = currentTime.hour;
							int minute = currentTime.minute;
							int second = currentTime.second;

							  for (int i = 0; i < 3; i++)
								  {
								  	  printf("Alarm : %02d:%02d\n", alarms[i].hour, alarms[i].minute);
								  	  printf(" Number of alarms: %d, %d, %d\n", alarms[0],alarms[1],alarms[2]);
									  if (alarms[i].hour == currentTime.hour && alarms[i].minute == currentTime.minute)
										  {
											   printf("Alarm Ringing: %02d:%02d\n", alarms[i].hour, alarms[i].minute);
											   // Add code to perform the action when the alarm triggers, such as playing a sound
										  }
								  }

							//if current time is found in alarm array()
							//{
								// Print..Alarm Ringing
							//}

							/*// Separating the different fragments of time
					        int hour_high = currentTime.hour / 10; // Write this calue to GPIO[0] - FPGA Pin No is PIN_W15
					        int hour_low = currentTime.hour % 10; // Write this calue to GPIO[1] - FPGA Pin No is PIN_AK2
					        int minute_high = currentTime.minute / 10; // Write this calue to GPIO[2] - FPGA Pin No is PIN_Y16
					        int minute_low = currentTime.minute % 10; // Write this calue to GPIO[3] - FPGA Pin No is PIN_AK3
					        int second_high = currentTime.second / 10; // Write this calue to GPIO[4] - FPGA Pin No is PIN_AJ1
					        int second_low = currentTime.second % 10; // Write this calue to GPIO[5] - FPGA Pin No is PIN_AJ2

					        int hour_high_bit1 = extractBit(hour_high, 3);
					        int hour_high_bit2 = extractBit(hour_high, 2);
					        int hour_high_bit3 = extractBit(hour_high, 1);
					        int hour_high_bit4 = extractBit(hour_high, 0);

					        int hour_low_bit1 = extractBit(hour_low, 3);
					       	int hour_low_bit2 = extractBit(hour_low, 2);
					       	int hour_low_bit3 = extractBit(hour_low, 1);
					       	int hour_low_bit4 = extractBit(hour_low, 0);

					       	int minute_high_bit1 = extractBit(minute_high, 3);
					       	int minute_high_bit2 = extractBit(minute_high, 2);
					       	int minute_high_bit3 = extractBit(minute_high, 1);
					       	int minute_high_bit4 = extractBit(minute_high, 0);

					       	int minute_low_bit1 = extractBit(minute_low, 3);
					       	int minute_low_bit2 = extractBit(minute_low, 2);
					       	int minute_low_bit3 = extractBit(minute_low, 1);
					      	int minute_low_bit4 = extractBit(minute_low, 0);

					      	int second_high_bit1 = extractBit(second_high, 3);
					      	int second_high_bit2 = extractBit(second_high, 2);
					      	int second_high_bit3 = extractBit(second_high, 1);
					      	int second_high_bit4 = extractBit(second_high, 0);

					      	int second_low_bit1 = extractBit(second_low, 3);
					        int second_low_bit2 = extractBit(second_low, 2);
					      	int second_low_bit3 = extractBit(second_low, 1);
					      	int second_low_bit4 = extractBit(second_low, 0);

					        // Assuming you have a way to directly write these BCD values to a hardware register mapped in memory
					        // Let's define these hypothetical registers, assuming one register per digit:

					        JP1_ptr[0] = hour_high_bit1;   // PIN_W15
					        JP1_ptr[1] = hour_high_bit2;   // PIN_AK2
					        JP1_ptr[2] = hour_high_bit3;   // PIN_Y16
					        JP1_ptr[3] = hour_high_bit4;   // PIN_AK3

					        JP1_ptr[4] = hour_low_bit1;    // PIN_AJ1
					        JP1_ptr[5] = hour_low_bit2;    // PIN_AJ2
					        JP1_ptr[6] = hour_low_bit3;    // PIN_AH2
					        JP1_ptr[7] = hour_low_bit4;    // PIN_AH3

					        JP1_ptr[8] = minute_high_bit1;  // PIN_AH4
					        JP1_ptr[9] = minute_high_bit2;  // PIN_AH5
					        JP1_ptr[10] = minute_high_bit3; // PIN_AG1
					        JP1_ptr[11] = minute_high_bit4; // PIN_AG2

					        JP1_ptr[12] = minute_low_bit1;  // PIN_AG3
					        JP1_ptr[13] = minute_low_bit2;  // PIN_AG5
					        JP1_ptr[14] = minute_low_bit3;  // PIN_AG6
					        JP1_ptr[15] = minute_low_bit4;  // PIN_AG7

					        JP1_ptr[16] = second_high_bit1; // PIN_AG8
					        JP1_ptr[17] = second_high_bit2; // PIN_AF4
					        JP1_ptr[18] = second_high_bit3; // PIN_AF5
					        JP1_ptr[19] = second_high_bit4; // PIN_AF6

					        JP1_ptr[20] = second_low_bit1;  // PIN_AF8
					        JP1_ptr[21] = second_low_bit2;  // PIN_AF9
					        JP1_ptr[22] = second_low_bit3; 	// PIN_AF10
					        JP1_ptr[23] = second_low_bit4;  // PIN_AE7*/

							 // Output to Hex and LEDR
							// printf("The time is=%f m/s^2\n", currentTime);
							// *JP1_ptr = currentTime;

							 int timeNumber = convertTimeToNumber(hour, minute, second);
							 printf("The time is: %d\n", timeNumber);
							 *JP1_ptr = convertTimeToNumber(hour, minute, second);

					    }


					// State Change
					if(buttonValue!= prev_key_value)
						{
							if(buttonValue == 8)
								{
									printf("UP\n");

									if(state == SetAlarm)
										{
											if(setAlarmState == 1)
												{
													printf("Current alarm state(UP): %d\n", setAlarmState);

													//alarmHour = (alarmHour + 1 + 24) % 24;
													alarmHour = (alarmHour + 1) % 12;
													if (alarmHour == 0) alarmHour = 12; // Adjust for 12-hour format

													// Update the display
													DRAW_Clear(&LcdCanvas, LCD_WHITE);
													DRAW_Rect(&LcdCanvas, 30,42, LcdCanvas.Width-82, LcdCanvas.Height-10, LCD_BLACK);
												    DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													DRAW_PrintString(&LcdCanvas, 30, 5, "Set Alarm", LCD_BLACK, &font_16x16);
													char buffer[20];
													snprintf(buffer, sizeof(buffer), "%02d:%02d %s", alarmHour, alarmMinute, AMorPM);
													DRAW_PrintString(&LcdCanvas, 30, 40, buffer, LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);
												}
											else if(setAlarmState == 2)
												{
													alarmMinute = (alarmMinute + 1 + 60) % 60;

													printf("Current alarm state: %d\n", setAlarmState);
													DRAW_Clear(&LcdCanvas, LCD_WHITE);
													DRAW_Rect(&LcdCanvas, 53,42, LcdCanvas.Width-58, LcdCanvas.Height-10, LCD_BLACK);
													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													DRAW_PrintString(&LcdCanvas, 30, 5, "Set Alarm", LCD_BLACK, &font_16x16);
													char buffer[20];
													snprintf(buffer, sizeof(buffer), "%02d:%02d %s", alarmHour, alarmMinute, AMorPM);
													DRAW_PrintString(&LcdCanvas, 30, 40, buffer, LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);
												}
											else if(setAlarmState == 3)
												{
													if (strcmp(AMorPM, "AM") == 0)
														{
												        	strcpy(AMorPM, "PM");
														}
													else
														{
															strcpy(AMorPM, "AM");
														}

													DRAW_Clear(&LcdCanvas, LCD_WHITE);
													DRAW_Rect(&LcdCanvas, 77,40, LcdCanvas.Width-30, LcdCanvas.Height-10, LCD_BLACK);
													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													DRAW_PrintString(&LcdCanvas, 30, 5, "Set Alarm", LCD_BLACK, &font_16x16);
													char buffer[20];
													snprintf(buffer, sizeof(buffer), "%02d:%02d %s", alarmHour, alarmMinute, AMorPM);
													DRAW_PrintString(&LcdCanvas, 30, 40, buffer, LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);
												}
										}
									else if(state == OptionsMenu)
										{
											if(highlightedOption == 3)
												{
													printf("Option2\n");
													DRAW_Clear(&LcdCanvas, LCD_WHITE);

													// Selecting Rectangle
													DRAW_Rect(&LcdCanvas, 25,35, LcdCanvas.Width-11, LcdCanvas.Height-17, LCD_BLACK);
													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													DRAW_PrintString(&LcdCanvas, 02, 5, "Select Option", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+16, "1.Set Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+28, "2.Delete Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+42, "3.Change Time", LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);

													//Highlight list 2
													highlightedOption = 2;
												}
											else if(highlightedOption == 1)
												{
													printf("Option3\n");

													// Highlight Option 3
													highlightedOption = 3;

													DRAW_Clear(&LcdCanvas, LCD_WHITE);

													// Selecting Rectangle
													DRAW_Rect(&LcdCanvas, 25,48, LcdCanvas.Width-11, LcdCanvas.Height-4, LCD_BLACK);

													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													DRAW_PrintString(&LcdCanvas, 02, 5, "Select Option", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+16, "1.Set Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+28, "2.Delete Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+42, "3.Change Time", LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);
												}
											else if(highlightedOption == 2)
												{
													printf("Option1\n");
													DRAW_Clear(&LcdCanvas, LCD_WHITE);

													// Selecting Rectangle
													DRAW_Rect(&LcdCanvas, 25,23, LcdCanvas.Width-11, LcdCanvas.Height-29, LCD_BLACK);

													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													DRAW_PrintString(&LcdCanvas, 02, 5, "Select Option", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+16, "1.Set Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+28, "2.Delete Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+42, "3.Change Time", LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);

													// Highlight Option 1
													highlightedOption = 1;
												}

										}

								}
							else if(buttonValue == 4)
								{
									// When the user presses the "Down" Button fort the first time, they will be presented with a highlighted option
									printf("DOWN\n");

									if(state == OptionsMenu)
										{


											if(highlightedOption == 1)
												{
													printf("Option2\n");

													DRAW_Clear(&LcdCanvas, LCD_WHITE);

													// Selecting Rectangle
													DRAW_Rect(&LcdCanvas, 25,35, LcdCanvas.Width-11, LcdCanvas.Height-17, LCD_BLACK);

													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													DRAW_PrintString(&LcdCanvas, 02, 5, "Select Option", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+16, "1.Set Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+28, "2.Delete Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+42, "3.Change Time", LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);

													//Highlight list 2
													highlightedOption = 2;
												}
											else if(highlightedOption == 2)
												{
													printf("Option3\n");

													// Highlight Option 3
													highlightedOption = 3;

													DRAW_Clear(&LcdCanvas, LCD_WHITE);

													// Selecting Rectangle
													DRAW_Rect(&LcdCanvas, 25,48, LcdCanvas.Width-11, LcdCanvas.Height-4, LCD_BLACK);

													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													DRAW_PrintString(&LcdCanvas, 02, 5, "Select Option", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+16, "1.Set Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+28, "2.Delete Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+42, "3.Change Time", LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);
												}
											else if(highlightedOption == 3)
												{
													printf("Option1\n");

													DRAW_Clear(&LcdCanvas, LCD_WHITE);

													// Selecting Rectangle
													DRAW_Rect(&LcdCanvas, 25,23, LcdCanvas.Width-11, LcdCanvas.Height-29, LCD_BLACK);

													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													DRAW_PrintString(&LcdCanvas, 02, 5, "Select Option", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+16, "1.Set Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+28, "2.Delete Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+42, "3.Change Time", LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);

													// Highlight Option 1
													highlightedOption = 1;
												}


										}
									if(state == SetAlarm)
										{
											if(setAlarmState == 1)
												{
													//alarmHour = (alarmHour + 1 + 24) % 24;
													alarmHour = (alarmHour - 1) % 12;
													if (alarmHour == 0) alarmHour = 12; // Adjust for 12-hour format

													// Update the display
													DRAW_Clear(&LcdCanvas, LCD_WHITE);
													DRAW_Rect(&LcdCanvas, 30,42, LcdCanvas.Width-82, LcdCanvas.Height-10, LCD_BLACK);
													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													DRAW_PrintString(&LcdCanvas, 30, 5, "Set Alarm", LCD_BLACK, &font_16x16);
													char buffer[20];
													snprintf(buffer, sizeof(buffer), "%02d:%02d %s", alarmHour, alarmMinute, AMorPM);
													DRAW_PrintString(&LcdCanvas, 30, 40, buffer, LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);
												}
											else if(setAlarmState == 2)
												{
													alarmMinute = (alarmMinute - 1 + 60) % 60;

													DRAW_Clear(&LcdCanvas, LCD_WHITE);
													DRAW_Rect(&LcdCanvas, 53,42, LcdCanvas.Width-58, LcdCanvas.Height-10, LCD_BLACK);
													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
												    DRAW_PrintString(&LcdCanvas, 30, 5, "Set Alarm", LCD_BLACK, &font_16x16);
													char buffer[20];
													snprintf(buffer, sizeof(buffer), "%02d:%02d %s", alarmHour, alarmMinute, AMorPM);
													DRAW_PrintString(&LcdCanvas, 30, 40, buffer, LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);
												}
											else if(setAlarmState == 3)
												{
													if (strcmp(AMorPM, "AM") == 0)
														{
															strcpy(AMorPM, "PM");
														}
													else
														{
															strcpy(AMorPM, "AM");
														}

													DRAW_Clear(&LcdCanvas, LCD_WHITE);
													DRAW_Rect(&LcdCanvas, 77,40, LcdCanvas.Width-30, LcdCanvas.Height-10, LCD_BLACK);
													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													DRAW_PrintString(&LcdCanvas, 30, 5, "Set Alarm", LCD_BLACK, &font_16x16);
													char buffer[20];
													snprintf(buffer, sizeof(buffer), "%02d:%02d %s", alarmHour, alarmMinute, AMorPM);
													DRAW_PrintString(&LcdCanvas, 30, 40, buffer, LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);
												}
										}

								}
							else if(buttonValue == 2 )
								{
								printf("RIGHT SIDEWAYS\n");

								    if(state == OptionsMenu)
										{
											// This should Navigate to the Home menu
								    		state = HomeScreen;
								    		DRAW_Clear(&LcdCanvas, LCD_WHITE);

								    		DRAW_PrintString(&LcdCanvas, 20, 5, greetingBuffer, LCD_BLACK, &font_16x16);

								    		DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
								    		DRAW_PrintString(&LcdCanvas, 30, 5+25, dayOfWeekBuffer, LCD_BLACK, &font_16x16);
								    		DRAW_PrintString(&LcdCanvas, 25, 5+35, monthDate, LCD_BLACK, &font_16x16);
								    		DRAW_PrintString(&LcdCanvas, 37, 5+45, yearBuffer, LCD_BLACK, &font_16x16);
								    		DRAW_Refresh(&LcdCanvas);
										}
								    else if(state == SetAlarm)
										{

								    		// Draw a box round the part being worked on
								    		if(setAlarmState == 0) // Default to having hours selected - Hours is Alarm state 1
												{
								    				setAlarmState = 1;

								    				DRAW_Clear(&LcdCanvas, LCD_WHITE);
								    				DRAW_Rect(&LcdCanvas, 30,42, LcdCanvas.Width-82, LcdCanvas.Height-10, LCD_BLACK);
								    				DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
								    				DRAW_PrintString(&LcdCanvas, 30, 5, "Set Alarm", LCD_BLACK, &font_16x16);
								    				char buffer[20];
								    				snprintf(buffer, sizeof(buffer), "%02d:%02d %s", alarmHour, alarmMinute, AMorPM);
								    				DRAW_PrintString(&LcdCanvas, 30, 40, buffer, LCD_BLACK, &font_16x16);
								    				DRAW_Refresh(&LcdCanvas);
												}
								    		else if(setAlarmState == 1) // Minutes Selected - Minutes are state 2
								    			{
								    				setAlarmState = 2;
								    				DRAW_Clear(&LcdCanvas, LCD_WHITE);
								    				DRAW_Rect(&LcdCanvas, 53,42, LcdCanvas.Width-58, LcdCanvas.Height-10, LCD_BLACK);
								    				DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
								    				DRAW_PrintString(&LcdCanvas, 30, 5, "Set Alarm", LCD_BLACK, &font_16x16);
								    				char buffer[20];
								    				snprintf(buffer, sizeof(buffer), "%02d:%02d %s", alarmHour, alarmMinute, AMorPM);
								    				DRAW_PrintString(&LcdCanvas, 30, 40, buffer, LCD_BLACK, &font_16x16);
								    				DRAW_Refresh(&LcdCanvas);
								    			}
								    		else if(setAlarmState == 2) // AM or PM - AM or PM is state 3
								    			{
								    				setAlarmState = 3;
								    				DRAW_Clear(&LcdCanvas, LCD_WHITE);
								    				DRAW_Rect(&LcdCanvas, 77,40, LcdCanvas.Width-30, LcdCanvas.Height-10, LCD_BLACK);
								    				DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
								    				DRAW_PrintString(&LcdCanvas, 30, 5, "Set Alarm", LCD_BLACK, &font_16x16);
								    				char buffer[20];
								    				snprintf(buffer, sizeof(buffer), "%02d:%02d %s", alarmHour, alarmMinute, AMorPM);
								    				DRAW_PrintString(&LcdCanvas, 30, 40, buffer, LCD_BLACK, &font_16x16);
								    				DRAW_Refresh(&LcdCanvas);
								    			}
								    		else if(setAlarmState == 3) //Return back to hours
								    			{
								    				setAlarmState = 1;
								    				printf("Current alarm state: %d\n", setAlarmState);
								    				DRAW_Clear(&LcdCanvas, LCD_WHITE);
								    				DRAW_Rect(&LcdCanvas, 30,42, LcdCanvas.Width-82, LcdCanvas.Height-10, LCD_BLACK);
								    				DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
								    				DRAW_PrintString(&LcdCanvas, 30, 5, "Set Alarm", LCD_BLACK, &font_16x16);
								    				char buffer[20];
								    				snprintf(buffer, sizeof(buffer), "%02d:%02d %s", alarmHour, alarmMinute, AMorPM);
								    				DRAW_PrintString(&LcdCanvas, 30, 40, buffer, LCD_BLACK, &font_16x16);
								    				DRAW_Refresh(&LcdCanvas);
								    			}
								    		//printf("The alarm is set for: %d : %d %s\n", alarmHour, alarmMinute, AMorPM);
										}
									}
							else if(buttonValue == 1)
								{
									printf("OK\n");
									if(state == HomeScreen)
										{
											// When the user is shown the options of what to do with the Alarm clock .
											state = OptionsMenu;
											DRAW_Clear(&LcdCanvas, LCD_WHITE);
											DRAW_Rect(&LcdCanvas, 25,23, LcdCanvas.Width-11, LcdCanvas.Height-29, LCD_BLACK); // Select Option 1 by default

                                            DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
								            DRAW_PrintString(&LcdCanvas, 02, 5, "Select Option", LCD_BLACK, &font_16x16);
											DRAW_PrintString(&LcdCanvas, 10, 5+16, "1.Set Alarm", LCD_BLACK, &font_16x16);
											DRAW_PrintString(&LcdCanvas, 10, 5+28, "2.Delete Alarm", LCD_BLACK, &font_16x16);
											DRAW_PrintString(&LcdCanvas, 10, 5+42, "3.Change Time", LCD_BLACK, &font_16x16);
										    DRAW_Refresh(&LcdCanvas);
										}
									else if (state == OptionsMenu)
										{
											//select highlighted option
											if(highlightedOption == 1)
												{
													state = SetAlarm;

													DRAW_Clear(&LcdCanvas, LCD_WHITE);
													DRAW_PrintString(&LcdCanvas, 30, 5, "Set Alarm", LCD_BLACK, &font_16x16);
													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													char buffer[20];
													snprintf(buffer, sizeof(buffer), "%02d:%02d %s", alarmHour, alarmMinute, AMorPM);
													DRAW_PrintString(&LcdCanvas, 30, 40, buffer, LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);
												}
											else if(highlightedOption == 2)
												{
													// Delete Alarm
													DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
													DRAW_PrintString(&LcdCanvas, 02, 5, "Delete Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+16, "1.Set Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+28, "2.Delete Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+42, "3.Change Time", LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);

												}
											else if(highlightedOption == 3)
												{
													// Change Time
												}
											else
												{

												}
										}
									else if (state == SetAlarm)
										{
											setAlarm(alarmHour, alarmMinute, AMorPM);

											// Message to show that alarm has been set
								    		state = HomeScreen;
								    		DRAW_Clear(&LcdCanvas, LCD_WHITE);
								    		DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
								     		DRAW_PrintString(&LcdCanvas, 30, 5+25, "Alarm has", LCD_BLACK, &font_16x16);
								    		DRAW_PrintString(&LcdCanvas, 25, 5+35, "been set!", LCD_BLACK, &font_16x16);
								    		//DRAW_PrintString(&LcdCanvas, 37, 5+45, yearBuffer, LCD_BLACK, &font_16x16);
								    		DRAW_Refresh(&LcdCanvas);

								    		 sleep(2); // Sleep for 2 seconds to allow the user to read the message

								    		// This should Navigate to the Home menu
								    		DRAW_Clear(&LcdCanvas, LCD_WHITE);

								    		DRAW_PrintString(&LcdCanvas, 20, 5, greetingBuffer, LCD_BLACK, &font_16x16);

								    		DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
								    		DRAW_PrintString(&LcdCanvas, 30, 5+25, dayOfWeekBuffer, LCD_BLACK, &font_16x16);
								    		DRAW_PrintString(&LcdCanvas, 25, 5+35, monthDate, LCD_BLACK, &font_16x16);
								    		DRAW_PrintString(&LcdCanvas, 37, 5+45, yearBuffer, LCD_BLACK, &font_16x16);
								    		DRAW_Refresh(&LcdCanvas);
										}
									else if (state == DeleteAlarm)
										{

										}
									else if (state == ChangeTime)
										{
										}
								}

									// UpdatepPrevious button value
									prev_key_value = *KEY_ptr;

								   //DRAW_Clear(&LcdCanvas, LCD_WHITE); // Clear the screen
								  // char info[128];
								   /*snprintf(info, sizeof(info), "Song: %s", player.currentTrack->songName);
								   DRAW_PrintString(&LcdCanvas, 1, 5, info, LCD_BLACK, &font_16x16);
								   snprintf(info, sizeof(info), "Artist: %s", player.currentTrack->artist);
								   DRAW_PrintString(&LcdCanvas, 1, 25, info, LCD_BLACK, &font_16x16);
								   snprintf(info, sizeof(info), "Album: %s", player.currentTrack->album);
								   DRAW_PrintString(&LcdCanvas, 1, 45, info, LCD_BLACK, &font_16x16);*/
								  // DRAW_Refresh(&LcdCanvas);
								}
					}

			 	 free(LcdCanvas.pFrame);

			 	 // clean up our memory mapping and exit
			 	 if( munmap( virtual_base, HW_REGS_SPAN ) != 0 )
					 {
						 printf( "ERROR: munmap() failed...\n" );
						 close( fd );
						 return( 1 );
					 }


			 	 close( fd );
			 	 return( 0 );
	}


