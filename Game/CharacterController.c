// C Source File
// Created 11/22/2017; 2:56:22 AM

#include "Main.h"

/*
	CharacterController
	-------------------
	
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
		- Jetpack
		
	We will handle all of them in here... 
*/

// local defines: the max slope a worm can walk up:
#define MAX_WORM_SLOPE 6

// these will be updated by CharacterController_update()
// so we don't have to constantly calculate them...
short *wX, *wY;
unsigned long wormMask=0;

/**
	* Handles moving the worm in the character controller, if its on the ground.
*/
void wormWalk()
{
	
	// test keys for a possible direction to walk
	char moveDir=0;
	
	// if jump was pressed, jump and exit
	if(Keys_keyDown(keyJump))
	{
		Worm_xVelo[(short)Worm_currentWorm] = (Worm_dir & wormMask) ? -1 : 1;
		Worm_yVelo[(short)Worm_currentWorm] = -4;
		return;
		
	// backflip
	}else if(Keys_keyDown(keyBackflip))
	{
		Worm_xVelo[(short)Worm_currentWorm] = (Worm_dir & wormMask) ? 1 : -1;
		Worm_yVelo[(short)Worm_currentWorm] = -6;
		return;
		
	// test for directions left/right
	}else if(Keys_keyState(keyLeft))
		moveDir=-1;
	else if(Keys_keyState(keyRight))
		moveDir=1;
		
	// if we are not moving, gtfo
	if(moveDir==0)
		return;
	
	// change the diretion the worm is facing, even if we dont move:
	if(moveDir<0)
		Worm_dir |= wormMask;
	else
		Worm_dir &= ~wormMask;
		
	// worms can't move up slops steeper than MAX_WORM_SLOPE pixels.
	// lets test pixels to the left or right +MAX_WORM_SLOPE pixels higher
	// if we find land, can't move
	if(Map_testPoint((*wX)+moveDir, (*wY)-MAX_WORM_SLOPE)==FALSE)
	{
		 // so the map position is free, now lets move down till we hit land:
		 short y=0;
		 for(y=0; y<MAX_WORM_SLOPE; y++)
		 	if(Map_testPoint((*wX)+moveDir, (*wY)-MAX_WORM_SLOPE+y))
		 		break;
		 		
		 // save the worms new position:
		 *wX += moveDir;
		 *wY = (short)((*wY)-MAX_WORM_SLOPE+y);
	}
}

// handles all update frames for controlling worms in the turn game mode
void CharacterController_update()
{
	// worm mask...
	wormMask = 1;
	wormMask = (unsigned long)((unsigned long)wormMask<<(Worm_currentWorm));
	
	// save references to our pos
	wX = &Worm_x[(short)Worm_currentWorm];
	wY = &Worm_y[(short)Worm_currentWorm];
	
	// if the worm is grounded, we should test for walking:
	if(Worm_onGround | wormMask)
		wormWalk();
		
		// TO-DO: implement parachute, bunjee, and ninja rope
}