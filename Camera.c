// C Source File
// Created 11/11/2017; 11:34:14 PM

#include "Main.h"

// boolean if Camera is focused on something
char isFocused = 0;

/* 
	Pointers to shorts that the Camera should focus on.
	
	Notice that we are using POINTERS here...
	
	If the x/y position of a worm, weapon, or something else is passed
	as a focus target as pointers, when that item moves, the Camera's
	targets will also change (since they're pointers to the original
	values)
*/
short *cameraTargetX;
short *cameraTargetY;


// this is the main update function for the logic of the Camera.
Camera_update()
{
	
}

// tell the Camera to focus on a pair of X/Y coordinates
Camera_focusOn(short *targetX, short *target Y)
{
	
}

Camera_clearFocus()
{
	
}
