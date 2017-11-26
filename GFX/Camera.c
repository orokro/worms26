// C Source File
// Created 11/11/2017; 11:34:14 PM

#include "Main.h"

/*
	Camera
	------
	
	This is the file for the Camera system.
	
	In the previus version of Worms, the Camera system was rather rigid.
	
	The Camera was either:
		- locked on 1:1 with the worm in control
		- locked on 1:1 with the weapon
		- controlled by the user
	
	The camera could not go out-of-bounds of the map. This is changed.
	
	In this new-and-improved Camera system, the camera will have targets
	that it attempts to follow.
	
	It will not immediately lock-on to a target, but rather, it will
	take it's current position and gradually move towards it's target
	every frame. This way, the camera will smoothly move between worms
	/ objects in focus.
*/

// the current position of the camera in game-world units
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



// --------------------------------------------------------------------------------------------------------------------------------------



// this is the main update function for the logic of the Camera.
void Camera_update()
{
	// if this is the first frame that shift was pressed,
	// we should reset the user offsets to whatever the camera's current
	// position is..
	if(Keys_keyDown(keyCameraControl))
	{
		userX = camX;
		userY = camY;
	}
	
	// if shift is down, we should test for the arrow keys for camera control:
	if(Keys_keyState(keyCameraControl))
	{
		if(Keys_keyState(keyLeft)==TRUE) userX-=camSpeed;
		if(Keys_keyState(keyRight)==TRUE) userX+=camSpeed;
		if(Keys_keyState(keyUp)==TRUE) userY-=camSpeed;
		if(Keys_keyState(keyDown)==TRUE) userY+=camSpeed;
		
		// at this point, the camera should just be the user-set value:
		camX = userX;
		camY = userY;
		
		
	// otherwise if we are focusing on a target, we should move towards it...
	}else if(cameraIsFocused==TRUE)
	{
		// calc deltas:
		short deltaX = (*cameraTargetX - camX);
		short deltaY = (*cameraTargetY - camY);
		
		// move just some small percent
		short moveX = (short)(deltaX * 0.3f);
		short moveY = (short)(deltaY * 0.3f);
		
		// if the deltas aren't zero (camera perfectly focused) but our moves
		// just make them 1 pixel in the correct direction. The camera always
		// gets within 4 pixels of it's target (due to the 0.2f) so we will
		// allways move one pixel at a time towards the target within 4 pixels
		if(deltaX!=0 && moveX==0)
			moveX = deltaX / abs(deltaX);
		if(deltaY!=0 && moveY==0)
			moveY = deltaY / abs(deltaY);
		
		// move the camera pos:
		camX += moveX;
		camY += moveY;
	}
	
	//  make sure the camera is always within bounds
	if(camY>200-50)
		camY=200-50;
	if(camY<-100)
		camY=-100;
	if(camX<-100)
		camX=-100;
	if(camX>320+100)
		camX=320+100;
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
