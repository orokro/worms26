/*
	Crates.c
	--------
	
	This defines Crates on the map.
	
	About Crates:
	
	There are 3 types of crates: weapon, health, tool.
	Each type can either be enabled, or disabled for a match.
	If disabled, they will not spawn.
	
	Crates have no velocity - they can fall down, but not moved otherwise.
	
	Crates have health, and will explode if drained of health.
*/


// includes
#include "Main.h"
#include "Camera.h"
#include "Keys.h"
#include "Match.h"
#include "Game.h"
#include "Game.h"
#include "Crates.h"
#include "PhysCol.h"
#include "Worms.h"
#include "Map.h"
#include "Explosions.h"
#include "SpriteData.h"
#include "Draw.h"
#include "StatusBar.h"
#include "Weapons.h"

// defines
#define NUM_TOOLS 16

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
// if this is anything else, it's the index of the parachute crate
char parachuteCrate=-1;



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * pause the game with info on the screen until the user presses 2ND
 * @char text to show for the info
 */
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

	short sx, sy;
	if(!Map_findSpawnPoint(SPAWN_CRATE, &sx, &sy))
		return FALSE;
		
	// spawn crate in the valid column and at the top of the sky (Map_findSpawnPoint returns sky coords for Crates)
	Crate_x[index] = sx;
	Crate_y[index] = sy;
	
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
	new_Collider(&(Crate_physObj[index].col), COL_DOWN, 0, 5, 0, 0);
	new_PhysObj(&Crate_physObj[index], &Crate_x[index], &Crate_y[index], &Crate_xVelo[index], &Crate_yVelo[index], 0.55f, 1.0f, (char)index, &Crate_settled);
	Crate_physObj[index].bouncinessX=-1.0f;
	
	// crates spawn with a parachute
	parachuteCrate = (char)index;
	
	return TRUE;
}


// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * spawns a Crate on the map, if they're enabled and returns if one was spawned
 */
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


/**
 * main update for Crates
 */
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
					Physics_setVelocity(&Crate_physObj[i], 0, ((Game_timer%2==0) ? 1 : 0), FALSE, TRUE);
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


// array of valid tool indexes
const char toolIndices[NUM_TOOLS] = {
	0, // Jetpack
	7, // Blowtorch
	8, // Ninja Rope
	12, // Skip Go
	20, // Pneumatic Drill
	21, // Bungie
	25, // Surrender Flag
	26, // Fast Walk
	33, // Girder
	34, // Parachute
	38, // Select Worm
	39, // Laser Sight
	47, // Teleport
	51, // Freeze
	52, // Invisibility
	59, // Girder Pack
};

/**
 * allows a worm to pick up a crate
 * @param index - which crate to pick up
 * @param worm - which worm is picking it up
 */
void Crates_pickUp(short index, short worm)
{
	// no longer active
	Crate_active &= ~((unsigned short)1<<(index));
	
	// based on the type, upgrade the worm
	if(Crate_type[index]==crateHealth)
	{
		Worm_setHealth(worm, 50, TRUE);
		StatusBar_showMessage("+50 Health!");
	}
	else if(Crate_type[index]==crateTool)
	{

		// give worm a tool
		short toolIndex = toolIndices[random(NUM_TOOLS)];
		Match_teamWeapons[(short)Game_currentTeam][toolIndex]++;
		StatusBar_showMessage(weaponNames[toolIndex]);
	}
	else if(Crate_type[index]==crateWeapon)
	{
		// pick random index from 0-65
		short weaponIndex = random(65);
		Match_teamWeapons[(short)Game_currentTeam][weaponIndex]++;
		StatusBar_showMessage(weaponNames[weaponIndex]);
	}
}


/**
	Draws all the in-game, on-screen Crates.
*/
void Crates_drawAll()
{
	short screenX, screenY;
	
	// loop over all crates and draw them if active:
	short i;
	for(i=0; i<MAX_CRATES; i++)
	{
		if(Crate_active & ((unsigned short)1<<(i)))
		{
			screenX=Crate_x[i];
			screenY=Crate_y[i];
			if(worldToScreen(&screenX, &screenY))
			{
				// if crate has parachute
				if(i==parachuteCrate)
				{
					ClipSprite16_MASK_R(screenX-6, screenY-16, 12, spr_Parachute+12, spr_Parachute, lightPlane);
					ClipSprite16_MASK_R(screenX-6, screenY-16, 12, spr_Parachute+24, spr_Parachute, darkPlane);
				}
				
				// draw crate (this comes after parachute so it can slightly overlap)
				unsigned short *sprite = ((Crate_type[i]==crateHealth) ? (unsigned short*)&spr_CrateHealth[0] : ((Crate_type[i]==crateWeapon) ? (unsigned short*)&spr_CrateWeapon[0] : (unsigned short*)&spr_CrateTool[0]));					
				ClipSprite16_MASK_R(screenX-6, screenY-6, 12, sprite+12, sprite, lightPlane);
				ClipSprite16_MASK_R(screenX-6, screenY-6, 12, sprite+24, sprite, darkPlane);

			}// end if on screen
		}// end if active
	}// next i
}
