// Header File
// Created 11/11/2017; 11:31:35 PM

/*
	Camera
	------
	
	This is the header file for the Camera system.
	
	In the previus version of Worms, the Camera system was rather rigid.
	
	The Camera was either:
		- locked on 1:1 with the worm in control
		- locked on 1:1 with the weapon
		- controlled by the user
		
	In this new-and-improved Camera system, the camera will have targets
	that it attempts to follow.
	
	It will not immediately lock-on to a target, but rather, it will
	take it's current position and gradually move towards it's target
	every frame. This way, the camera will smoothly move between worms
	/ objects in focus.
	
	This header will define some public methods for controlling the camera,
	as well as the current x/y value of the camera.

*/

// the current position of the camera in game-world units
int camX = 0;
int camY = 0;

// function prototypes
void Camera_update();
void Camera_focusOn();
void Camera_clearFocus();
