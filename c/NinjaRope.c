/*
	NinjaRope.c
	-----------

	This file handles the Ninja Rope weapon.

	Because this weapon has unique logic that needs to be updated every frame,
	and it's pretty substantial, it gets it's own file.

	This file will be in-line included in CharacterController.c
*/

#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

#define SIN_SCALE 8192

static const short SineTable[91] = { 0, 143, 286, 429, 571, 714, 856, 998, 1140, 1282, 1423, 1563, 1703, 1843, 1982, 2120, 2258, 2395, 2531, 2667, 2802, 2936, 3069, 3201, 3332, 3462, 3591, 3719, 3846, 3972, 4096, 4219, 4341, 4462, 4581, 4699, 4815, 4930, 5043, 5155, 5266, 5374, 5482, 5587, 5691, 5793, 5893, 5991, 6088, 6183, 6275, 6366, 6455, 6542, 6627, 6710, 6791, 6870, 6947, 7022, 7094, 7165, 7233, 7299, 7363, 7424, 7484, 7541, 7595, 7648, 7698, 7746, 7791, 7834, 7875, 7913, 7949, 7982, 8013, 8041, 8068, 8091, 8112, 8131, 8147, 8161, 8172, 8181, 8187, 8191, 8192 };

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
	
	if (deg <= 90) return (float)SineTable[deg] / 8192.0f;
	if (deg <= 180) return (float)SineTable[180 - deg] / 8192.0f;
	if (deg <= 270) return -(float)SineTable[deg - 180] / 8192.0f;
	return -(float)SineTable[360 - deg] / 8192.0f;
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
    if (degrees <= 90) return SineTable[degrees];
    if (degrees <= 180) return SineTable[180 - degrees];
    if (degrees <= 270) return -SineTable[degrees - 180];
    return -SineTable[360 - degrees];
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

    // standard atan2(y, x)
    // we want 0=Up, 90=Right, so atan2(dx, -dy)
    double angle_rad = atan2((double)(wx - ax), (double)(ay - wy));
    short newAngle = (short)(angle_rad * 180.0 / PI);

	// Make newAngle continuous with Game_ninjaRopeAngle
	short diff = (newAngle - Game_ninjaRopeAngle) % 360;
    if (diff > 180) diff -= 360;
    if (diff < -180) diff += 360;
    Game_ninjaRopeAngle += diff;
}

/**
 * @brief Moves the worm along the ninja rope in or out
 * 
 * @param direction - -1 to move in, 1 to move out
 */
void NinjaRope_moveInOut(short direction)
{
	short ax = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][0];
    short ay = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][1];
    short *wx = &Worm_x[(short)Worm_currentWorm];
    short *wy = &Worm_y[(short)Worm_currentWorm];

    short d = dist(*wx, *wy, ax, ay);
    
    if (direction < 0) { // Move in
        if (d <= 3) {
            if (Game_ninjaRopeAnchorCount > 1) {
                Game_ninjaRopeAnchorCount--;
				updateNinjaRopeCurrentAngle();
            } else {
                // At first hook point, don't move past it.
                *wx = ax;
                *wy = ay;
            }
        } else {
            // LERP 3 pixels in
            *wx += (short)(((long)(ax - *wx) * 3) / d);
            *wy += (short)(((long)(ay - *wy) * 3) / d);
        }
    } else { // Move out
        if (d == 0) {
            // can't lerp away if we are at the point, use Game_ninjaRopeAngle
            *wx += (short)((long)3 * fixedSin(Game_ninjaRopeAngle) / SIN_SCALE);
            *wy -= (short)((long)3 * fixedCos(Game_ninjaRopeAngle) / SIN_SCALE);
        } else {
            // LERP 3 pixels out
            *wx -= (short)(((long)(ax - *wx) * 3) / d);
            *wy -= (short)(((long)(ay - *wy) * 3) / d);
        }
    }
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

    // Update worm position based on new angle and fixed radius
    short ax = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][0];
    short ay = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][1];
    short d = dist(Worm_x[(short)Worm_currentWorm], Worm_y[(short)Worm_currentWorm], ax, ay);

    Worm_x[(short)Worm_currentWorm] = ax + (short)((long)d * fixedSin(Game_ninjaRopeAngle) / SIN_SCALE);
    Worm_y[(short)Worm_currentWorm] = ay - (short)((long)d * fixedCos(Game_ninjaRopeAngle) / SIN_SCALE);

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
    
    if (lastDir == 1) {
        if (Game_ninjaRopeAngle < lastAngle) {
            Game_ninjaRopeAnchorCount--;
            return TRUE;
        }
    } else if (lastDir == -1) {
        if (Game_ninjaRopeAngle > lastAngle) {
            Game_ninjaRopeAnchorCount--;
            return TRUE;
        }
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

	// even if we git a hit, don't add it if it's closer to the last point than this
	const short minDistanceForNewPoint = 5;

	short wx = Worm_x[(short)Worm_currentWorm];
    short wy = Worm_y[(short)Worm_currentWorm];
    short ax = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][0];
    short ay = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][1];

    short dx = ax - wx;
    short dy = ay - wy;
    short d_val = dist(wx, wy, ax, ay);
    if (d_val == 0) return FALSE;
    short rayDirX = (short)((long)dx * 10 / d_val);
    short rayDirY = (short)((long)dy * 10 / d_val);

    RaycastHit hit = Game_raycast(wx, wy, rayDirX, rayDirY, FALSE);
	
	if(hit.hitType != RAY_HIT_NOTHING)
	{
		// Distance from current anchor to hit point
        short d = dist(hit.x, hit.y, ax, ay);
        if (d > minDistanceForNewPoint) {
			// add the new hit point:
			Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount][0] = hit.x;
			Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount][1] = hit.y;
			Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount][2] = Game_ninjaRopeAngle;
			Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount][3] = (Game_ninjaRopeRotationSpeed > 0) ? 1 : -1;
			
			// increment our counter
			Game_ninjaRopeAnchorCount++;

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

	RaycastHit hit = Game_raycast(wx, wy, dirX, dirY, FALSE);

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
			invertLatch = (angle>90 && angle<270) ? -1 : 1;
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