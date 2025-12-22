#include "Main.h"
#include "Game.h"
#include "Game.h"
#include "Crates.h"
#include "PhysCol.h"
#include "Worms.h"

/*
	Crates
	------
	
	This defines Crates on the map.
	
	About Crates:
	
	There are 3 types of crates: weapon, health, tool.
	Each type can either be enabled, or disabled for a match.
	If disabled, they will not spawn.
	
	Crates have no velocity - they can fall down, but not moved otherwise.
	
	Crates have health, and will explode if drained of health.
*/

// x/y positions of our Crates
short Crate_x[MAX_CRATES] = {0, 0, 0, 0, 0, 0, 0, 0};
short Crate_y[MAX_CRATES] = {0, 0, 0, 0, 0, 0, 0, 0};

// health of our crates
char Crate_health[MAX_CRATES] = {-1, -1, -1, -1, -1, -1, -1, -1};

// type of each Crate
char Crate_type[MAX_CRATES] = {0, 0, 0, 0, 0, 0, 0, 0};

// is the crate active?
unsigned short Crate_active=0;

// is the crate settled on land?
unsigned short Crate_settled=0;

PhysObj Crate_physObj[MAX_CRATES];
char Crate_xVelo[MAX_CRATES] = {0, 0, 0, 0, 0, 0, 0, 0};
char Crate_yVelo[MAX_CRATES] = {0, 0, 0, 0, 0, 0, 0, 0};

// only one crate can have a parachute at a time. if this is -1, none do
// if this is anything else, it's the index of the parachut crate
char parachuteCrate=-1;

// --------------------------------------------------------------------------------------------------------------------------------------


// pause the game with info on the screen until the user presses 2ND
void showInfo(const char *txt)
{
	Keys_update();
	
	GrayDBufSetHiddenAMSPlane(DARK_PLANE);
	ClrScr();
	DrawStr(1, 1, txt, A_NORMAL);
	
	GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
	ClrScr();
	DrawStr(1, 1, txt, A_NORMAL);

	GrayDBufToggleSync();
	
	while(Keys_keyDown(keyAction)==FALSE)
	{
		Keys_update();
	}
	
	Keys_update();
}

/**
 * Spawns a single crate of the given type
 *
 *@ param type the type of crate to spawn
 *@ return TRUE or FALSE if a crate was successfully spawned
*/
static char spawnCrate()
{

	// find a free spot in our array of crates to spawn one, 
	// we will assume that crates with less than 0 health are unactive
	short index=-1;
	short i=0;
	for(i=0; i<MAX_CRATES; i++)
	{
		if((Crate_active & ((unsigned short)1<<(i)))==0)
		{
			index = i;
			break;	
		}
	}

	// if we didn't find a free Crate slot, that means the maximum of 8 are already
	// spawned on the map. Just exit
	if(index==-1)
		return FALSE;

	/*
		So we need to intelligently pick a place to spawn a crate.
		
		The crate will spawn in the sky and fall down, so it will hit the top-most
		land pixel in the colum of pixels it spawns in.
		
		However, the crate cannot spawn where it will touch a worm as it's falling
		down, or after it's landed, because that worm will unfairly get the crate
		automatically.
		
		So we need to pick a point that it can fall to without touching worms.
		
		To do so, we'll first pick a random column of 32 pixels to test, in our 10-column
		wide map buffer.
		
		We will then move down from the top, and build an array of the top-level pixels,
		that aren't conflicting with a worm.
		
		In order to do so, we will first loop over all the worms and build an array
		of 32 unsigned chars, representing each pixel in our column.
		
		We will set the unsigned to be the value of the worms top, in each pixel column
		that worm exists in.
		
		While looping over the worms, we will also build an unsigned long where it's
		bits represent vertical colums that worms exist in.
		
		it's entirely possible that no worms exist in this colum, or no parts of worms.
		
		Anyway once we have these two things: an array of worms tops in our pixel columns,
		and a bitwise unsigned long of columns worms exist in in our pixel columns,
		We can move onto the next phase.
		
		We will create another array of 32 unsigned chars, all set to -1 by deault.
		We will also use another unsigned long, where it's bits represent if we've
		seen a pixel in that column.
		
		we will then move down the entire 32bit wide column from the top.
		
		When we first encounter a pixel in a colum, we check if it's ABOVE any worms
		that might exist in that pixel column. If it is above, then we save it as a valid
		pixel.
		
		If it's below the top of a worm, we cannot spawn there, because the crate will hit
		that worm on the way down.
		
		We can keep moving down until we've either disqualified or found a pixel in all 32 columns,
		or until we reach the end of the colum in the buffer.
		
		After this colum pass, we will have an array of valid tops that wont conflict and can pick
		a random one.
	*/
	
	// if we found a spawn point or not, and it's x colum
	short spawnCol = -9000;
	
	// keep track of which columns we've tested, and how many columns are left to test
	char cols[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	char remaining=10;
	while(remaining>0)
	{
		// pick a random colum to spawn in
		short r = random((short)remaining);
		short col = cols[r];
		
		// since we picked a spot from our array, we will move the highest
		// remaining column into it's hole, and decrease the remaining counter
		// this way, on next loop, we will pick a random nubmer in the range of
		// remaining columns
		cols[r] = cols[(short)remaining];
		remaining--;
		
		// calculate the left pixel of the column
		short colX = col*32;
		
		// so, now that we have a colum, we need to loop over all the worms,
		// and build an array of their tops from left-to-right, if they over lap with our column
		// we will make the worms default values an impossible one, to know if it's valid or not
		short wormTops[] = 	{ 900, 900, 900, 900, 900, 900, 900, 900,
													900, 900, 900, 900, 900, 900, 900, 900,
													900, 900, 900, 900, 900, 900, 900, 900,
													900, 900, 900, 900, 900, 900, 900, 900};
		
		// loop over worms and test their tops
		short w;
		for(w=0; w<16; w++)
		{
			// check if the worm is active and healthy
			unsigned long mask = 1;
			mask = mask<<(w);
			if((Worm_active & mask)	&& Worm_health[w]>0)
			{
				// top value of this worm
				short wTop = (Worm_y[w]-8);
				
				// check all the pixel columns the worm exists in, and add their highest top to our array
				// note that our range is not from the left of the worm to the right of the worm
				// its from the left of the worm, minus a half crate width, to the right of the worm plus a half crate width
				short c;
				for(c=0; c<28; c++)
				{
					short pixCol = ((Worm_x[w]-14)+c);
					if(pixCol>=colX && pixCol<colX+32)
						if(wTop<wormTops[pixCol-colX])
							wormTops[pixCol-colX] = wTop;
				}// next t
			}// end if active and health
		}// next w
													
		// now we have an array of either invalid 900s, or the tops of worms for all the columns they
		// exist in that overlap ours.
		
		// now we have to loop down the entire column from the top, and build an array of valid spawn
		// heights for each bit
		short spawnTops[32] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
														-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
												
		// once we've checked or invalidated all pixels, we can exit the loop early if these bits are all unset
		unsigned long undoneCols = 0b11111111111111111111111111111111;

		// if we find even one valid colum, we can spawn a crate!
		char foundValid = FALSE;
		
		// buffer references
		unsigned long *light = mapLight;
		unsigned long *dark = mapDark;
		
		// the top unsigned long to start with:
		short pos = col*200;
		
		// loop down our map buffer column
		for(i=0; i<199; i++)
		{
			// get the on pixels from both light and dark buffer
			unsigned long pixels = (unsigned long)((unsigned long)light[pos+i] | (unsigned long)dark[pos+1]);
			
			// if no pixels are on, this is just AIR and we can skip it.
			if(pixels==0)
				continue;
				
			// start with the left-most bit and check if any pixels qualify
			unsigned long pixel = 0b10000000000000000000000000000000;
			short p;
			for(p=0; p<32; p++)
			{
				// is this colum undone we should check it
				if(undoneCols & pixel)
				{
					// if the pixel is on, and not interfering with a worm, we can set this pixel as a valid spawn point
					if((pixels & pixel) && (wormTops[p]==900 || i<wormTops[p]))
					{
						// then we can save this top, say that this colum is satisfied, and we found a valid pixel
						spawnTops[p] = i;
						undoneCols &= ~pixel;
						foundValid=TRUE;
						
					// otherwise, if the pixel is greater than a worms top, we can deactivte this column
					}else if(i>=wormTops[p])
						undoneCols &= ~pixel;
						
				}// endif column is undone
				
				// if all columns are considered "done" we can exit bit the loop early
				if(undoneCols==0)
					break;
				
				// shift bits for next loop
				pixel = (unsigned long)pixel>>(1);
				
			}// next p
			
			// if all columns are considered "done" we can exit the height loop early
			if(undoneCols==0)
				break;
				
		}// next i
		
		// if we found at least one valid pixel, we can pick a spawn point!
		if(foundValid)
		{
			// lets make an array of valid columns:
			short validCount=0;
			short validTops[32];
			for(i=0; i<32; i++)
				if(spawnTops[i]!=-1)
				{
					validTops[validCount] = (colX+i);
					validCount++;
				}
				
			// pick a random column to spawn the crate in
			spawnCol = validTops[random(validCount)];
			
			// now break out of our while loop since this column was OK
			break;
			
		}// found valid, yay
			
		// nothing found? try another column...
		
	}// wend
	
	// if nothing found, abort
	if(spawnCol==-9000)
		return FALSE;
		
	// spawn crate in the valid column and at the top of the screen
	Crate_x[index] = spawnCol;
	Crate_y[index] = 0;
	
	// focus the camera on the crate
	Camera_focusOn(&Crate_x[(short)index], &Crate_y[(short)index]);

	// pick a type based on allowed types for this match. tools are more rare
	char hc = Match_healthCratesEnabled ? 0 : -1;
	char wc = Match_weaponCratesEnabled ? 1 : -1;
	char tc = Match_toolCratesEnabled ? 2 : -1;
	char enabledCrateTypes[7] = {hc, hc, hc, wc, wc, wc, tc};
	char type = random(7);
	while(enabledCrateTypes[(short)type]<0)
		type = random(7);
		
	Crate_type[index] = enabledCrateTypes[(short)type];
	
	Crate_type[index] = Game_turn%3;
	
	// reset health:
	Crate_health[index] = 30;

	// set active, and unsettled!
	Crate_active |= (unsigned short)1<<(index);
	Crate_settled &= ~((unsigned short)1<<(index));
	
	// make a new collider and physics object for this crate
	Collider coll = new_Collider(COL_DOWN, 0, 5, 0, 0);
	Crate_physObj[index] = new_PhysObj(&Crate_x[index], &Crate_y[index], &Crate_xVelo[index], &Crate_yVelo[index], 0.55f, 1.0f, (char)index, &Crate_settled, coll);
	Crate_physObj[index].bouncinessX=-1.0f;
	
	// crates spawn with a parachute
	parachuteCrate = (char)index;
	
	return TRUE;
}



// --------------------------------------------------------------------------------------------------------------------------------------


// spawns a Crate on the map, if they're enabled and returns if one was spawned
char Crates_spawnCrate()
{
	// if no crates are enabled in this game, just exit without spawning a crate
	if(Match_toolCratesEnabled==FALSE && Match_healthCratesEnabled==FALSE && Match_weaponCratesEnabled==FALSE)
		return FALSE;
	
	// for debug, always spawn crates
	return spawnCrate();
	
	// decide to spawn a crate... later in the game, crates will spawn more frequently
	short r = random(10);
	short range = (Game_turn < 30) ? (2+(Game_turn/10)) : 5;
	if(r<range)
		return spawnCrate();
	else
		return FALSE;
		
}

// main update for Crates
void Crates_update()
{
	// for debug...
	if(Keys_keyDown(key4))
		spawnCrate();
	
	// if any crate is parachuting, and action is pressed, drop the crate
	if(parachuteCrate!=-1 && Keys_keyDown(keyAction))
		parachuteCrate=-1;
	
	// if any of the active Crates have less than 0 health, create an explosion
	// and set it inactive for the rest of the game
	short i=0;
	for(i=0; i<MAX_CRATES; i++)
	{
		// check if enabled
		if((Crate_active & (unsigned short)1<<(i)))
		{
			// check all explosions if they are near-by and damaging this crate
			short damage = Physics_checkExplosions(&Crate_physObj[i]);
			Crate_health[i] -= damage;
			
			// if ded
			if(Crate_health[i]<=0)
			{
				// boom
				Explosion_spawn(Crate_x[i], Crate_y[i], 10, 10, TRUE);

				// no longer active
				Crate_active &= ~((unsigned short)1<<(i));

				// nothing left to check on this drum
				continue;
			}// end if health<=0
			
			// if the Crate is considered "settled" no need for physics
			if(!(Crate_settled & (unsigned short)1<<(i)))
			{
				// add gravity to Crate
				Crate_yVelo[i]++;
				
				// for the parachute crate, fall at a constant velocity,
				// and half speed
				if(i==parachuteCrate){
					Physics_setVelocity(&Crate_physObj[i], 0, ((Game_timer%2==0) ? 1 : 0), FALSE);
				}
				
				// do physics and collision for Crate
				Physics_apply(&Crate_physObj[i]);
				
				// if a crate collides while in parachute mode, disable parachute
				if(i==parachuteCrate && Crate_physObj[i].col.collisions>0)
					parachuteCrate=-1;
			}
			
		}// end if active
	}// next i
}

// allows a worm to pick up a crate
void Crates_pickUp(short index, short worm)
{
	// no longer active
	Crate_active &= ~((unsigned short)1<<(index));
	
	// based on the type, upgrade the worm
	if(Crate_type[index]==crateHealth)
		Worm_setHealth(worm, 50, TRUE);
}

