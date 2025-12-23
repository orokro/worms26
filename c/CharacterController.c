/*
	CharacterController.c
	---------------------

	During the turn mode, and only during the turn mode, the player
	can move the worm around.

	This file handles all input for moving the worm, as well as actually moving the worm.

	The worm can be moved in various ways:
		- Walking left/right
		- Jumping forward
		- Backfliping
		- Ninja rope
		- Bungee cord
		- Parachute
		- Jet pack

	We will handle all of them in here...

	C Source File
	Created 11/22/2017; 2:56:22 AM
*/

// includes
#include "Main.h"
#include "Game.h"
#include "Keys.h"
#include "Map.h"
#include "CharacterController.h"
#include "PhysCol.h"
#include "Worms.h"
#include "Weapons.h"
#include "Camera.h"

// local defines: the max slope a worm can walk up:
#define MAX_WORM_SLOPE 6

// these will be updated by CharacterController_update()
// so we don't have to constantly calculate them...
short *wX, *wY;
unsigned short wormMask = 0;

/**
 * Handles moving the worm in the character controller, if its on the ground.
 */
void wormWalk()
{

	// test keys for a possible direction to walk
	char moveDir = 0;

	// if jump was pressed, jump and exit
	if (Keys_keyDown(keyJump))
	{
		Physics_setVelocity(&Worm_physObj[(short)Worm_currentWorm], ((Worm_dir & wormMask) ? -2 : 2), -5, FALSE, TRUE);

		// set jump animation
		Game_wormAnimState = ANIM_JUMP;
		Game_wormAnimTimer = 0;

		return;

		// backflip
	}
	else if (Keys_keyDown(keyBackflip))
	{
		Physics_setVelocity(&Worm_physObj[(short)Worm_currentWorm], ((Worm_dir & wormMask) ? 1 : -1), -6, FALSE, TRUE);

		// Set Animation State
		Game_wormAnimState = ANIM_BACKFLIP;
		Game_wormAnimTimer = 0;

		// Store the direction we were facing when we started the flip
		// (We need this because the worm might spin, but the sequence depends on start dir)

		Game_wormFlipStartDir = (Worm_dir & wormMask);
		return;

		// test for directions left/right
	}
	else if (Keys_keyState(keyLeft))
	{
		moveDir = -1;
		Camera_clearFocus();
	}
	else if (Keys_keyState(keyRight))
	{
		moveDir = 1;
		Camera_clearFocus();
	}

	// if we are not moving, gtfo
	if (moveDir == 0)
		return;

	// change the direction the worm is facing, even if we dont move:
	if (moveDir < 0)
		Worm_dir |= wormMask;
	else
		Worm_dir &= ~wormMask;

	// worms can't move up slops steeper than MAX_WORM_SLOPE pixels.
	// lets test pixels to the left or right +MAX_WORM_SLOPE pixels higher
	// if we find land, can't move
	if (Map_testPoint((*wX) + moveDir, (*wY) - MAX_WORM_SLOPE) == FALSE)
	{
		// so the map position is free, now lets move down till we hit land:
		short y = 0;
		for (y = 0; y < MAX_WORM_SLOPE; y++)
			if (Map_testPoint((*wX) + moveDir, (*wY) - MAX_WORM_SLOPE + y))
				break;

		// save the worms new position:
		*wX += moveDir;
		*wY = (short)((*wY) - MAX_WORM_SLOPE + y);

		// unsettle worm
		Worm_physObj[(short)Worm_currentWorm].staticFrames = 0;
		Worm_settled &= ~wormMask;
	}
}


/**
 * This method handles weapon-specific logic if a weapon is currently selected
 */
void wormWeapon()
{
	// if the weapon has usesCursor and we haven't picked a spot yet, we should goto cursor mode
	if ((Game_currentWeaponProperties & usesCursor) && (Game_currentWeaponState & targetPicked) == 0)
	{
		// if action was pressed, place the x-mark
		if (Keys_keyUp(keyAction))
		{
			Game_changeMode(gameMode_Cursor);
		}

		// no need to continue
		return;

		// otherwise, if a target WAS picked, use escape to cancel instead of pause
	}
	else
	{
		if (Keys_keyUp(keyEscape))
		{
			// un-set target picked
			Game_currentWeaponState &= ~targetPicked;
			Game_xMarkPlaced = FALSE;

			// no need to continue
			return;
		}
	}

	// if uses aim we should let them press up and down
	if (Game_currentWeaponProperties & usesAim)
	{
		if (Keys_keyState(keyUp))
			Game_aimAngle++;
		else if (Keys_keyState(keyDown))
			Game_aimAngle--;

		// bound check
		if (Game_aimAngle <= 0)
			Game_aimAngle = 0;
		else if (Game_aimAngle > 18)
			Game_aimAngle = 18;
	}

	// if this weapon uses charge
	if (Game_currentWeaponProperties & usesCharge)
	{
		// if the action key was let go we should fire the weapon at it's current charge, reset charge and exit
		if (Keys_keyUp(keyAction))
		{
			Weapons_fire(Game_currentWeaponCharge);
			Game_currentWeaponCharge = 0;

			// end turn because we fired a weapon
			//	Game_changeMode(gameMode_TurnEnd);

			return;

			// if the key is pressed, we should add to the charge:
		}
		else if (Keys_keyState(keyAction))
		{
			Game_currentWeaponCharge += 18;
			if (Game_currentWeaponCharge >= 255)
				Game_currentWeaponCharge = 255;
		}

		// otherwise, proceed since this weapon isn't chargeable
	}
	else
	{

		// if 2nd was pressed, activate weapon
		if (Keys_keyUp(keyAction))
		{
			Weapons_fire(Game_currentWeaponCharge);
			Game_currentWeaponCharge = 0;

			// end turn because we fired a weapon
			// Game_changeMode(gameMode_TurnEnd);
			return;
		} // end if action was pressed
	} // end if uses charge
}

// --------------------------------------------------------------------------------------------------------------------------------------

/**
 * handles all update frames for controlling worms in the turn game mode
 */
void CharacterController_update()
{
	// if the camera is being controlled, no need to control the character
	if (Keys_keyState(keyCameraControl))
		return;

	// worm mask...
	wormMask = 1;
	wormMask = (unsigned short)((unsigned short)wormMask << (Worm_currentWorm));

	// save references to our pos
	wX = &Worm_x[(short)Worm_currentWorm];
	wY = &Worm_y[(short)Worm_currentWorm];

	// if the worm is grounded, we should test for walking:
	if (Worm_onGround & wormMask)
		wormWalk();

	// if the user has a weapon selected and check for first-frames of keyDown
	if (Game_currentWeaponSelected != -1)
		wormWeapon();

	// TO-DO: implement parachute, bungee, and ninja rope
}