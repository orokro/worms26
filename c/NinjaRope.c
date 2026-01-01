/*
	NinjaRope.c
	-----------

	This file handles the Ninja Rope weapon.

	Because this weapon has unique logic that needs to be updated every frame,
	and it's pretty substantial, it gets it's own file.

	This file will be in-line included in CharacterController.c
*/

/**
 * @brief Get the Ninja Rope angles based on a int value between 0-360
 * 
 * @param angle - angle in degrees
 * @param outX - output X component
 * @param outY - output Y component
 */
void getNinjaRopeAngles(short angle, short *outX, short *outY)
{
	// get the direction vector from the angle
	// TODO: optimize with a lookup table

	// *outX = 
	// *outY = 
}


/**
 * @brief computes fast sine from degrees
 * 
 * @param thetaInDegrees - angle in degrees
 * @return float - sine value
 */
float fastSin(short thetaInDegrees)
{
	// TODO: implement table
}

/**
 * @brief Computes fast cosine from degrees	
 * 
 * @param thetaInDegrees - angle in degrees
 * @return float - cosine value
 */
float fasCos(short thetaInDegrees)
{
	// TODO: implement table
}


/**
 * @brief Moves the worm along the ninja rope in or out
 * 
 * @param direction - -1 to move in, 1 to move out
 */
void NinjaRope_moveInOut(short direction)
{
	// TODO: implement
}

/**
 * @brief Normalizes degrees to 0-359
 * 
 * @param degrees - degrees to normalize
 */
void getNormalizedDegrees(short degrees){

	while(degrees<0)
		degrees+=360;
	return degrees%360;
}


/**
 * @brief Get's the distance from the current worm to the last ninja rope point
 * 
 * @return short - distance in pixels
 */
short fastDistance()
{
	// TODO: implement
}


/**
 * @brief Changes the worms position based on ninja rope rotation
 * 
 * @return char - TRUE if the angle changed, FALSE otherwise
 */
char doNinjaRopeRotation()
{
	/* 
		TODO: implement rotation around:
		x = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount-1][0];
		y = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount-1][0];
		radius = fastDistance()

		return TRUE if the Game_ninjaRopeAngle changed
	*/
}


/**
 * @brief Checks if the old hit point should be removed
 * 
 * @return char - TRUE if it was removed, FALSE otherwise
 */
char checkToRemoveOldHitPoint()
{
	// TODO: implement
}


/**
 * @brief Checks if the new rotation should add a new ninja rope point
 * 
 * @return char 
 */
char raycastForNewHitPoint()
{
	// TODO: implement

	// even if we git a hit, don't add it if it's closer to the last point than this
	const minDistanceForNewPoint = 5;

	// TODO: compute dirX/dirY from Game_ninjaRopeAngle
	// short dirX=0, dirY=0;

	RaycastHit hit = Game_raycast(Worm_x[Worm_currentWorm], Worm_x[Worm_currentWorm], dirX, dirY, FALSE);
	
	if(hit.hitType != RAY_HIT_NOTHING)
	{
		// ...
	}
	else
	{
		return FALSE;
	}
}


/**
 * @brief updates the current ninja rope angle based on rotation speed and direction
 */
void updateNinjaRopeCurrentAngle()
{
	// TODO: implement
	// do the math to set Game_ninjaRopeAngle, where 12'o'clock is 0 degrees, 3 o'clock is 90 degrees, etc.
}


/**
 * @brief Does the initial ninja rope shot raycast
 * 
 * @param dirX - direction X
 * @param dirY - direction Y
 * @return char - TRUE if we hit something, FALSE otherwise
 */
char CharacterController_doInitialNinjaRopeShot(short dirX, short dirY)
{
	RaycastHit hit = Game_raycast(Worm_x[Worm_currentWorm], Worm_x[Worm_currentWorm], dirX, dirY, FALSE);

	// get worm position
	const short wx = Worm_x[Worm_currentWorm];
	const short wy = Worm_y[Worm_currentWorm];

	// draw a ray, regardless of whether we hit or not
	short sx = wx, sy =wy, ex = (wx+(dirX*100)), ey = (wy+(dirY*100));
	worldToScreen(&sx, &sy);
	worldToScreen(&ex, &ey);
	Draw_setRayLine(sx, sy, ex, ey);

	// if we did hit something, though, start the ninja rope mode
	if(hit.hitType != RAY_HIT_NOTHING)
	{	
		// compute or initial angle
		updateNinjaRopeCurrentAngle();

		// enable the ninja rope mode
		Game_stateFlags |= gs_ninjaRopeMode

		// save the first anchor point data
		Game_ninjaRopeAnchors[0] = hit.x;
		Game_ninjaRopeAnchors[1] = hit.y;
		Game_ninjaRopeAnchors[2] = Game_ninjaRopeAngle;
		Game_ninjaRopeAnchors[3] = 0; // always 0 for the first one
		
		// set our ninja rope state variables
		Game_ninjaRopeAnchorCount = 1;
		Game_ninjaRopeRotationDir = 0;
		Game_ninjaRopeRotationSpeed = 0;
		
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/**
 * @brief Handles the logic for the NinjaRope
 */
void wormNinjaRope(){

	// gtfo if we're not in ninja rope mode
	if(!(Game_stateFlags & gs_ninjaRopeMode))
		return;

	// unsettle worm
	Worm_physObj[(short)Worm_currentWorm].staticFrames = 0;
	Worm_settled &= ~wormMask;

	// if action key is pressed, exit mode
	if(Keys_keyState(keyAction))
	{
		Game_stateFlags &= ~gs_ninjaRopeMode;
		return;
	}

	// if the up key is pressed, we should get closer to the current / last ninja rope point
	if(Keys_keyState(keyUp))
	{
		// -1 implies move closer, shrink distance
		NinjaRope_moveInOut(-1);
	}
	else if(Keys_keyState(keyDown))
	{
		// 1 implies move away, increase distance
		NinjaRope_moveInOut(1);
	}

	// for testing, we can get the current angle in normalized space
	const short angle = getNormalizedDegrees(Game_ninjaRopeAngle);

	// if left or right are pressed, we need to do ninja-rope rotation
	if(Keys_keyState(keyLeft|keyRight))
	{
		// based on the current angle, if it's between 90-270, we need to invert the rotation direction
		const char invert = (angle>90 && angle<270) ? -1 : 1;
		if(Keys_keyState(keyLeft))
		{
			// rotate left
			Game_ninjaRopeRotationDir = 1 * invert;
		}
		else if(Keys_keyState(keyRight))
		{
			// rotate right
			Game_ninjaRopeRotationDir = -1 * invert;
		}
	}
	else
	{
		// not rotating
		Game_ninjaRopeRotationDir = 0;
	}

	// apply the rotation and if it rotated, we should check for adding/removing points
	if(doNinjaRopeRotation())
	{
		// see if we need to remove the last point
		checkToRemoveOldHitPoint();

		// we can also check if we should add a hit point
		raycastForNewHitPoint();
	}
}
