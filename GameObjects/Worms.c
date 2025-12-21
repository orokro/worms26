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

// physics objects for this worm
PhysObj Worm_physObj[MAX_WORMS];

// the direction the worm is currently FACING.. 0 = LEFT 1 = RIGHT
unsigned short Worm_dir = 0;

// the current HEALTH of the worm
short Worm_health[MAX_WORMS] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

// bit mask if the worm is dead. bit 1 = dead, bit = 0 alive
unsigned short Worm_isDead = 0;

// bit mask if the worm is ACTIVE... different that dead.
unsigned short Worm_active = 0;

// store the current mode for every worm:
char Worm_mode[MAX_WORMS] = {wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle, wormMode_idle };

// every time the team / turn switches a new worm is the "active" worm.
// if worm-select is enabled, during this period the play can change the active worm.
// the game-logic will update this variable, such that worms that are alive, and on the current team, will cycle
char Worm_currentWorm = 0;

// if any of the bits in this mask are 1, then the worm moved within
// the last frame. We can't end a turn until all worms are "stable"
unsigned short Worm_settled = 0;

// if the collision dection for DOWN on a worm was true, then the worm is "on the ground"
// this is important for detecting falling for parachut or for walking
unsigned short Worm_onGround = 0;

// the current worms 10x10 tile. we don't need to store both X and Y, just the tile index
short Worm_tile[MAX_WORMS] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * So we don't have to render text every time, this routine will generate a the sprites to use for a worms health.
 *
 * This way, we can redraw the sprite, only when the health changes.
 *
 * @param index the index of the worm to draw the health sprite for
*/
void renderHealthSprite(short index)
{
	Draw_healthSprite(index);
}


/**
 * Spawns a Worm of given index on the Map.
 * 
 * @param index the worm to spawn.
*/
void spawnWorm(short index)
{
	unsigned short mask = 1;
	mask = mask<<(index);
	
	// find a free place for it on the map
	Map_getSpawnPoint();
	Worm_x[index] = Map_lastRequestedSpawnX;
	Worm_y[index] = Map_lastRequestedSpawnY;
	
	// set active
	Worm_active |= mask;

	// make a new collider and physics object for this worm
	Collider col = new_Collider(COL_UDLR, -4, 6, -2, 2);
	Worm_physObj[index] = new_PhysObj(&Worm_x[index], &Worm_y[index], &Worm_xVelo[index], &Worm_yVelo[index], 0.4f, 1.0f, (char)index, &Worm_settled, col);
	
	// allow worms to bounce of walls, but not land
	Worm_physObj[index].bouncinessY = 0.0f;
	
	// for debug, give random health:
	Worm_health[index] = random(200)+1;
	
	// render initial health sprite
	renderHealthSprite(index);
	
	// random direction
	if(random(2)==0)
		Worm_dir |= mask;
}



/**
 * checks if a worm is close to a crate or a mine, based on the tile the worm is in
 */
void checkCratesAndMines(short index)
{
		short i, c;
    
    // Check Mines
    for(i=0; i<MAX_MINES; i++)
    {
        // Check if active, not triggered, and close enough
        if((Mine_active & (1<<i)) && !(Mine_triggered & (1<<i)))
        {
             if(abs(Mine_x[i] - Worm_x[index]) < 15 && abs(Mine_y[i] - Worm_y[index]) < 15)
             {
                 Mines_trigger(i);
             }
        }
    }

    // Check Crates
    for(c=0; c<MAX_CRATES; c++)
    {
        // Check if active, not triggered, and close enough
        if((Crate_active & (1<<c)))
        {
             if(abs(Crate_x[c] - Worm_x[index]) < 15 && abs(Crate_y[c] - Worm_y[index]) < 15)
             {
				Crates_pickUp(c, index);
             }
        }
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
		wormMask = 1;
		wormMask = (unsigned short)((unsigned short)wormMask<<(i));
		
		// only update worms in the game
		if(Worm_active & wormMask)
		{
			
			// check all explosions if they are near-by and damaging this worm
			short damage = Physics_checkExplosions(&Worm_physObj[i]);
			if(damage!=0)
				Worm_setHealth(i, -damage, TRUE);
				
			// if the worm is considered "settled" no need for physics
			if(!(Worm_settled & wormMask))
			{
				// add gravity to the worm
				Worm_yVelo[i]++;
				
				// if the worm is dead, it's gravestone can only have vertical velocity, no X
				if(Worm_isDead & wormMask)
					Worm_xVelo[i]=0;

				// do physics and collision for worm
				Physics_apply(&Worm_physObj[i]);
				
				// if we collided with the ground on the last frame, assume the worm is grounded.
				// (it should collide with the ground ever frame its grounded, due to gravity.
				// if the worm was settled on this frame, we won't get here on the next frame
				// so we can just mark it as settled and it will stay that way till moved again
				if(Worm_physObj[i].col.collisions & COL_DOWN || (Worm_settled & wormMask))
					Worm_onGround |= wormMask;
				else
					Worm_onGround &= ~wormMask;
			}
			
			// calculate the worms tile, and if it changed, we need to check for mine and crate updates
			Worm_tile[i] = (Worm_x[i]/10) * (Worm_y[i]/10);
		
			//check if a worm triggered a crate or a mine
			checkCratesAndMines(i);
					
			// if the worm goes below 200 pixels, its drown:
			if(Worm_y[i]>200)
				Worm_isDead |= wormMask;
					
		}// end if active worm
	}// next i
}


// changes the worms health
void Worm_setHealth(short index, short health, char additive)
{
	if(additive)
		Worm_health[index] += health;
	else
		Worm_health[index] = health;
		
	// update it's sprite
	renderHealthSprite(index);
}




