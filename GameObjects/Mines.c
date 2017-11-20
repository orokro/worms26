#include "../Headers/System/Main.h"

// define our exter/global variables
short Mine_x[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
short Mine_y[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Mine_xVelo[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Mine_yVelo[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Mine_fuse[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int Mine_active=0;

// function prototypes
void spawnMine(char);
void checkExplosions(short);
void checkProximity(short);
void updateMine(short);

// local vars
char proxmityCheckTimer = 0;

// spawns Mines on the map, if they're enabled
void Mines_spawnMines()
{
	if(Match_minesEnabled==TRUE)
	{
		char i=0;
		for(i=0; i<10; i++)
			spawnMine(i);
	}
}

// spawns a single Mine
void spawnMine(char index)
{
	// find a free place for it on the map
	Map_getSpawnPoint();
	
	// save the last requested point:
	Mine_x[(short)index] = Map_lastRequestedSpawnX;
	Mine_y[(short)index] = Map_lastRequestedSpawnY;
	
	// make this mine active:
	Mine_active |= (int)1<<(index);
}

// main update for Mines
void Mines_update()
{
	proxmityCheckTimer++;
	if(proxmityCheckTimer>5)
		proxmityCheckTimer=0;
		
	// if any of the active Crates have less than 0 health, create an explosion
	// and set it inactive for the rest of the game
	short i=0;
	for(i=0; i<10; i++)
	{
		// okay we will only check proxity every 5 frames to save CPU since proxmity check is
		// epensive
		if(proxmityCheckTimer==0)
			checkProximity(i);
		
		// unfortuntely, since blast-detomation reqiores first-frame access, we gotta check
		// explosions every frame
		checkExplosions(i);
		
		// udpates the mine (if it has a timer, decrease it and explode it if necessary)
		updateMine(i);
		
	}// next i
}


// updates a mine
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
			Mine_active &= ~((int)1<<(index));

			// boom
			Explosion_spawn(Mine_x[index], Mine_y[index], 30, 30, TRUE);
				
		}// end if 
		
	}//end if active fuse
}


// check if a mine is within-detonation range of a Worm
void checkProximity(short index)
{
	short i=0; 
	for(i=0; i<16; i++)
	{
		// check if the worm is active
		char wormIsActiveInGame = (char)((Worm_active & (int)1<<(i))>0);
		char wormIsNotDead = (char)((Worm_isDead & (int)1<<(i))>0);
		
		// only do shit if the worm is active in the game, and NOT dead
		if(wormIsActiveInGame==TRUE && wormIsNotDead==TRUE)
		{
					
			// if it's in it's first frame, calculate the distance from us to it:
			short d = dist(Worm_x[index], Worm_y[index], Mine_x[i], Mine_y[i]);
			
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



// loop over explosions, and if there are any first-frame explosions, see if they hit us
void checkExplosions(short index)
{
	short i=0; 
	for(i=0; i<8; i++)
	{
		// check if the explosion is in it's first-frame
		char firstFrame = (char)((Explosion_firstFrame & (int)1<<(i))>0);

		// only do shit if first frame, yo
		if(firstFrame==TRUE)
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