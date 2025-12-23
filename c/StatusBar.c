/*
	StatusBar.c
	-----------

	This file will provide a system to queue & show status text for things like:
	- crate pickups
	- worm deaths
	- turn changes,
	- weapon usage
	- etc
*/

// includes
#include "Main.h"

// defines
#define STATUSBAR_MAX_MESSAGES 10
#define STATUSBAR_MESSAGE_DISPLAY_TIME 50  // in frames


// local vars

// array of 10 string pointers for the messages
static char* StatusBar_messages[STATUSBAR_MAX_MESSAGES];

// timer to show each message
static char StatusBar_messageTimer = 0;



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * Shows a message in the status bar for a set amount of time.
 * 
 */
extern void StatusBar_showMessage(const char* message) {

	// add message to first empty, no shifting
	short i;
	for (i = 0; i < STATUSBAR_MAX_MESSAGES; i++) {
		if(StatusBar_messages[i] == NULL) {
			
			StatusBar_messages[i] = (char*)message;
			
			// if i is negative we succeeded in adding the message
			i = -1;
			break;
		}
	}

	// if we just overwrite the end
	if (i != -1)
		StatusBar_messages[STATUSBAR_MAX_MESSAGES - 1] = (char*)message;

	// reset the timer
	// show for 100 frames (adjust as needed)
	StatusBar_messageTimer = STATUSBAR_MESSAGE_DISPLAY_TIME; 
}


/**
 * Updates the status bar, showing messages as needed.
 * 
 */
extern void StatusBar_update() {

	// if we have a message timer running
	if (StatusBar_messageTimer > 0) {

		// decrement the timer
		StatusBar_messageTimer--;

		// if the timer is done shift the messages
		if (StatusBar_messageTimer == 0) {
			short i;
			for (i = 0; i < STATUSBAR_MAX_MESSAGES - 1; i++) {
				StatusBar_messages[i] = StatusBar_messages[i + 1];
			}

			// clear the last message
			StatusBar_messages[STATUSBAR_MAX_MESSAGES - 1] = NULL;

			// if messages remain, reset the timer
			if (StatusBar_messages[0] != NULL) {
				StatusBar_messageTimer = STATUSBAR_MESSAGE_DISPLAY_TIME; 
			}
		}

	}
}


/**
 * Renders the status bar messages to the screen.
 * 
 */
extern void StatusBar_draw() {

	// GTFO if clock is 0
	if (StatusBar_messageTimer == 0)
		return;

	// render each message
	GrayDrawStr2B(1, 1, StatusBar_messages[0], A_XOR, lightPlane, darkPlane);
}
