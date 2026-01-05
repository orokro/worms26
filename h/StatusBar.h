/*
	StatusBar.h
	-----------

	Main header file for the StatusBar.c module
*/

#ifndef STATUSBAR_H
#define STATUSBAR_H

/**
 * Shows a message in the status bar for a set amount of time.
 * 
 */
void StatusBar_showMessage(const char *msg);


/**
 * Updates the status bar, showing messages as needed.
 * 
 */
extern void StatusBar_update();


/**
 * Renders the status bar messages to the screen.
 * 
 */
extern void StatusBar_draw();

#endif
