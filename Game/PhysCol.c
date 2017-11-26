// C Source File
// Created 11/22/2017; 2:28:07 AM

#include "Main.h"

/*
	PhysCol
	-------
	
	This file provides a bunch of generic physics and collision methods,
	which many objects will be using
	
*/

// makes a new collider
Collider new_Collider(char type, char p1, char p2, char p3, char p4)
{
	Collider new;
	new.type = type;
	
	// make a new reference since the 
	new.p1 = p1;
	new.p2 = p2;
	new.p3 = p3;
	new.p4 = p4;
	
	return new;
}

// makes a new PhysObj
PhysObj new_PhysObj(short *x, short *y, char *xVelo, char *yVelo, float bounciness, float smoothness, char objIndex, void *settled, void *grounded, Collider c)
{
	PhysObj new;
	new.x = x;
	new.y = y;
	new.xVelo = xVelo;
	new.yVelo = yVelo;
	new.bounciness = bounciness;
	new.smoothness = smoothness;
	new.settled = settled;
	new.grounded = grounded;
	new.col = c;
	new.index = objIndex;
	new.lastX = 0;
	new.lastY = 0;
	return new;
}

/**
 * Spplys an objects collider. This can mutate it's X/Y.
 *
 * @param *col the Collider struct to apply
 * @param the x position to test relatively
 * @param the y position to test relatively
 * @return TRUE or FALSE if the collider hit/moved the object.
*/
char Collider_apply(Collider col, short *x, short *y)
{
	return FALSE;
	/*
	short x = 0;
	short y = 0;
	char dir = 1;
	
	// based on the directions involed, test various states
	
	// do we need to test up and down?
	if(col.type & (COL_UD | COL_UDLR))
	{
		// testing down takes precedence
		if(Collide_test(*x, *y-col.
	}*/
}


// does physics for an in game object
char Physics_apply(PhysObj *obj)
{
	// before we apply physics, save the inital state of the X/Y
	short x = *obj->x;
	short y = *obj->y;
	
	// apply the velocity
	*obj->x += (*obj->xVelo);
	*obj->y += (*obj->yVelo);
	
	// apply the collider, which might move our object
	char moved = Collider_apply(obj->col, obj->x, obj->y);
	
	return TRUE;
}

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
	char xDir;// = xDirs[(short)dir];
	char yDir;// = yDirs[(short)dir];
	
	switch(dir)
	{
		case COL_UP:
			xDir=xDirs[0];
			yDir=yDirs[0];
			break;
		case COL_DOWN:
			xDir=xDirs[1];
			yDir=yDirs[1];
			break;
		case COL_LEFT:
			xDir=xDirs[2];
			yDir=yDirs[2];
			break;
		case COL_RIGHT:
			xDir=xDirs[3];
			yDir=yDirs[3];
			break;
	}
	
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
	return (dir < 4) ? yMovement : xMovement;
	
}
