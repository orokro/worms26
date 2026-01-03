/*
	NinjaRope.c
	-----------

	This file handles the Ninja Rope weapon.

	Because this weapon has unique logic that needs to be updated every frame,
	and it's pretty substantial, it gets it's own file.

	This file will be in-line included in CharacterController.c
*/

// #include <math.h> // Caused FPU Privilege Violation

#ifndef PI
#define PI 3.14159265358979323846
#endif

#define SIN_SCALE 8192

/**
 * @brief Integer atan2 approximation to avoid FPU usage
 * Returns angle 0-360
 */
short fastAtan2(short y, short x)
{
    short absX = x < 0 ? -x : x;
    short absY = y < 0 ? -y : y;
    short angle;

    if (x == 0 && y == 0) return 0;

    if (absY == 0) angle = 0;
    else if (absX == 0) angle = 90;
    else if (absY <= absX) angle = (45 * absY) / absX;
    else angle = 90 - ((45 * absX) / absY);

    if (x < 0) angle = 180 - angle;
    if (y < 0) angle = 360 - angle;
    return angle;
}

/**
 * @brief computes fast sine from degrees
 * 
 * @param thetaInDegrees - angle in degrees
 * @return float - sine value
 */
float fastSin(short thetaInDegrees)
{
	short deg = thetaInDegrees % 360;
	if (deg < 0) deg += 360;
	
	if (deg <= 90) return (float)RS_sin8192tab[deg] / 8192.0f;
	if (deg <= 180) return (float)RS_sin8192tab[180 - deg] / 8192.0f;
	if (deg <= 270) return -(float)RS_sin8192tab[deg - 180] / 8192.0f;
	return -(float)RS_sin8192tab[360 - deg] / 8192.0f;
}

/**
 * @brief Computes fast cosine from degrees	
 * 
 * @param thetaInDegrees - angle in degrees
 * @return float - cosine value
 */
float fasCos(short thetaInDegrees)
{
	return fastSin(thetaInDegrees + 90);
}

short fixedSin(short degrees) {
    degrees %= 360;
    if (degrees < 0) degrees += 360;
    if (degrees <= 90) return RS_sin8192tab[degrees];
    if (degrees <= 180) return RS_sin8192tab[180 - degrees];
    if (degrees <= 270) return -(short)RS_sin8192tab[degrees - 180];
    return -(short)RS_sin8192tab[360 - degrees];
}

short fixedCos(short degrees) {
    return fixedSin(degrees + 90);
}

/**
 * @brief Get the Ninja Rope angles based on a int value between 0-360
 * 
 * @param outX - output X component
 * @param outY - output Y component
 */
void getNinjaRopeAngles(short *outX, short *outY)
{
	*outX = fixedSin(Game_ninjaRopeAngle);
	*outY = -fixedCos(Game_ninjaRopeAngle);
}

/**
 * @brief Normalizes degrees to 0-359
 * 
 * @param degrees - degrees to normalize
 */
short getNormalizedDegrees(short degrees){
	degrees = degrees % 360;
	if(degrees < 0)
		degrees += 360;
	return degrees;
}

/**
 * @brief updates the current ninja rope angle based on rotation speed and direction
 */
void updateNinjaRopeCurrentAngle()
{
	short ax = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][0];
    short ay = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][1];
    short wx = Worm_x[(short)Worm_currentWorm];
    short wy = Worm_y[(short)Worm_currentWorm];

    // FIX: Use integer math to avoid FPU usage (Privilege Violation)
    short dx = wx - ax;
    short dy = ay - wy;
    
    // We want 0=Up, 90=Right.
    // fastAtan2 returns 0=Right (1,0), 90=Up (0,1).
    // dx is Right. -dy is Up (since Y grows down).
    // So fastAtan2(-dy, dx) -> 0=Right, 90=Up.
    // We want 0=Up (which is 90 in math), 90=Right (which is 0 in math).
    // So targetAngle = 90 - mathAngle.
    
    short mathAngle = fastAtan2(-dy, dx);
    short newAngle = 90 - mathAngle;
    
    // Normalize newAngle to 0-360
    while(newAngle < 0) newAngle += 360;
    while(newAngle >= 360) newAngle -= 360;

	// Make newAngle continuous with Game_ninjaRopeAngle
	short diff = (newAngle - Game_ninjaRopeAngle) % 360;
    if (diff > 180) diff -= 360;
    if (diff < -180) diff += 360;
    Game_ninjaRopeAngle += diff;
}

/**
 * @brief Updates the worm's position based on the current anchor, length, and angle
 */
void updateWormPosFromRope()
{
	short ax = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][0];
    short ay = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][1];
	
	// X = Center + Radius * Sin(theta)
	// Y = Center - Radius * Cos(theta)
	
	Worm_x[(short)Worm_currentWorm] = ax + (short)((long)Game_ninjaRopeLength * fixedSin(Game_ninjaRopeAngle) / SIN_SCALE);
    Worm_y[(short)Worm_currentWorm] = ay - (short)((long)Game_ninjaRopeLength * fixedCos(Game_ninjaRopeAngle) / SIN_SCALE);
}

/**
 * @brief Moves the worm along the ninja rope in or out
 * 
 * @param direction - -1 to move in, 1 to move out
 */
void NinjaRope_moveInOut(short direction)
{
	// Adjust length
	Game_ninjaRopeLength += (direction * 3);
	
	// Bound check min length
	if(Game_ninjaRopeLength <= 3) {
		// If we are super close, check if we can remove an anchor
		if(Game_ninjaRopeAnchorCount > 1) {
			Game_ninjaRopeAnchorCount--;
			
			// Recalculate properties for the previous anchor
			updateNinjaRopeCurrentAngle();
			
			// New length is distance to the *new* current anchor
			short nax = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][0];
			short nay = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][1];
			Game_ninjaRopeLength = dist(Worm_x[(short)Worm_currentWorm], Worm_y[(short)Worm_currentWorm], nax, nay);
		} else {
			// Cap at min length
			Game_ninjaRopeLength = 3;
		}
	}
	
	// Apply new position
	updateWormPosFromRope();
}

/**
 * @brief Changes the worms position based on ninja rope rotation
 * 
 * @return char - TRUE if the angle changed, FALSE otherwise
 */
char doNinjaRopeRotation()
{
	if (Game_ninjaRopeRotationDir != 0) {
        Game_ninjaRopeRotationSpeed += Game_ninjaRopeRotationDir;
        if (Game_ninjaRopeRotationSpeed > 5) Game_ninjaRopeRotationSpeed = 5;
        if (Game_ninjaRopeRotationSpeed < -5) Game_ninjaRopeRotationSpeed = -5;
    } else {
        if (Game_ninjaRopeRotationSpeed > 0) Game_ninjaRopeRotationSpeed--;
        else if (Game_ninjaRopeRotationSpeed < 0) Game_ninjaRopeRotationSpeed++;
    }

    if (Game_ninjaRopeRotationSpeed == 0) return FALSE;

    Game_ninjaRopeAngle += Game_ninjaRopeRotationSpeed;

    // Update worm position based on new angle and FIXED radius
	updateWormPosFromRope();

    return TRUE;
}


/**
 * @brief Checks if the old hit point should be removed
 * 
 * @return char - TRUE if it was removed, FALSE otherwise
 */
char checkToRemoveOldHitPoint()
{
	if (Game_ninjaRopeAnchorCount <= 1) return FALSE;

    short lastAngle = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][2];
    short lastDir = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][3];
    
    if ((lastDir == 1 && Game_ninjaRopeAngle < lastAngle) || 
	    (lastDir == -1 && Game_ninjaRopeAngle > lastAngle)) 
	{
		Game_ninjaRopeAnchorCount--;
		
		// We have unwrapped!
		// Update Angle relative to the *new* anchor
		updateNinjaRopeCurrentAngle();
		
		// Update Length relative to the *new* anchor
		short ax = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][0];
		short ay = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][1];
		Game_ninjaRopeLength = dist(Worm_x[(short)Worm_currentWorm], Worm_y[(short)Worm_currentWorm], ax, ay);
		
		return TRUE;
	}
    return FALSE;
}


/**
 * @brief Checks if the new rotation should add a new ninja rope point
 * 
 * @return char 
 */
char raycastForNewHitPoint()
{
	// if we're at maximum hit points already, gtfo
	if(Game_ninjaRopeAnchorCount >= MAX_NINJA_ROPE_POINTS)
		return FALSE;

	short wx = Worm_x[(short)Worm_currentWorm];
    short wy = Worm_y[(short)Worm_currentWorm];
    short ax = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][0];
    short ay = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][1];

    short dx = ax - wx;
    short dy = ay - wy;
    short d_val = Game_ninjaRopeLength; // Use our trusted length
    if (d_val == 0) return FALSE;
	
    short rayDirX = (short)((long)dx * 10 / d_val);
    short rayDirY = (short)((long)dy * 10 / d_val);

    RaycastHit hit;
    Game_raycast(wx, wy, rayDirX, rayDirY, FALSE, &hit);
	
	if(hit.hitType != RAY_HIT_NOTHING)
	{
		// Distance from current anchor to hit point
        short d = dist(hit.x, hit.y, ax, ay);
		short distFromWorm = dist(hit.x, hit.y, wx, wy);

		// Must be significantly far from the current anchor AND not hitting the worm itself/pixels next to worm
        if (d > 5 && distFromWorm > 8) {
			// add the new hit point:
			Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount][0] = hit.x;
			Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount][1] = hit.y;
			Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount][2] = Game_ninjaRopeAngle;
			Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount][3] = (Game_ninjaRopeRotationSpeed > 0) ? 1 : -1;
			
			// increment our counter
			Game_ninjaRopeAnchorCount++;
			
			// Update Length to be distance from *new* anchor to worm
			// Mathematically, this should be distFromWorm
			Game_ninjaRopeLength = distFromWorm;

			return TRUE;
		}
	}
	
	return FALSE;
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
	// get worm position
	const short wx = Worm_x[(short)Worm_currentWorm];
	const short wy = Worm_y[(short)Worm_currentWorm];

	RaycastHit hit;
	Game_raycast(wx, wy, dirX, dirY, FALSE, &hit);

	// draw a ray, regardless of whether we hit or not
	short sx = wx, sy = wy, ex = (wx+(dirX*30)), ey = (wy+(dirY*30));
	worldToScreen(&sx, &sy);
	worldToScreen(&ex, &ey);
	Draw_setRayLine(sx, sy, ex, ey);

	// if we did hit something, though, start the ninja rope mode
	if(hit.hitType != RAY_HIT_NOTHING)
	{	
		// save the first anchor point data
		Game_ninjaRopeAnchors[0][0] = hit.x;
		Game_ninjaRopeAnchors[0][1] = hit.y;
		
		// set our ninja rope state variables
		Game_ninjaRopeAnchorCount = 1;
		Game_ninjaRopeRotationDir = 0;
		Game_ninjaRopeRotationSpeed = 0;

		// compute or initial angle
		updateNinjaRopeCurrentAngle();
		Game_ninjaRopeAnchors[0][2] = Game_ninjaRopeAngle;
		Game_ninjaRopeAnchors[0][3] = 0; // always 0 for the first one
		
		// Set initial Length
		Game_ninjaRopeLength = dist(wx, wy, hit.x, hit.y);

		// enable the ninja rope mode
		Game_stateFlags |= gs_ninjaRopeMode;
		
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
		
		// Apply a little "hop" or momentum conservation
		Worm_yVelo[(short)Worm_currentWorm] = -2; 
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

	static char rotationLatch = 0;
	static char invertLatch = 1;

	// if left or right are pressed, we need to do ninja-rope rotation
	if(Keys_keyState(keyLeft|keyRight))
	{
		if(!rotationLatch) {
			rotationLatch = 1;
			// At 180 (Down), we want Right Key to Decrease Angle (Move Right). 
            // My engine: Right Key -> Game_ninjaRopeRotationDir = -1 * invertLatch.
            // We want Dir = -1. So invertLatch must be 1.
            // At 0 (Up), we want Right Key to Increase Angle (Move Right).
            // We want Dir = 1. So invertLatch must be -1.
			invertLatch = (angle>90 && angle<270) ? 1 : -1;
		}

		if(Keys_keyState(keyLeft))
		{
			// rotate left
			Game_ninjaRopeRotationDir = 1 * invertLatch;
		}
		else if(Keys_keyState(keyRight))
		{
			// rotate right
			Game_ninjaRopeRotationDir = -1 * invertLatch;
		}
	}
	else
	{
		// not rotating
		rotationLatch = 0;
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
