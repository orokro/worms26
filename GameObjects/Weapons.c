// C Source File
// Created 11/13/2017; 10:31:52 PM

#include "Main.h"

/*
	Weapons
	-------
	
	This defines Weapons on the map.
	
	About Weapons:
	
	The weapons system defines a number of arrays for on-screen weapon objects.
	Each weapon object has or optionally can have:
	- an X/Y position
	- an X/Y velocity
	- a timer
	- a target
	- gravity
	- .. and more
	
	Each weapon has it's on unique behavoir, however, some behavoirs can be
	shared between many weapons. For example, hand grenands and cluster bombs
	both are fired and have physics. When the cluser explodes it will create
	more bomb particles, where as the grenade just explodes.
	
	Physics for grenades, therefore, is shared between the two.

	A note about how Weapons are indexed:
	
	For the weapons menu, and inventory, the weapons are stored in a 5x14 array, like so:
	[5][14] = [5][14] = { 
												{0,9,9,9,9,1,1,3,5,0,2,0,9,1},
												{0,1,3,9,9,2,1,3,2,0,1,0,9,1},
												{0,5,0,9,1,1,0,3,2,0,0,0,3,1},
												{0,0,2,0,0,0,0,1,2,0,0,0,0,1},
												{0,0,0,2,9,0,0,0,0,0,0,0,0,1}
											};
											
	This pattern reflects the same position they are in the WWP menu on the PC.
	However, it can also be indexed directly, such that [0][0]=0 and [0][1]=1,
	where as [1][0] would actually be weapon 15.
	
	Regarding the total number of weapons on screen at once:
	Fire particles, either for napalm or fire, will be handled with a fire system separately,
	since fire doesn't need to take up weapon slots, and lots of weapons may need to spawn fire
	
	Thus, if a cluster bomb clusters into 5 small cluser fragments, there needs to be a total of 6 weapon
	slots availalbe:
	1 for the initial bomb, room to spawn 5 clusters on the same frame the bomb is deactivated
	
	For the uzi and gattling gun, there may need to be more than 6 bullets on screen tho, which would
	by far be the most in-game weapons at once.
	
	Let's say for now, 10 weapons, but we will only loop to 6 unless the "bullets" flag is set																		

	For Reference:
	
	usesVelocity 		1 	// 00000001
	usesGravity	 		2 	// 00000010
	usesTimer	   		4 	// 00000100
	usesHoming 	 		8		// 00001000
	usesMovement 		16 	// 00010000
	usesController 	32 	// 00100000

	WJetPack, 		WLowG, 				WFastWalk, 		WLaser,				WInvis,
	WBazooka,			WHoming,			WMorter,			WHomingP,			WSheepLaunch,
	WGrenade,			WCluster,			WBanana,			WAxe, 				WQuake,
	WShotG, 			WHandG, 			WUzi, 				WMiniG, 			WBow, 
	WPunch, 			WDragonBall, 	WDeath, 			WSBomb, 			WProd,
	WDyna, 				WMine, 				WSheep, 			WSSheep, 			WMole,
	WAirStrike, 	WNapStrike, 	WMailStrike,	WMineStrike, 	WMoleStrike,
	WBlow, 				WDrill, 			WGirder, 			WBaseball, 		WGirderPak,
	WNinja, 			WBungee, 			WParachute, 	WTeleport, 		WScales,
	WSBanana, 		WHolyGrenade, WFlame, 			WSalArmy, 		WMB,
	WMolotov, 		WSkunk, 			WMingVase, 		WSheepStrike, WCarpet,
	WCows, 				WOldLady, 		WDonkey, 			WNuke, 				WGeddon,
	WSkip, 				WSurrender, 	WSwitch, 			WIce, 				WMagicB,
	WClusterFrag, WBananaFrag, 	WComet,				WSuperSheep };

*/

// the type of the weapon!
char Weapon_type[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// x/y positions of our weapons
short Weapon_x[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
short Weapon_y[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// velocities of weapons
char Weapon_xVelo[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Weapon_yVelo[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// timer: various weapons can make use of a fuse timer, or timers for other reasons
char Weapon_time[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// each weapon has varius properties it can have: uses velocity, uses gravity, etc.
// this is an array of bit masks for the current weapons, and what features they have
char Weapon_uses[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// bit mask if the weapon is active in this slot
unsigned short Weapon_active = 0;

// because Weapons are defined BEFORE the Game header file, we cant access the cursor target
// thus, whever a target is set, it will have to update our weapon target
short Weapon_targetX = 0;
short Weapon_targetY = 0;



// --------------------------------------------------------------------------------------------------------------------------------------



// finds a free weapon slot available
short findFreeWeaponSlot()
{
	// loop till an active slot is found:
	short i=0;
	for(i=0; i<MAX_WEAPONS; i++)
	{
		char freeSlot = (char)((Weapon_active & (short)((short)1<<(i))) <= 0);
		if(freeSlot)
			return i;
	}
	
	// nothing found, return error code
	return -1;
}

	
// if a weapon has velocity enabled, use its velocity and move it appropriately
void updateVelocity(short index)
{
	// move based on velocity
	Weapon_x[index] += Weapon_xVelo[index];
	Weapon_y[index] += Weapon_yVelo[index];
	
	// we will handle collisions later, which includes bounds-checks
}

// if a weapon has gravity enabled, gravity as necessary
void updateGravity(short index)
{
	Weapon_yVelo[index]--;
	
	// we will handle collisions later, which includes bounds-checks
}

// if a weapon has a timer, this will decrement it, and call it's detonation method if time reaches 0
void updateTimer(short index)
{
	Weapon_time[index]--;
	if(Weapon_time[index]<=0)
	{
		// weapon is no longer active:
		Weapon_active &= ~((short)1<<(index));
		
		// here we will call custom methods for when time expires
		// TO-DO: implement detonation
	}
}

// if a weapon has homing, this will take move its position appropriately
void updateHoming(short index)
{
	// adjust the weapons velocity towards it's target, less agressively as it gets closer
	short deltaX = (short)((Weapon_targetX - Weapon_x[index])*0.25f);
	short deltaY = (short)((Weapon_targetY - Weapon_y[index])*0.25f);
	
	Weapon_xVelo[index] += deltaX;
	Weapon_yVelo[index] += deltaY;
}


// if a weapon has movement (e.g. cows, sheep, old lady, etc) this will update its custom movement logic
void updateMovement(short index)
{
	// here we need to provide custom logic for moving weapon objects,
	// for instance, sheep should move whatever direction their facing
	// and sheep should jump occasionally
	
	// TO-DO: implement
	index = index;
}


// if a weapon requires user input (such as super-sheep, or machine gun) this will take input from the user
void updateController(short index)
{
	// some weapons, like super sheep require user input to steer
	// other weapons, such as sheep or super banana bomb can be detonated before their time
	// this method will check for the key press, and call the detonation function
	// for the corresponding weapon
	
	// TO-DO: implement
	index = index;
}

// if a weapon colides with the ground or a worm, it may either deconate, bounce, etc
void updateCollision(short index)
{
	// we dont have our collisions system set up yet..
	
	// TO-DO: implement
	index = index;
}



// --------------------------------------------------------------------------------------------------------------------------------------


// spawns a weapon... simple enough
void Weapons_spawn(char type, short x, short y, char xVelocity, char yVelocity, char time, char properties)
{
	// find a free slot, if none are available, we are unable to spawn this weapon (should never happen)
	short slot = findFreeWeaponSlot();
	if(slot==-1)
		return;
		
	// set the weapon active:
	Weapon_active |= (short)1<<(slot);
	
	// set it's varius properties
	Weapon_type[slot] = type;
	Weapon_x[slot] = x;
	Weapon_y[slot] = y;
	Weapon_xVelo[slot] = xVelocity;
	Weapon_yVelo[slot] = yVelocity;
	Weapon_time[slot] = time;
	Weapon_uses[slot] = properties;
}

// returns TRUE/FALSE if any weapon is active:
char Weapons_weaponsActive()
{
	return (char)(Weapon_active>0);
}

// sets the target for our weapon
void Weapons_setTarget(short x, short y)
{
	Weapon_targetX = x;
	Weapon_targetY = y;
}

// update all weapons in game every frame
void Weapons_update()
{
	// loop over all weapons, and update the active ones as necessary
	short i=0;
	for(i=0; i<MAX_WEAPONS; i++)
	{
		// check if it's active
		char activeWeapon = (char)((Weapon_active & (short)((short)1<<(i))) > 0);
		if(activeWeapon)
		{
			
			// based on the properties it has, call each subroutine with it's index
			if(Weapon_uses[i] & usesVelocity)
				updateVelocity(i);
				
			if(Weapon_uses[i] & usesGravity)
				updateGravity(i);
				
			if(Weapon_uses[i] & usesTimer)
				updateTimer(i);
				
			if(Weapon_uses[i] & usesHoming)
				updateHoming(i);
				
			if(Weapon_uses[i] & usesMovement)
				updateMovement(i);
				
			if(Weapon_uses[i] & usesController)
				updateController(i);
			
			// all weapons will need to update their collision.
			// some weapons will detonate on impact, others will bounce or something else
			updateCollision(i);
			
		}// endif active
	}// next i
}


