#include "ClockFace.h"
#include "Time.h"

#include <stdio.h>

// Assuming Time struct is defined in ClockFace.h or Time.h
// If not, define Time struct here as well.

void fillTimeStruct(Time* time)
	{
		getCurrentTime(time);
	}

void fillTimeOnBCDStruct(Time* time)
	{
		getCurrentTime(time);
	}
