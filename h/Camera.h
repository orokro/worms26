/*
	Camera.h
	--------
	
	Main header file for the Camera.c module
*/
#ifndef CAMERA_H
#define CAMERA_H


// camera defines
#define camSpeed 6

// camera globals
extern short camX;
extern short camY;

// true if camera is in auto focus mode
extern char cameraAutoFocus;

// true to shake
extern char Camera_shake;

// camera function prototypes

/**
 * Updates the current state of the Camera, should be called once per frame.
*/
extern void Camera_update();


/**
 * Sets the Camera to focus on a pair of short points.
 *
 * The Camera will follow the pointers set each frame.
 * If the variables that were passed in as pointers are updated,
 * the Camera will have new coordinates to follow each frame.
 *
 * @param targetX a pointer to a short x variable for the Camera to track
 * @param targetY a pointer to a short y variable for the Camera to track
*/
extern void Camera_focusOn(short *targetX, short *targetY);


/**
 * @brief Clears the camera focus if it's currently focused on the given target
 * 
 * @param targetX The target X pointer to check
 * @param targetY The target Y pointer to check
 */
extern void Camera_clearIf(short *targetX, short *targetY);


/**
 * @brief Clears the camera focus if it's currently focused on a different target than the given one
 * 
 * @param targetX - The target X pointer to check
 * @param targetY - The target Y pointer to check
 */
extern void Camera_clearUnless(short *targetX, short *targetY);


/**
 * Removes the current focal reference pointers from the camera.
 *
 * After calling, the Camera will be static until moved by the user,
 * or a new target is set with Camera_focusOn(*x, *y)
 * @brief stops the camera from focusing on anything in particular
 */
static inline void Camera_clearFocus()
{
	extern char cameraIsFocused;
	// just disable the focus mode, no need to clear pointers
	// since they won't be used until set again
	cameraIsFocused=FALSE;
}

#endif
