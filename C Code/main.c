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
					buttonValue = *KEY_ptr;


					// State Change
					if(buttonValue!= prev_key_value)
						{
							if(buttonValue == 8)
								{
									printf("UP\n");

									if(state == SetAlarm)
										{
											if(setAlarmState == SetAlarmHour)
												{
													alarmHour = (alarmHour - 1 + 24) % 24;
												}
											else if(setAlarmState == SetAlarmMinute)
												{
													alarmMinute = (alarmMinute - 1 + 60) % 60;
												}
											else if(setAlarmState == SetAlarmAMPM)
												{
													if (strcmp(AMorPM, "AM") == 0)
														{
												        	strcpy(AMorPM, "PM");
														}
													else
														{
															strcpy(AMorPM, "AM");
														}
												}

											alarmHour = (alarmHour + 1) % 12;
											if (alarmHour == 0) alarmHour = 12; // Adjust for 12-hour format

											// Directly update the display here
											DRAW_Clear(&LcdCanvas, LCD_WHITE);
											DRAW_Rect(&LcdCanvas, 0, 0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
											char buffer[20];
											snprintf(buffer, sizeof(buffer), "%02d:%02d %s", alarmHour, alarmMinute, AMorPM);
											DRAW_PrintString(&LcdCanvas, 30, 40, buffer, LCD_BLACK, &font_16x16);
											DRAW_Refresh(&LcdCanvas);
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
													DRAW_PrintString(&LcdCanvas, 10, 5+28, "2.Edit Alarm", LCD_BLACK, &font_16x16);
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
													DRAW_PrintString(&LcdCanvas, 10, 5+28, "2.Edit Alarm", LCD_BLACK, &font_16x16);
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
													DRAW_PrintString(&LcdCanvas, 10, 5+28, "2.Edit Alarm", LCD_BLACK, &font_16x16);
													DRAW_PrintString(&LcdCanvas, 10, 5+42, "3.Change Time", LCD_BLACK, &font_16x16);
													DRAW_Refresh(&LcdCanvas);

													// Highlight Option 1
													highlightedOption = 1;
												}


										}
									else if(state == SetAlarm)
										{
											// Increment the current value that has been selected.
											//printf("set alarm placeholder\n");
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
								    		DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
								    		DRAW_PrintString(&LcdCanvas, 20, 5, greetingBuffer, LCD_BLACK, &font_16x16);
								    		DRAW_PrintString(&LcdCanvas, 30, 5+25, dayOfWeekBuffer, LCD_BLACK, &font_16x16);
								    		DRAW_PrintString(&LcdCanvas, 25, 5+35, monthDate, LCD_BLACK, &font_16x16);
								    		DRAW_PrintString(&LcdCanvas, 37, 5+45, yearBuffer, LCD_BLACK, &font_16x16);
								    		DRAW_Refresh(&LcdCanvas);
										}
								    else if(state == ChangeTime || state == SetAlarm)
										{
											// Handle specific actions for changing time or setting alarm

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
                                            DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK);
								            DRAW_PrintString(&LcdCanvas, 02, 5, "Select Option", LCD_BLACK, &font_16x16);
											DRAW_PrintString(&LcdCanvas, 10, 5+16, "1.Set Alarm", LCD_BLACK, &font_16x16);
											DRAW_PrintString(&LcdCanvas, 10, 5+28, "2.Edit Alarm", LCD_BLACK, &font_16x16);
											DRAW_PrintString(&LcdCanvas, 10, 5+42, "3.Change Time", LCD_BLACK, &font_16x16);
										    DRAW_Refresh(&LcdCanvas);
										}
									else if (state == OptionsMenu)
										{
											//select highlighted option
											if(highlightedOption == 1)
												{
													state = SetAlarm;
													//setAlarmState = SetAlarmHour;

													//setAlarm();
												}
											else if(highlightedOption == 2)
												{
													// Edit Alarm

												}
											else if(highlightedOption == 3)
												{
													// Change Time
												}
											else
												{

												}
										}
									else if (state == EditAlarmMenu || state == ChangeTime || state == SetAlarm)
										{
											// Select highlighted option
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


