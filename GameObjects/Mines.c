#include "Main.h"

/*
	Mines
	-----
	
	This defines Mines on the map.
	
	About Mines:
	
	Mines can either be enabled or disabled.
	If disabled, they will not spawn on the map.
	
	Mines can have pre-set fuse lenghs, or random fuse lengths.
	
	Mines can optionally have duds, which fizzle out and do not explode
	when their fuse runs out.
	
	Note about dudes: since 6 mines will spawn per round, randomly all over
	the map, we can say: the mine at index 0 will be a dud, if dudes are enabled.
	
	There will never be more than one dud, and if duds are enabled we can
	just use index 0, which will randomly be placed - no need to choose
	a random index.
	
*/

// x/y positions of our Mines
short Mine_x[MAX_MINES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
short Mine_y[MAX_MINES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// velocity of mines
char Mine_xVelo[MAX_MINES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Mine_yVelo[MAX_MINES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// fuse of mines
char Mine_fuse[MAX_MINES] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

// is the mine active? using bits for booleans
unsigned short Mine_active=0;

// is the mine settled on the map?
unsigned short Mine_settled=0;

// local vars
char proxmityCheckTimer = 0;
PhysObj Mine_physObj[MAX_MINES];


// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * Spawns a single mine of the given index.
 * 
 * @param index the mine to spawn.
*/
void spawnMine(char index)
{
	// find a free place for it on the map
	Map_getSpawnPoint();
	
	// save the last requested point:
	Mine_x[(short)index] = Map_lastRequestedSpawnX;
	Mine_y[(short)index] = Map_lastRequestedSpawnY;
	
	// make this mine active:
	Mine_active |= (unsigned short)1<<(index);
	
	// make a new collider and physics object for this mine
	Collider col = new_Collider(COL_UDLR, 2, 2, 3, 3);
	Mine_physObj[(short)index] = new_PhysObj(&Mine_x[(short)index], &Mine_y[(short)index], &Mine_xVelo[(short)index], &Mine_yVelo[(short)index], 0.65f, 1.0f, (char)index, &Mine_settled, col);
}



/**
 * Updates a Mine of given index, should be called every frame by Mines_update().
 * 
 * @param index the mine to udpate.
*/
void updateMine(short index)
{
	// if the mine has time on it, decrement it
	if(Mine_fuse[index]>0)
	{
		Mine_fuse[index]--;
		
		// if it reaches 0, boom
		if(Mine_fuse[index]==0)
		{
			// disable the mine
			Mine_fuse[index] = -1;
			Mine_active &= ~((unsigned short)1<<(index));

			// boom
			Explosion_spawn(Mine_x[index], Mine_y[index], 30, 30, TRUE);
				
		}// end if 
		
	}//end if active fuse
	
	// if the Mine is considered "settled" no need for physics
	if(!(Mine_settled & (unsigned short)1<<(index)))
	{
		// add gravity to mine
		Mine_yVelo[index]++;
		
		// do physics and collision for OilDrum
		Physics_apply(&Mine_physObj[index]);
	}
}


/**
 * Check if the Mine of given index is within proximity of worm and should explode.
 * 
 * @param index the mine to check for worm proximity.
*/
void checkProximity(short index)
{
	// if this mine already has a timer, gtfo
	if(Mine_fuse[index]>=0)
		return;
		
	short i=0; 
	for(i=0; i<16; i++)
	{
		// check if the worm is active
		char wormIsActiveInGame = (char)((Worm_active & (unsigned short)1<<(i))>0);
		char wormIsNotDead = (char)((Worm_isDead & (unsigned short)1<<(i))==0);
		
		// only do shit if the worm is active in the game, and NOT dead
		if(wormIsActiveInGame && wormIsNotDead)
		{
					
			// if it's in it's first frame, calculate the distance from us to it:
			short d = dist(Worm_x[i], Worm_y[i], Mine_x[index], Mine_y[index]);
			
			// if we're withing the trigger distance, pick a timer for this mine
			if(d <= mineTriggerDistance)
			{
				// based on mine timer settings pick set a time for this mine
				// 6 will be random length, anything less will be the associated time
				Mine_fuse[index] = ((Match_mineFuseLength<6) ? (Match_mineFuseLength*TIME_MULTIPLIER) : (random(5)*TIME_MULTIPLIER));
			}
			
		}// end if first frame
		
	}// next i
}


/**
 * Checks if any explosions affect the mine of the given index.
 *
 * @param index the mine to check to see if explostions affected it.
*/
void checkExplosions(short index)
{
	short i=0; 
	for(i=0; i<8; i++)
	{
		// check if the explosion is in it's first-frame
		char firstFrame = (char)((Explosion_firstFrame & (unsigned short)1<<(i))>0);

		// only do shit if first frame, yo
		if(firstFrame)
		{
						
			// the power of the velcotity
			float velocityPower = 0.0f;
			
			// if it's in it's first frame, calculate the distance from us to it:
			short d = dist(Mine_x[index], Mine_y[index], Explosion_x[i], Explosion_y[i]);
			
			// calculate the extranius radius, with a power-fall off
			short largerRadius = (short)(1.5f * Explosion_size[i]);
			
			// if we're withing the blast radius, take full damage:
			if(d<Explosion_size[i])
			{
				velocityPower = 1.0f;
			}else if(d<largerRadius)
			{
				// subract the minimum radius from both:
				short minD = d - Explosion_size[i];
				short minL = largerRadius - Explosion_size[i];
				
				// calculate how far away we are:
				float distFallOffRatio = (1.0f - ((float)minD/(float)minL));
				
				// set velocity power:
				velocityPower = distFallOffRatio;
				continue;
				
			}else
			{
				velocityPower = 0.0f;
			}//end if
			
			// apply velocity from explosion:
			Mine_xVelo[index] = (Explosion_x[i]-Mine_x[index]) * velocityPower;
			Mine_yVelo[index] = (Explosion_y[i]-Mine_y[index]) * velocityPower;
			
		}// end if first frame
		
	}// next i
}



// --------------------------------------------------------------------------------------------------------------------------------------



// spawns Mines on the map, if they're enabled
void Mines_spawnMines()
{

	if(Match_minesEnabled)
	{
		char i=0;
		
		// only spawn 6 mines, leaving 4 free slots for user placeable mines
		for(i=0; i<6; i++)
			spawnMine(i);
	}
}


// main update for Mines
void Mines_update()
{
	proxmityCheckTimer++;
	if(proxmityCheckTimer>5)
		proxmityCheckTimer=0;
		
	// for debugging, we'll allow the user to switch between mines, and add velocity to
	// test physics
	static short testMine = 0;
	if(Keys_keyDown(key1))
	{
		testMine++;
		if(testMine>5)
			testMine=0;
		Camera_focusOn(&Mine_x[(short)testMine], &Mine_y[(short)testMine]);
	}
	if(Keys_keyDown(key2))
	{
		//Mine_y[testMine]-=6;
		Physics_setVelocity(&Mine_physObj[testMine], -4, -7, FALSE);
	}
	if(Keys_keyDown(key3))
	{
		//Mine_y[testMine]-=6;
		Physics_setVelocity(&Mine_physObj[testMine], 4, -7, FALSE);
	}
	
	// if any of the active Crates have less than 0 health, create an explosion
	// and set it inactive for the rest of the game
	short i=0;
	for(i=0; i<MAX_MINES; i++)
	{
		// only update active mines
		if(Mine_active & (unsigned short)1<<(i))
		{
			// okay we will only check proxity every 5 frames to save CPU since proxmity check is
			// epensive
			//if(proxmityCheckTimer==0)
			//	checkProximity(i);
			
			// unfortuntely, since blast-detomation reqiores first-frame access, we gotta check
			// explosions every frame
			checkExplosions(i);
			
			// udpates the mine (if it has a timer, decrease it and explode it if necessary)
			updateMine(i);
		}	
	}// next i
}
