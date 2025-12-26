/*
	StatusBar.c
	-----------
	
	This file will provide a system to queue & show status text.
*/

// includes
#include "Main.h"
#include "StatusBar.h"
#include <string.h> // Required for strcpy

// defines
#define STATUSBAR_MAX_MESSAGES 10
#define STATUSBAR_MESSAGE_DISPLAY_TIME 50 // in frames
#define STATUSBAR_MSG_LEN 32 

// local vars

// CHANGED: Now a 2D array to hold copies of strings, not just pointers
static char StatusBar_messages[STATUSBAR_MAX_MESSAGES][STATUSBAR_MSG_LEN];

// The number of messages currently valid in the queue
static short StatusBar_count = 0;

// timer to show each message
static char StatusBar_messageTimer = 0;


// --------------------------------------------------------------------------------------------------------------------------------------


/**
 * Shows a message in the status bar for a set amount of time.
 */
void StatusBar_showMessage(const char *msg)
{
	// if we are full, we must shift everything down to make room
	if(StatusBar_count >= STATUSBAR_MAX_MESSAGES)
	{
		short i;
		for(i = 0; i < STATUSBAR_MAX_MESSAGES - 1; i++)
		{
			strcpy(StatusBar_messages[i], StatusBar_messages[i+1]);
		}
		StatusBar_count = STATUSBAR_MAX_MESSAGES - 1;
	}

	// Copy the incoming string into our local storage
	strcpy(StatusBar_messages[StatusBar_count], msg);
	
	// Reset the timer if this is the only message
	if(StatusBar_count == 0)
		StatusBar_messageTimer = STATUSBAR_MESSAGE_DISPLAY_TIME;

	StatusBar_count++;
}


/**
 * Updates the status bar logic (timers and queue management)
 */
void StatusBar_update()
{
	// if we have active messages
	if(StatusBar_count > 0)
	{
		StatusBar_messageTimer--;

		// if time is up for the top message
		if(StatusBar_messageTimer <= 0)
		{
			// Shift messages down (remove the top one)
			short i;
			for(i = 0; i < StatusBar_count - 1; i++)
			{
				strcpy(StatusBar_messages[i], StatusBar_messages[i+1]);
			}
			
			StatusBar_count--;

			// Reset timer for the next message (if any)
			if(StatusBar_count > 0)
				StatusBar_messageTimer = STATUSBAR_MESSAGE_DISPLAY_TIME;
		}
	}
}


/**
 * Draws the current status bar message (index 0)
 */
void StatusBar_draw()
{
	// RESTORED: Original logic using messageTimer and GrayDrawStr2B
	if(StatusBar_count > 0)
	{
		// Calculate centered X position: 80 - (PixelWidth / 2)
		// Assuming 4x6 font, width is roughly len * 4
		const short x = 80 - ((strlen(StatusBar_messages[0]) * 4) >> 1);
		
		// Draw to both grayscale planes
		GrayDrawStr2B(x, 1, StatusBar_messages[0], A_XOR, lightPlane, darkPlane);
	}
}
