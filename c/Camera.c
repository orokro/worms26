/*
	Camera.c
	--------
	
	This is the file for the Camera system.
	
	In the previous version of Worms, the Camera system was rather rigid.
	
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
	
	C Source File
	Created 11/11/2017; 11:34:14 PM
*/


// Camera.c

// includes
#include "Main.h"
#include "Camera.h"
#include "Keys.h"
#include "Explosions.h"
#include "Weapons.h"
#include "Worms.h"


// the current position of the camera in game-world units (RENDER coordinates)
short camX = 0;
short camY = 0;

// LOGICAL position (Used for smooth tracking, before shake is applied)
static short logicalCamX = 0;
static short logicalCamY = 0;

// boolean if Camera is focused on something
char cameraIsFocused = FALSE;

// true if camera is in auto-focus mode
char cameraAutoFocus = TRUE;

// Global shake flag
char Camera_shake = FALSE;

short *cameraTargetX;
short *cameraTargetY;

short userX=0;
short userY=0;



// --------------------------------------------------------------------------------------------------------------------------------------



// this is the main update function for the logic of the Camera.
void Camera_update()
{
    short i;

    // 1. Handle Manual Control (Operates on LOGICAL pos)
    if(Keys_keyDown(keyCameraControl))
    {
        // Reset user offset to current logical position
        userX = logicalCamX;
        userY = logicalCamY;
    }
    
    if(Keys_keyState(keyCameraControl))
    {
        if(Keys_keyState(keyLeft) && logicalCamX > -100) userX -= camSpeed;
        if(Keys_keyState(keyRight) && logicalCamX < 320+100) userX += camSpeed;
        if(Keys_keyState(keyUp) && logicalCamY > -100) userY -= camSpeed;
        if(Keys_keyState(keyDown) && logicalCamY < 220-6-50) userY += camSpeed;
        
        // Set logical pos directly
        logicalCamX = userX;
        logicalCamY = userY;

    }
    // 2. Handle Auto-Focus / Tracking (Operates on LOGICAL pos)
    else if(cameraIsFocused == TRUE)
    {
        // calc deltas based on LOGICAL position
        short deltaX = (*cameraTargetX - logicalCamX);
        short deltaY = (*cameraTargetY - logicalCamY);
        
        // move just some small percent
        short moveX = (short)(deltaX * 0.3f);
        short moveY = (short)(deltaY * 0.3f);
        
        // Ensure minimum 1px movement if close but not there
        if(deltaX != 0 && moveX == 0)
            moveX = deltaX / abs(deltaX);
        if(deltaY != 0 && moveY == 0)
            moveY = deltaY / abs(deltaY);
        
        // update logical pos
        logicalCamX += moveX;
        logicalCamY += moveY;
    }
    
    // 3. Auto-Focus Target Selection
    if(cameraAutoFocus == TRUE)
    {
        if(Weapon_active & ~Weapon_isFire)
        {
            i = Weapons_getFirstActive();
            cameraTargetX = &Weapon_x[i];
            cameraTargetY = &Weapon_y[i];
        }
		else if(Explosion_active)
        {
            i = Explosion_getFirstActive();
            cameraTargetX = &Explosion_x[i];
            cameraTargetY = &Explosion_y[i];
        }
        else
        {
            cameraTargetX = &Worm_x[(short)Worm_currentWorm];
            cameraTargetY = &Worm_y[(short)Worm_currentWorm];
        }
        cameraIsFocused = TRUE;
    }
    
    // 4. Bounds Check (Clamp LOGICAL position)
    if(logicalCamY > 220-6-50) logicalCamY = 220-6-50;
    if(logicalCamY < -100)     logicalCamY = -100;
    if(logicalCamX < -100)     logicalCamX = -100;
    if(logicalCamX > 320+100)  logicalCamX = 320+100;

    // 5. Apply Shake and Set Final Render Position (camX/camY)
    if(Camera_shake)
    {
        // Shake randomly between -3 and +3 pixels
        camX = logicalCamX + (random(7) - 3);
        camY = logicalCamY + (random(7) - 3);
    }
    else
    {
        camX = logicalCamX;
        camY = logicalCamY;
    }
}


/**
 * Focuses the camera on the given target pointers, so it will follow them.
 * 
 * @param *targetX pointer to the x value to focus on
 * @param *targetY pointer to the y value to focus on
 */
void Camera_focusOn(short *targetX, short *targetY)
{
	// save our target pointers
	cameraTargetX = targetX;
	cameraTargetY = targetY;
	
	// we are now in camera follow mode:
	cameraIsFocused=TRUE;
}


/**
 * @brief Clears the camera focus if it's currently focused on the given target
 * 
 * @param targetX The target X pointer to check
 * @param targetY The target Y pointer to check
 */
void Camera_clearIf(short *targetX, short *targetY)
{
	// only clear if the current target matches the given target
	if(cameraIsFocused && cameraTargetX==targetX && cameraTargetY==targetY)
	{
		Camera_clearFocus();
	}
}


/**
 * @brief Clears the camera focus if it's currently focused on a different target than the given one
 * 
 * @param targetX - The target X pointer to check
 * @param targetY - The target Y pointer to check
 */
void Camera_clearUnless(short *targetX, short *targetY)
{
	// only clear if the current target does NOT match the given target
	if(cameraIsFocused && (cameraTargetX!=targetX || cameraTargetY!=targetY))
	{
		Camera_clearFocus();
	}
}
