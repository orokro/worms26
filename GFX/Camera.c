// C Source File
// Created 11/11/2017; 11:34:14 PM

#include "../Headers/System/Main.h"

// declare our external/global variables
int camX = 0;
int camY = 0;

// boolean if Camera is focused on something
char cameraIsFocused = FALSE;

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

// if the user is controlling the camera, keep track of the users
// camera offset
short userX=0;
short userY=0;

// this is the main update function for the logic of the Camera.
void Camera_update()
{
	// if this is the first frame that shift was pressed,
	// we should reset the user offsets to whatever the camera's current
	// position is..
	if(Keys_keyDown(keyCameraControl)==TRUE)
	{
		userX = camX;
		userY = camY;
	}
	
	// if shift is down, we should test for the arrow keys for camera control:
	if(Keys_keyState(keyCameraControl)==TRUE)
	{
		if(Keys_keyState(keyLeft)==TRUE) userX--;
		if(Keys_keyState(keyRight)==TRUE) userX++;
		if(Keys_keyState(keyUp)==TRUE) userY--;
		if(Keys_keyState(keyDown)==TRUE) userY++;
		
		// at this point, the camera should just be the user-set value:
		camX = userX;
		camY = userY;
		
		// and we out
		return;
	}
	
	// if we are focusing on a target, we should move towards it...
	if(cameraIsFocused==TRUE)
	{
		// calc deltas:
		short deltaX = (*cameraTargetX - camX);
		short deltaY = (*cameraTargetY - camY);
		
		// move just some small percent
		short moveX = (short)(deltaX * 0.2f);
		short moveY = (short)(deltaY * 0.2f);
		
		// if the deltas aren't zero (camera perfectly focused) but our moves
		// are 0, then just set the camera to the target
		// since move will never be 
		if(deltaX!=0 && moveX==0)
			moveX = deltaX / abs(deltaX);
		if(deltaY!=0 && moveY==0)
			moveY = deltaY / abs(deltaY);
		
		// move the camera pos:
		camX += moveX;
		camY += moveY;
	}
}

// tell the Camera to focus on a pair of X/Y coordinates
void Camera_focusOn(short *targetX, short *targetY)
{
	// save our target pointers
	cameraTargetX = targetX;
	cameraTargetY = targetY;
	
	// we are now in camera follow mode:
	cameraIsFocused=TRUE;
}

// stop the camera from focusing on anything in particular
void Camera_clearFocus()
{
	// just disable the focus mode, no need to clear pointers
	// since they won't be used until set again
	cameraIsFocused=FALSE;
}
