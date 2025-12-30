
/*
	CharacterController.c
	---------------------

	During the turn mode, and only during the turn mode, the player
	can move the worm around.

	This file handles all input for moving the worm, as well as actually moving the worm.

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
#include "Match.h"
#include "StatusBar.h"
#include "Draw.h"


// local defines: the max slope a worm can walk up:
#define MAX_WORM_SLOPE 6


// these will be updated by CharacterController_update()
// so we don't have to constantly calculate them...
short *wX, *wY;
unsigned short wormMask = 0;

// Bungee vars
static short lastGroundX = 0;
static short lastGroundY = 0;
static short ungroundedTimer = 0;
static char bungeeLatch = 0;
static char bungeeDisabled = 0;
static short bungeeValidX = 0;
static short bungeeValidY = 0;


/**
 * Handles moving the worm in the character controller, if its on the ground.
 */
void wormWalk()
{
	// Update last ground position if on ground
    lastGroundX = *wX;
    lastGroundY = *wY;
    ungroundedTimer = 0;
    bungeeDisabled = 0;

	// test keys for a possible direction to walk
	char moveDir = 0;

	// if jump was pressed, jump and exit
	if (Keys_keyDown(keyJump))
	{
		Physics_setVelocity(&Worm_physObj[(short)Worm_currentWorm], ((Worm_dir & wormMask) ? -2 : 2), -5, FALSE, TRUE);

		// set jump animation
		Game_wormAnimState = ANIM_JUMP;
		Game_wormAnimTimer = 0;
        
        // Disable bungee until grounded again
        bungeeDisabled = 1;

		return;

		// backflip
	}
	else if (Keys_keyDown(keyBackflip))
	{
		CharacterController_doBackflip();
        bungeeDisabled = 1;
		return;

	// test for directions left/right
	}
	else if (Keys_keyState(keyLeft))
	{
		moveDir = (Game_stateFlags & gs_fastWalk) ? -2 : -1;
		Camera_clearUnless(&Worm_x[(short)Worm_currentWorm], &Worm_y[(short)Worm_currentWorm]);
	}
	else if (Keys_keyState(keyRight))
	{
		moveDir = (Game_stateFlags & gs_fastWalk) ? 2 : 1;
		Camera_clearUnless(&Worm_x[(short)Worm_currentWorm], &Worm_y[(short)Worm_currentWorm]);
	}

	if ((Game_currentWeaponState & targetPicked) && Keys_keyUp(keyEscape))
	{
		// un-set target picked
		Game_currentWeaponState &= ~targetPicked;

		// no need to continue
		return;
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
 * @brief Adjusts aim for weapons that require it
 * 
 */
void adjustAim(){

	// if uses aim we should let them press up and down
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


/**
 * This method handles weapon-specific logic if a weapon is currently selected
 */
void wormWeapon()
{
	// handle meta weapons being pressed
	if((Game_currentWeaponProperties & isMeta)!=0 && Keys_keyUp(keyAction))
	{
		switch(Game_currentWeaponSelected)
		{
			case WLowGravity:
				Game_stateFlags |= gs_lowGravity;
				StatusBar_showMessage("Low Gravity Activated!");
				break;
			
			case WFastWalk:
				Game_stateFlags |= gs_fastWalk;
				StatusBar_showMessage("Fast Walk Activated!");
				break;

			case WLaserSight:
				Game_stateFlags |= gs_laserSight;
				StatusBar_showMessage("Laser Sight Activated!");
				break;

			case WJetPack:
				Game_stateFlags |= gs_jetpackMode;
				Game_jetPackFuel = MAX_JETPACK_FUEL;
				break;

			case WBungeeCord:
				Game_stateFlags |= gs_bungeeActive;
				StatusBar_showMessage("Bungee Equipped!");
				break;

			case WSkipGo:
				Game_changeMode(gameMode_TurnEnd);
				StatusBar_showMessage("Turn Skipped");
				break;

			case WSurrender:
				Game_surrenderedTeam = Game_currentTeam;
				Game_changeMode(gameMode_GameOver);
				return;
				break;

			case WSelectWorm:
				Game_turn--;
				Game_currentTeam = (Game_currentTeam==1 ? 0 : 1);
				Game_changeMode(gameMode_WormSelect);
				CharacterController_weaponConsumed(TRUE);
				return;
				break;

			case WFreeze:
				Game_stateFlags |= gs_lockTurnEnd;
				Game_stateFlags |= ((Game_currentTeam==0) ? gs_team1Frozen : gs_team2Frozen);
				break;

			case WInvisibility:
				Game_stateFlags |= ((Game_currentTeam==0) ? gs_team1Invisible : gs_team2Invisible);
				break;
		}

		// handle clean up logic
		CharacterController_weaponConsumed(FALSE);
		return;
	}

	if ((Game_currentWeaponProperties & usesCursor) && (Game_currentWeaponState & targetPicked) == 0)
	{
		Game_xMarkAllowedOverLand = !(Game_currentWeaponSelected==WTeleport);
		if (Keys_keyUp(keyAction))
			Game_changeMode(gameMode_Cursor);
		return;
	}

	if (Game_currentWeaponProperties & usesCharge)
	{
		if (Keys_keyUp(keyAction))
		{
			const char shouldConsumeWeapon = Weapons_fire(Game_currentWeaponCharge);
			Game_currentWeaponCharge = 0;
			if(shouldConsumeWeapon && Game_weaponUsesRemaining==-1)
				CharacterController_weaponConsumed(FALSE);
		}
		else if (Keys_keyState(keyAction))
		{
			Game_currentWeaponCharge += 18;
			if (Game_currentWeaponCharge >= 255)
				Game_currentWeaponCharge = 255;
		}
	}
	else
	{
		if (Keys_keyUp(keyAction))
		{
			const char shouldConsumeWeapon = Weapons_fire(Game_currentWeaponCharge);
			Game_currentWeaponCharge = 0;
			if(shouldConsumeWeapon && Game_weaponUsesRemaining==-1)
				CharacterController_weaponConsumed(FALSE);
		} 
	}
}


/**
 * @brief Handles parachute logic
 */
void wormParachute()
{
	// Auto-disable if grounded
    if(Worm_onGround & wormMask) {
        Game_stateFlags &= ~gs_parachuteMode;
    }
    
    // Toggle Logic
    if(!(Worm_onGround & wormMask)) 
    {
		if(Keys_keyDown(keyAction)) 
		{
			if((Game_currentWeaponSelected == WParachute) && !(Game_stateFlags & gs_parachuteMode))
			{				
				Game_stateFlags |= gs_parachuteMode;
				Worm_xVelo[(short)Worm_currentWorm] = 0;
				Worm_yVelo[(short)Worm_currentWorm] = 0;

				CharacterController_weaponConsumed(TRUE); 					
			}else{
				Game_stateFlags &= ~gs_parachuteMode;
			}
		}
    }

    // Flight Logic
    if(Game_stateFlags & gs_parachuteMode)
    {
        Worm_physObj[(short)Worm_currentWorm].staticFrames = 0;
        Worm_settled &= ~wormMask;

        if(Keys_keyState(keyLeft)) {
            Worm_xVelo[(short)Worm_currentWorm] = -1;
            Worm_dir |= wormMask; 
        } else if(Keys_keyState(keyRight)) {
            Worm_xVelo[(short)Worm_currentWorm] = 1;
            Worm_dir &= ~wormMask; 
        } else {
            Worm_xVelo[(short)Worm_currentWorm] = 0;
        }

        if(Worm_xVelo[(short)Worm_currentWorm] == 0 && Game_wind != 0 && (Game_timer % 4 == 0)) {
            Worm_xVelo[(short)Worm_currentWorm] = (Game_wind > 0) ? 1 : -1;
        }

        Worm_yVelo[(short)Worm_currentWorm] = (Game_timer % 2 == 0) ? -1 : 0;
    }
}


/**
 * @brief Handles Bungee logic
 */
void wormBungee()
{
    if((Game_stateFlags & gs_bungeeActive) || (Game_currentWeaponSelected == WBungeeCord))
    {
        if(Worm_onGround & wormMask) {
            if(Game_stateFlags & gs_bungeeMode) {
                 Game_stateFlags &= ~gs_bungeeMode; 
            }
            lastGroundX = *wX;
            lastGroundY = *wY;
            ungroundedTimer = 0;
            bungeeDisabled = 0;
        } 
        else if(!(Game_stateFlags & gs_bungeeMode) && !bungeeDisabled)
        {
            ungroundedTimer++;
            if(ungroundedTimer >= 3) 
            {
                // Raycast straight down from center of worm
                RaycastHit hit = Game_raycast(*wX, *wY, 0, 1, FALSE);
                short dist = hit.y - *wY;
                
                // Activation Condition: falling over a gap or water
                if(hit.hitType == RAY_HIT_NOTHING || dist > 40) {
                    Game_stateFlags |= gs_bungeeMode;
                    
                    // If we used the selected weapon, consume it
                    if(Game_currentWeaponSelected == WBungeeCord) {
                        CharacterController_weaponConsumed(TRUE);
                    }
                    
                    // Clear active flag so it's single-use
                    Game_stateFlags &= ~gs_bungeeActive;
                }
            }
        }
    }

    if(Game_stateFlags & gs_bungeeMode)
    {
        // Check for collisions to prevent teleporting
        if(Worm_physObj[(short)Worm_currentWorm].col.collisions != 0) {
            // "Bonk" - Revert to last valid position
            *wX = bungeeValidX;
            *wY = bungeeValidY;
            Worm_xVelo[(short)Worm_currentWorm] = 0;
            Worm_yVelo[(short)Worm_currentWorm] = 0;
        } else {
            // Update valid position
            bungeeValidX = *wX;
            bungeeValidY = *wY;
        }

        // Auto-disable if grounded (extra check inside mode block)
        if(Worm_onGround & wormMask) {
            Game_stateFlags &= ~gs_bungeeMode;
            return;
        }

        if(Keys_keyState(keyAction)) {
            if(!bungeeLatch) {
                bungeeLatch = 1;
                Game_stateFlags &= ~gs_bungeeMode;
                ungroundedTimer = 0; // Reset timer to prevent instant re-activation
                Worm_yVelo[(short)Worm_currentWorm] = -4; 
                return;
            }
        } else {
            bungeeLatch = 0;
        }

        Worm_physObj[(short)Worm_currentWorm].staticFrames = 0;
        Worm_settled &= ~wormMask;

        // Draw Line (World to Screen translation)
        short sx = lastGroundX, sy = lastGroundY, ex = *wX, ey = *wY;
        worldToScreen(&sx, &sy);
        worldToScreen(&ex, &ey);
        Draw_setRayLine(sx, sy, ex, ey);

        long dx = (long)*wX - lastGroundX;
        long dy = (long)*wY - lastGroundY;
        long distSq = dx*dx + dy*dy;
        long maxLenSq = 40L * 40L;

        if(distSq > maxLenSq)
        {
            float d = sqrt((float)distSq);
            float ratio = 40.0f / d;
            
            *wX = lastGroundX + (short)(dx * ratio);
            *wY = lastGroundY + (short)(dy * ratio);
            
            float nx = dx / d;
            float ny = dy / d;
            float vx = (float)Worm_xVelo[(short)Worm_currentWorm];
            float vy = (float)Worm_yVelo[(short)Worm_currentWorm];
            float dot = vx * nx + vy * ny;
            
            if(dot > 0) {
                float bounce = 0.5f; 
                float impulse = (1.0f + bounce) * dot;
                vx -= impulse * nx;
                vy -= impulse * ny;
                Worm_xVelo[(short)Worm_currentWorm] = (char)vx;
                Worm_yVelo[(short)Worm_currentWorm] = (char)vy;
            }
        }

        if(Keys_keyState(keyLeft)) {
            Worm_xVelo[(short)Worm_currentWorm] -= 1;
        } else if(Keys_keyState(keyRight)) {
            Worm_xVelo[(short)Worm_currentWorm] += 1;
        }
    }
}


/**
 * @brief Handles jetpack logic
 */
void wormJetpack()
{
	if(Game_stateFlags & gs_jetpackMode)
    {
		// unsettle worm
        Worm_physObj[(short)Worm_currentWorm].staticFrames = 0;
        Worm_settled &= ~wormMask;

        if(Keys_keyState(keyLeft)) 
		{
            Worm_xVelo[(short)Worm_currentWorm] = -1;
            Worm_dir |= wormMask; 
        }
		else if(Keys_keyState(keyRight))
		{
            Worm_xVelo[(short)Worm_currentWorm] = 1;
            Worm_dir &= ~wormMask; 
        }
		
		if (Keys_keyState(keyUp | keyDown))
		{
			Worm_yVelo[(short)Worm_currentWorm] = Keys_keyState(keyDown) ? 1 : -3;
		}
		else
		{
            // Worm_xVelo[(short)Worm_currentWorm] = 0;
			Worm_yVelo[(short)Worm_currentWorm] = (Game_timer % 2 == 0) ? -1 : 0;
        }

		// exit early if action is pressed
		if(Keys_keyDown(keyAction)) 
		{
			Game_stateFlags &= ~gs_jetpackMode; 
			return;
		}

		if(Keys_keyState(keyUp | keyDown | keyLeft | keyRight)){
			Game_jetPackFuel--;
			if(Game_jetPackFuel <= 0){
				Game_stateFlags &= ~gs_jetpackMode;
				StatusBar_showMessage("Jet Pack Out of Fuel!");
			}
		}

        if(Worm_xVelo[(short)Worm_currentWorm] == 0 && Game_wind != 0 && (Game_timer % 4 == 0)) {
			Worm_xVelo[(short)Worm_currentWorm] += (Game_wind > 0) ? 1 : -1;
        }
    }
}


/**
 * @brief Initiates a backflip for the current worm
 */
void CharacterController_doBackflip()
{
	Physics_setVelocity(&Worm_physObj[(short)Worm_currentWorm], ((Worm_dir & wormMask) ? 1 : -1), -6, FALSE, TRUE);
	Game_wormAnimState = ANIM_BACKFLIP;
	Game_wormAnimTimer = 0;
	Game_wormFlipStartDir = (Worm_dir & wormMask);
}


/**
 * handles all update frames for controlling worms in the turn game mode
 */
void CharacterController_update()
{
	// GTFO if camera is being manually controlled
	if (Keys_keyState(keyCameraControl))
		return;

	wormMask = (unsigned short)1 << (Worm_currentWorm);
	wX = &Worm_x[(short)Worm_currentWorm];
	wY = &Worm_y[(short)Worm_currentWorm];

	if(Weapon_superSheepDir != SHEEP_INACTIVE)
		return;

	// if we're in the ANIM_TORCH mode, always move the direction it's facing as if its auto-walking
	// if (Game_wormAnimState == ANIM_TORCH)
	// {
	// 	// save the worms new position:
	// 	*wX += (Worm_dir & wormMask) ? -1 : 1;

	// 	// unsettle worm
	// 	Worm_physObj[(short)Worm_currentWorm].staticFrames = 0;
	// 	Worm_settled &= ~wormMask;
	// }

    wormParachute();
    wormBungee();
    wormJetpack();

	if (Worm_onGround & wormMask)
		wormWalk();

	if (Game_currentWeaponSelected != -1)
		wormWeapon();

	if ((Game_currentWeaponProperties & usesAim) || (Game_currentWeaponState & keepAimDuringUse))
		adjustAim();
}


/**
 * @brief handles clean up after weapon has been decidedly used
 */
void CharacterController_weaponConsumed(char noEndTurn){

	// decrement weapon count
	Match_teamWeapons[(short)Game_currentTeam][(short)Game_currentWeaponSelected]--;

	// reset weapon selection, save last weapon selected
	if(Game_currentWeaponSelected!=-1)
		Game_lastWeaponSelected = Game_currentWeaponSelected;
	Game_currentWeaponSelected = -1;

	// end turn if necessary
	if(noEndTurn || (Game_currentWeaponProperties & doesntEndTurn))
		return;
	Game_changeMode(gameMode_TurnEnd);
}
