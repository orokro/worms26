// C Source File
// Created 11/11/2017; 11:34:05 PM

#include "Main.h"

/*
	Worms
	-----
	
	This file defines the worms. Note that, a separate file will handle controlling
	the currently selected worm during a turn.

	In the previous version of Worms, we had two separate sets of arrays:
	one for the White team and one for the Black team.
	
	In this new implementation, we will have one set of arrays for ALL worms.
	
	This way, we can loop over all of them at once, and update all of them at once.
*/

// the X/Y position of all the worms
short Worm_x[MAX_WORMS] = {15, 81, 120, 65, 90, 35, 150, 40, 175, 95, 250, 210, 25, 140, 150, 10};
short Worm_y[MAX_WORMS] = {100, 195, 55, 40, 15, 140, 20, 135, 30, 20, 150, 45, 170, 15, 75, 5};

// the current X/Y velocity of the worm, if it was knocked back by an explosion, etc
char Worm_xVelo[MAX_WORMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Worm_yVelo[MAX_WORMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// the direction the worm is currently FACING.. 0 = LEFT 1 = RIGHT
unsigned long Worm_dir = 0;

// the current HEALTH of the worm
char Worm_health[MAX_WORMS] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

// bit mask if the worm is dead. bit 1 = dead, bit = 0 alive
unsigned long Worm_isDead = 0;

// bit mask if the worm is ACTIVE... different that dead.
unsigned long Worm_active = 0;

// store the current mode for every worm:
char Worm_mode[MAX_WORMS] = {wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle };

// every time the team / turn switches a new worm is the "active" worm.
// if worm-select is enabled, during this period the play can change the active worm.
// the game-logic will update this variable, such that worms that are alive, and on the current team, will cycle
char Worm_currentWorm = 0;

// if any of the bits in this mask are 1, then the worm moved within
// the last frame. We can't end a turn until all worms are "stable"
unsigned long Worm_unstable = 0;

// if the collision dection for DOWN on a worm was true, then the worm is "on the ground"
// this is important for detecting falling for parachut or for walking
unsigned long Worm_onGround = 0;




// --------------------------------------------------------------------------------------------------------------------------------------




/**
 * Spawns a Worm of given index on the Map.
 * 
 * @param index the worm to spawn.
*/
void spawnWorm(short index)
{
	// find a free place for it on the map
	Map_getSpawnPoint();
	Worm_x[index] = Map_lastRequestedSpawnX;
	Worm_y[index] = Map_lastRequestedSpawnY;
	
	// set active
	Worm_active |= (unsigned long)1<<(index);
	
	// random direction
	if(random(2)==0)
		Worm_dir |= (unsigned long)1<<(index);
}



/**
 * Checks if the worm collided with the map, and puts it back in an OK place if it did
 *
 * @param index the index of the worm to collision test
*/
void wormCollision(short index)
{
	/*
		How this works:
		
		The worms origin point is it's the middle of it's sprite, not it's bottom.
		
		We will test for horizontal collisions first, by testing offsets left and right
		of the worms origin. If either collide, we will push the worm in the opposite
		direction to the first open pixel.
		
		Then we will test the top and bottom, and push the worm up or down in a similar fashion.
		
		Two problem with this is:
		
		If the worm falls into a crack that is more narrow than its width, the horizontal
		tests might pass, but it will still fall.
		
		On future frames, both the left and right colliders will trigger and the worm
		will be pushed into the map on the left side, since the right collider will happen
		after.
		
		Thus, the worm should not be allowed to fall if both it's left and right fired
		on this turn.
		
		Being flug upwards through a narrow crack is such an extreme edge case I wont
		handle it.
		
		Now technically, there should be no way for the map to create such narrow cracks.
		And damaging the map, tunneling, or drilling should never make such a narrow crack.
		
		As long as the map generator makes safe maps, this edge case does not need to be tested.
	*/
	
	// to prevent feed back loops, we always want to test on the worms ORIGINAL position
	short wormX = Worm_x[index];
	short wormY = Worm_y[index];
	
	// test horizontal positions at once. If both collide, the total would the same
	// delta needed to move the worm anyway. Also, both should never collide if our
	// maps are generated correctly.
	short col = Collide_test(wormX-2, wormY, COL_LEFT) + Collide_test(wormX+2, wormY, COL_RIGHT);
	if(col)
	{
		Worm_x[index] += col;
		
		// the worm hit a wall horizontally. Flip its horizontal velocity and half it
		Worm_xVelo[index] *= -0.5f;
		
		unsigned long mask = (unsigned long)1<<(index);
		if(Worm_dir & mask)
			Worm_dir &= ~mask;
		else
			Worm_dir |= mask;
	}
	
	// In a similar fashion we can check both top and bottom at the same time
	col = Collide_test(wormX, wormY-4, COL_UP) + Collide_test(wormX, wormY+6, COL_DOWN);
	if(col)
	{
		Worm_y[index] += col;
		
		// vertical collisions will kill all velocity
		Worm_xVelo[index]=0;
		Worm_yVelo[index]=0;
		
		/*
		  if the collision value was negative, we hit the ground at some point.
		  this, we are grounded. (if we hit the ceiling, the would have needed to move DOWN
			that is, a positive value.
			
			in theory, both the top and bottom collisions could happen and result in a negative,
			but we will ignore that edge case and try to never generate maps that would let that happen.
		*/
		if(col<0)
			Worm_onGround |= (unsigned long)1<<(index);
		else
			Worm_onGround &= ~((unsigned long)1<<(index));
	}
}




// --------------------------------------------------------------------------------------------------------------------------------------



// spawns worms on the map
void Worm_spawnWorms()
{
	short i=0;
	for(i=0;i<Match_wormCount[0]; i++)
		spawnWorm(i);
	for(i=0;i<Match_wormCount[1]; i++)
		spawnWorm(8+i);
}


// main update method for all worms
void Worm_update()
{
	// loop over worms, and any active worms should have their gravity and physics applied..
	short i;
	long wormMask;
	for(i=0; i<MAX_WORMS; i++)
	{
		// caculate the bitmask for this worm once, since we'll use it alot
		wormMask = (unsigned long)1<<(i);
		
		// only update worms in the game
		if((Worm_active & wormMask) > 0)
		{
			// add gravity to the worm
			Worm_yVelo[i]++;
			
			// if the worm is dead, it's gravestone can only have vertical velocity, no X
			if((Worm_isDead & wormMask) == 0)
				Worm_x[i] += Worm_xVelo[i];
		
			// add y velocity component
			Worm_y[i] += Worm_yVelo[i];
				
			// if the worm goes below 200 pixels, its drown:
			if(Worm_y[i]>200)
				Worm_isDead |= wormMask;
				
			// handle worm collision from it's new place:
			wormCollision(i);
			
			// if the worm has no velocity, it either hit the ground, or isn't moving
			// we can consider it stable:
			if(Worm_xVelo[i]==0 && Worm_yVelo==0)
				Worm_unstable |= wormMask;
			else
				Worm_unstable &= ~wormMask;
				
		}// end if active worm
	}// next i
}





