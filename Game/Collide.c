// C Source File
// Created 11/22/2017; 2:28:07 AM

#include "Main.h"

/*
	Collide
	-------
	
	This file provides a bunch of generic collision test methods,
	which many objects will be using
	
*/

// tests for a collision and moves the opposite direction until a free pixel is found, if it did collide
short Collide_test(short x, short y, char dir)
{
	// these arrays will old the values we should move by, based on the direction
	// note: opposite values of actual direction, since we want to move opposite
	static const char xDirs[] = { 0, 0, 1, -1};
	static const char yDirs[] = { 1, -1, 0, 0};
	
	// test for a collision, and if none, just return 0
	if(Map_testPoint(x, y)==FALSE)
		return 0;
		
	// get the direction to move:
	char xDir = xDirs[(short)dir];
	char yDir = yDirs[(short)dir];
	
	// make relative pixel coordinates to test with:
	char xMovement = 0;
	char yMovement = 0;
	
	/*
		move until a free pixel.
	  worst case scenario is that the worm is put in an invalide state on the map
	  and this function moves them all the way to the edge of the map.
	  
	  Map_testPoint WILL return false once OOB of the map.. so the worm
	  would fly to the left or right of the map. Lets hope the game never lets worms
	  end up in invalid places...
	*/
	while(Map_testPoint(x+xMovement, y+yMovement))
	{
		xMovement += xDir;
		yMovement += yDir;
	}// wend
	
	// return the amount that direction needs to move:
	// COL_UP and COL_DOWN are 0 and 1 respectively, so they are less than 2
	return (dir < 2) ? yMovement : xMovement;
	
}
