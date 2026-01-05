/*
	NinjaRope.c
	-----------

	This file handles the Ninja Rope weapon.

	Because this weapon has unique logic that needs to be updated every frame,
	and it's pretty substantial, it gets it's own file.

	This file will be in-line included in CharacterController.c
*/

// --- CONSTANTS ---
#define GRAVITY 0.25f
#define ROPE_CLIMB_SPEED 2.0f
#define ROPE_SWING_ACCEL 0.008f
#define ROPE_DAMPING 0.99f
#define WORM_COLLISION_RADIUS 4
#define MIN_ROPE_LENGTH 10.0f
#define TWO_PI 6.28318531f


// --- FAST MATH IMPLEMENTATIONS ---

/**
 * @brief Fast Sine Approximation (High performance, reasonable precision)
 * Domain: -PI to PI
 */
static float fast_sin(float x)
{

    // Wrap to range -PI to PI
    while (x < -PI) x += TWO_PI;
    while (x >  PI) x -= TWO_PI;

    // 1.27323954  = 4/pi
    // 0.405284735 = 4/pi^2
    float z = x;
    if (z < 0.0f) z = -z;
    
    float y = 1.27323954f * x - 0.405284735f * x * z;

    // Optional precision improvement (extra cost, smoother curve)
    // Q = 0.225
    float y_abs = y; 
    if(y_abs < 0.0f) 
		y_abs = -y_abs;		
    y = 0.225f * (y * y_abs - y) + y;

    return y;
}


/**
 * @brief Fast Cosine Approximation
 */
static float fast_cos(float x)
{
    return fast_sin(x + HALF_PI);
}


/**
 * @brief Fast Atan2 Approximation
 * Error < 0.005 radians
 */
static float fast_atan2(float y, float x)
{
    if (x == 0.0f)
	{
        if (y > 0.0f) return HALF_PI;
        if (y < 0.0f) return -HALF_PI;
        return 0.0f;
    }

    float abs_y = y; 
    if(abs_y < 0.0f)
		abs_y = -abs_y;
    
    float angle;
    if (x >= 0.0f)
	{
        float r = (x - abs_y) / (x + abs_y);
        angle = 0.785398163f - 0.785398163f * r; // PI/4 * (1 - r)
    }
	else
	{
        float r = (x + abs_y) / (abs_y - x);
        angle = 2.35619449f - 0.785398163f * r; // 3*PI/4 - PI/4 * r
    }
    
    return y < 0.0f ? -angle : angle;
}


/**
 * @brief Helper: Check if worm body overlaps land
 */
static char CheckWormCollision(short x, short y)
{
    // 1. Check center
    if (Map_testPoint(x, y)) return 1;

    // 2. Check diamond shape (faster than circle)
    if (Map_testPoint(x + WORM_COLLISION_RADIUS, y)) return 1;
    if (Map_testPoint(x - WORM_COLLISION_RADIUS, y)) return 1;
    if (Map_testPoint(x, y + WORM_COLLISION_RADIUS)) return 1;
    if (Map_testPoint(x, y - WORM_COLLISION_RADIUS)) return 1;

    return 0;
}


/**
 * @brief Updates the current angle of the ninja rope based on the worm's position
 */
static void updateNinjaRopeCurrentAngle()
{
	const short wx = Worm_x[(short)Worm_currentWorm];
	const short wy = Worm_y[(short)Worm_currentWorm];
	const short ax = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][0];
	const short ay = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][1];

	Game_ninjaRopeAngle = (short)(fast_atan2((float)(wy - ay), (float)(wx - ax)) * 57.29578f);
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
void wormNinjaRope()
{
    // 1. SAFETY CHECKS
    if (!(Game_stateFlags & gs_ninjaRopeMode))
		return;
    if (Game_ninjaRopeAnchorCount == 0)
		return;

    // Get shortcuts to current worm data
    short wIdx = (short)Worm_currentWorm;
    short wx = Worm_x[wIdx];
    short wy = Worm_y[wIdx];

    // 2. STATE MANAGEMENT (PRECISION HANDLING)
    static float fAngle = 0.0f;
    static float fLength = 0.0f;
    static float fAngVel = 0.0f;
    static short lastActiveWorm = -1;

    short activePivotX = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][0];
    short activePivotY = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 1][1];

    // Detect if we just entered mode or switched worms, reset physics state
    if (lastActiveWorm != wIdx || fLength == 0.0f)
	{
        lastActiveWorm = wIdx;
        fLength = (float)Game_ninjaRopeLength;
        // Initialize angle based on current position relative to pivot
        fAngle = fast_atan2((float)(wy - activePivotY), (float)(wx - activePivotX));
        fAngVel = 0.0f;
    }

    // Keep track of where we started this frame (Last Known Good)
    short startX = wx;
    short startY = wy;

    // 3. INPUT: ROPE LENGTH (Climb/Extend)
    float oldLength = fLength;

    if (!(Game_stateFlags & gs_bungeeMode)) {
        if (Keys_keyState(keyUp)) {
            fLength -= ROPE_CLIMB_SPEED;
            if (fLength < MIN_ROPE_LENGTH) fLength = MIN_ROPE_LENGTH;
        }
        if (Keys_keyState(keyDown)) {
            fLength += ROPE_CLIMB_SPEED;
        }

        // Test if climbing hit a ceiling
        short testX = activePivotX + (short)(fast_cos(fAngle) * fLength);
        short testY = activePivotY + (short)(fast_sin(fAngle) * fLength);

        // no climb if we collide
        if (CheckWormCollision(testX, testY))
            fLength = oldLength; 
    }
    
    // 4. PHYSICS: ANGULAR MOMENTUM
    // Gravity: pulls towards PI/2 (down). Torque = Gravity/Len * cos(angle)
    fAngVel += (GRAVITY / fLength) * fast_cos(fAngle);

    // Input: Swing
    char isBelow = wy > activePivotY;
    float swingForce = (Game_stateFlags & gs_bungeeMode) ? (ROPE_SWING_ACCEL * 0.25f) : ROPE_SWING_ACCEL;

	// Invert controls if below pivot
    if (Keys_keyState(keyLeft))
        fAngVel += isBelow ? swingForce : -swingForce;
    if (Keys_keyState(keyRight))
        fAngVel -= isBelow ? swingForce : -swingForce;
    

    fAngVel *= ROPE_DAMPING;

    // 5. CALCULATE PROPOSED POSITION
    float nextAngle = fAngle + fAngVel;

    short nextX = activePivotX + (short)(fast_cos(nextAngle) * fLength);
    short nextY = activePivotY + (short)(fast_sin(nextAngle) * fLength);

    // 6. COLLISION CHECK (BODY)
    if (CheckWormCollision(nextX, nextY))
	{
        // Bonk! Stop momentum
        fAngVel = 0.0f;
		
        // Revert to start of frame
        nextX = startX;
        nextY = startY;
        nextAngle = fAngle;
    }
    else
	{
        // Path is clear, commit the angle
        fAngle = nextAngle;
    }

    // 7. UNWRAP CHECK (Reverse Raycast: Worm -> Prev Pivot)
    char didUnwrap = 0;
    if (Game_ninjaRopeAnchorCount > 1)
	{
        short prevX = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 2][0];
        short prevY = Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount - 2][1];

        RaycastHit hit;
        // Cast FROM worm TO previous pivot
        Game_raycast(nextX, nextY, prevX - nextX, prevY - nextY, FALSE, &hit);

        short distToPrev = dist(nextX, nextY, prevX, prevY);
        short distToHit = (hit.hitType == RAY_HIT_LAND) ? dist(nextX, nextY, hit.x, hit.y) : 32000;

        // If we hit nothing, OR we made it all the way back to the previous pivot (with buffer)
        if (hit.hitType == RAY_HIT_NOTHING || distToHit >= distToPrev - 5)
		{
            
            Game_ninjaRopeAnchorCount--;
            activePivotX = prevX;
            activePivotY = prevY;

            // Recalculate physics for new pivot
            float newLength = (float)distToPrev;
            if (newLength > 0.1f) {
                fAngVel = fAngVel * (fLength / newLength);
                fLength = newLength;
            }

            fAngle = fast_atan2((float)(nextY - activePivotY), (float)(nextX - activePivotX));
            didUnwrap = 1;
        }
    }

    // 8. WRAP CHECK (Reverse Raycast: Worm -> Current Pivot)
    if (!didUnwrap)
	{
        RaycastHit hit;
        // Cast FROM worm TO current pivot
        Game_raycast(nextX, nextY, activePivotX - nextX, activePivotY - nextY, FALSE, &hit);

        if (hit.hitType == RAY_HIT_LAND)
		{
            short distToHit = dist(nextX, nextY, hit.x, hit.y);
            short distToPivot = dist(nextX, nextY, activePivotX, activePivotY);

            // If we hit a wall BEFORE reaching the pivot
            if (distToHit < distToPivot - 5)
			{
                
                if (Game_ninjaRopeAnchorCount < MAX_NINJA_ROPE_POINTS)
				{
                    Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount][0] = hit.x;
                    Game_ninjaRopeAnchors[Game_ninjaRopeAnchorCount][1] = hit.y;
                    Game_ninjaRopeAnchorCount++;

                    // New length is distance to the wall we just hit
                    float newLength = (float)distToHit;
                    if (newLength < 1.0f)
						newLength = 1.0f;

                    // Conserve angular momentum
                    fAngVel = fAngVel * (fLength / newLength);
                    fLength = newLength;

                    // Update active pivot
                    activePivotX = hit.x;
                    activePivotY = hit.y;
                    fAngle = fast_atan2((float)(nextY - activePivotY), (float)(nextX - activePivotX));
                } 
				else {
					// Rope too complex
                    fAngVel = 0; 
                }
            }
        }
    }

    // 9. EXIT CONDITION
    if (Keys_keyDown(keyAction))
	{
        float linearSpeed = fAngVel * fLength;
        Worm_xVelo[wIdx] = (char)(-fast_sin(fAngle) * linearSpeed);
        Worm_yVelo[wIdx] = (char)(fast_cos(fAngle) * linearSpeed);
		Physics_setVelocity(&Worm_physObj[(short)Worm_currentWorm], Worm_xVelo[wIdx], Worm_yVelo[wIdx], FALSE, TRUE);
        Game_stateFlags &= ~(gs_ninjaRopeMode | gs_bungeeMode);
        
        // Reset state for next use
        fLength = 0; 
        return;
    }

    // 10. FINAL COMMIT TO GLOBALS
    Worm_x[wIdx] = nextX;
    Worm_y[wIdx] = nextY;

    Game_ninjaRopeLength = (short)fLength;
    Game_ninjaRopeAngle = (short)(fAngle * 57.29578f);
}
