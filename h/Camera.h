#ifndef CAMERA_H
#define CAMERA_H

/* ======================================================================================================================================
   CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CAMERA +++ CA
   ====================================================================================================================================== */

// camera defines
#define camSpeed 6

// camera globals
extern short camX;
extern short camY;

// camera function prototypes

/**
 * Updates the current state of the Camera, should be called once per frame.
*/
extern void Camera_update();

/**
 * Sets the Camera to focus on a pair of short points.
 *
 * The Camera will follow the pointers set each frame.
 * If the variavles that were passed in as pointers are updated,
 * the Camera will have new coordinates to follow each frame.
 *
 * @param targetX a pointer to a short x variable for the Camera to track
 * @param targetY a pointer to a short y variable for the Camera to track
*/
extern void Camera_focusOn(short*, short*);

/**
 * Removes the current focal reference pointers from the camera.
 *
 * After calling, the Camera will be static until moved by the user,
 * or a new target is set with Camera_focusOn(*x, *y)
*/
extern void Camera_clearFocus();

#endif