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
	[5][13] = [5][13] = { 
												{0,9,9,9,9,1,1,3,5,0,2,0,9,1},
												{0,1,3,9,9,2,1,3,2,0,1,0,9,1},
												{0,5,0,9,1,1,0,3,2,0,0,0,3,1},
												{0,0,2,0,0,0,0,1,2,0,0,0,0,1},
												{0,0,0,2,9,0,0,0,0,0,0,0,0,1}
											};
											
	This pattern reflects the same position they are in the WWP menu on the PC.
	However, it can also be indexed directly, such that [0][0]=0 and [0][1]=1,
	where as [1][0] would actually be weapon 15.

	For Reference:
	
	usesAim
	usesCharge
	usesCursor
	usesPhysics
	usesWind
	usesHoming
	usesFuse
	usesDetonation
	usesController
	usesDetonateOnImpact
	isAnimal
	isCluster
	isParticle
	isMele
	spawnsSelf
	multiUse

	WJetPack,				WBazooka, 				WGrenade, 	WShotGun,		WFirePunch, 	WDynamite, 		WAirStrike, 		WBlowTorch, 	WNinjaRope, 			WSuperBanana, 		WPetrolBomb, 	WMadCows, 				WSkipGo,
	WLowGravity, 		WHomingMissle, 		WCluster,		WHandGUn,		WDragonBall,	WMine, 				WNapalmStrike, 	WDrill, 			WBungeeCord,			WHolyHandGrenade,	WSkunk, 			WOldLady, 				WSurrender,
	WFastWalk,			WMortar, 					WBanana, 		WUzi,				WKakamaze, 		WSheep, 			WMailStrike, 		WGirder, 			WParachute,				WFlameThrower, 		WMingVase, 		WConcreteDonkey, 	WSelectWorm,
	WLaserSight, 		WHomingPigeon, 		WAxe,				WMiniGun,		WSuicideBomb, WSuperSheep, 	WMineStrike, 		WBaseballBat, WTeleport, 				WSalvationArmy, 	WSheepStrike, WNuclearTest, 		WFreeze,
	WInvisibility,	WSheepLauncher,		WQuake, 		WLongbow,  	WProd, 				WMole, 				WMoleSquadron, 	WGirderPack, 	WScalesOfJustice,	WMBBomb, 					WCarpetBomb, 	WArmageddon, 			WMagicBullet,
	WFragment, 			WFire,						WSkunkGas,	WComet

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

// bit mask if the weapon is active in this slot
unsigned short Weapon_active = 0;

// because Weapons are defined BEFORE the Game header file, we cant access the cursor target
// thus, whever a target is set, it will have to update our weapon target
short Weapon_targetX = 0;
short Weapon_targetY = 0;

/*
	Below is a list of relative points to a worm to use for rotating the target
	crosshairs around the worm, when aiming a weapon.
*/
char Weapon_aimPosList[10][2] = {
	{0,  13},
	{2,  13},
	{4,  12},
	{6,  11},
	{8,  10},
	{10, 8},
	{11, 6},
	{12, 4},
	{13, 2},
	{13, 0}
};

/*
	So below is seems straight forward:
	
	Bitwise properties for each type of weapon.
	See Main.h for all the weapon property flags.
	
	But there's a catch:
	
	These weapon properties serve two purposes, but are all stored in the same place
	to conserve memory, and reduce redundency.
	
	However, it's important to note that these properties apply in two different
	situations, so you can read them easily.
	
	1) 	When the user selects a weapon from a the menu, the game must let the user
			use that weapon in a way that is appropriate.
			
			For instance, if they choose a weapon that is aimable it must show the cross
			hairs and read Up/Down input to aim the crosshairs.
			
			Not all aimable weapons are charable and fire at a fixed rate of speed, but
			some aimable weapons do require charging.
			
			These types of paramters apply to the worms character controller for activating
			the weapon.
			
	2)	In game / "on screen" weapons are slightly different. They are instances in our
			arrays above. That is, they have physics objects (sometimes), colliders (somtimes),
			the draw sprites, and etc.
			
			Weapons as OBJECTS in Game, also reference these properties.
			
	So for instance, a BAZOOKA has the following properties:
		usesAim | usesCharge | usesPhysics | usesWind | usesDetonateOnImpact
		
	We can see that: usesAim and usesCharge apply to the character controller.
	When this weapon is selected from the menu, the game must provide the interface for
	aiming a weapon, as well as the charge interface when firing.
	
	However, once the user has fired weapon, the rocket that appears on screen
	(aka an in game object), will have: usesPhysics, usesWind, usesDetonateOnImpact.
	
	Okay, so now you understand that these weapon properties apply both to their
	method of activation, as well as their ingame presence once they are spawned
	as a weapon object.
	
	But There's two things that might be a tad bit confusing:
	
		1) 	Some weapons NEVER have an ingame presence
				
				For instance, the Sheep Strike usesCursor to pick a place to spawn,
				and this part of the Character Controller.
				
				Once the user picks a place, it will spawn 5, Sheep weapons in the sky
				(NOT sheep strike weapons).
				
				This means that the code for weapons will sometimes care about different
				weapons than the code for the character controller.
				
		2)	Some weapons exist that the user does not directly use.
		
				When a cluster bomb, mortar, or etc goes off, it spawns clusters.
				
				Clusters are not in the menu, and cannot be directly used by the user.
				
				In this case, they are a weapon that is an in-game object ONLY.
				
	Other things to keep in mind:
		- properties 0-64 are the ingame usable weapons by the user
		- properties 65-68 are the sub-weapons that can only be spawned by other weapons.
		
	To recap:
		- some weapon properties/IDs are used both for the character controller and game object
		- some weapon properties/IDs are never used as in game objects
		- some weapon properties/IDs are never directly used by the character controller
		- properties for both character controlling and in game objects are stored in the same unsigned short
	
	For refrence, the following flags show which apply to character controlling and which to game objects
		
		Charactar Controller:
			- usesAim: 				the user needs to aim this weapon
			- usesCharge: 		the user needs to charge this weapon
			- usesCursor: 		the user must use the cursor to pick a target
			- usesFuse: 			the user can pick a fuse time length
			- spawnsSelf: 		the weapon is simple and can simply spawn near the worm with no extra logic
			- isMele: 				mele routines don't ever spawn weapon objects and just apply damage and velocity
												to nearby worms
			- multiUse: 			the weapon can be used more than once per turn, such as shotgun or longbow
		
		Game Objects:
			- usesPhysics: 		the weapon will have a PhysicsObjet and Collider
			- usesWind: 			wind velocity will be added every frame to the PhysicsObject
			- usesHoming: 		the object will path-find to the target
			- usesFuse: 			this is present in both uses. When the fuse time runs out, it's detonation
												routine will be called
			- usesDetonation: the user can press the action button to "detonate" or state-change the weapon
			- usesController: the weapon can change based on user input, even after it's spawned (e.g. super sheep)
			- isAnimal:				the sheep, cows, old lady, etc. will use similar routines
			- isCluster:			should it spawn clusters weapons upon detonation?	
			- isParticle:			no collision or typical physics, but should float up and cause damamge
	
	Below some of the properties are set to '0'
	This doesn't mean that the weapon doesn't do anything, but it will default to the
	weapon specific instructions.
	
	Originally I had a flag for usesCustom, for weapons that sometimes need extra logic per frame
	or during the character controller. Many weapons used this feature.
	
	I decided it was easier to assume most weapons will use this, and check all weapons against
	custom logic per frame. This freed a slot in the 16 bits for usesDetonteOnImpact, which is
	more specifically and reusable between weapon types.
	
	NOTE:
	
	Right now the array definition below uses a lot of bitwise OPs to set the flags for the various
	weapon types.
	
	This wastes space.
	
	In the final build I will replace this array with a list of numbers representing the bitwise props
	
	This way, I can avoid the OPs and save space. For debug, it will remain.
*/
unsigned short Weapon_props[69] = {
    // row 1
        // jetpack
        0,
        // bazooka
        usesAim | usesCharge | usesPhysics | usesWind | usesDetonateOnImpact,
        // grenade
        usesAim | usesCharge | usesPhysics | usesWind | usesFuse,
        // shotgun
        usesAim | multiUse,
        // firepunch
        isMele,
        // dynamite
        spawnsSelf,
        // air strike
        usesCursor,
        // blow torch
        0,
        // ninja rope
        0,
        // super banana bomb
        usesAim | usesCharge | usesPhysics | usesController | isCluster,
        // petrol bomb
        usesAim | usesCharge | usesPhysics | usesDetonateOnImpact,
        // mad cows
        spawnsSelf | usesFuse | usesController | isAnimal | usesPhysics,
        // skip turn
        0,
    
    // row 2
        // low gravity
        0,
        // homing missle
        usesAim | usesCharge | usesPhysics | usesHoming | usesDetonateOnImpact,
        // cluster bomb
        usesAim | usesCharge | usesPhysics | usesWind | usesCursor | isCluster,
        // hand gun
        usesAim | multiUse,
        // dragonball
        isMele,
        // mine
        0,
        // napalm strike
        usesCursor,
        // pneumatic drill
        0,
        // bunjee
        0,
        // holy hand grenade
        usesAim | usesCharge | usesPhysics,
        // skunk
        spawnsSelf | isAnimal | usesPhysics,
        // old lady
        spawnsSelf | isAnimal | usesPhysics | usesFuse,
        // surrender
        0,
    
    // row 3
        // fast walk
        0,
        // mortar
        usesAim | usesPhysics | isCluster | usesDetonateOnImpact,
        // banana bomb
        usesAim | usesCharge | usesPhysics | usesFuse,
        // uzi
        usesAim,
        // kamakaze
        isMele,
        // sheep
        spawnsSelf | usesPhysics | isAnimal,
        // mail strike
        usesCursor,
        // girder
        0,
        // parachute
        0,
        // flame thrower
        usesAim,
        // priceless ming vase
        spawnsSelf | usesFuse | isCluster,
        // concrete donkey
        usesCursor | usesPhysics | usesDetonateOnImpact,
        // select worm
        0,
    
    // row 4
        // laser sight
        0,
        // homing pigeon
        usesAim | usesHoming | usesCursor | usesDetonateOnImpact,
        // battle axe
        isMele,
        // minigun
        usesAim,
        // suicide bomber
        0,
        // supersheep
        usesController | usesDetonateOnImpact,
        // mine strike
        usesCursor,
        // baseball battle
        isMele,
        // teleport
        0,
        // salvation army lady
        spawnsSelf | usesPhysics | isAnimal | isCluster | usesController,
        // sheep strike
        usesCursor,
        // indian nuclear nuclear test
        0,
        // freeze
        0,
    
    // row 5
        // invisibility
        0,
        // sheep launcher
        usesAim | usesPhysics | usesController | usesDetonateOnImpact,
        // earth quake
        0,
        // long bow
        usesAim | usesPhysics | multiUse | usesDetonateOnImpact,
        // prod
        isMele,
        // mole
        spawnsSelf | usesPhysics | isAnimal | usesDetonateOnImpact,
        // mole squadron
        usesCursor,
        // girder pack
        usesCursor | multiUse,
        // scales of justice
        0,
        // MB bomb
        usesCursor | usesDetonateOnImpact,
        // carpet bomb
        usesCursor | usesPhysics | usesDetonateOnImpact,
        // armageddon
        0,
        // magic bullet
        usesAim | usesHoming | usesDetonateOnImpact,
    
    // sub weapons (ones you cannot directly use, but can spawn as a result of other weapons)
        // cluster/mortar/salvation army/ming vase fragment
        usesPhysics | usesDetonateOnImpact | usesFuse,
        // napalm / fire
        usesPhysics | usesDetonateOnImpact | usesFuse,
        // skunk gas
        isParticle | usesFuse,
        // comet from armageddon
        usesPhysics | usesDetonateOnImpact
};
    
    

// --------------------------------------------------------------------------------------------------------------------------------------



// finds a free weapon slot available
short findFreeWeaponSlot()
{
	// loop till an active slot is found:
	short i=0;
	for(i=0; i<MAX_WEAPONS; i++)
	{
		char freeSlot = (char)((Weapon_active & (unsigned short)((unsigned short)1<<(i))) <= 0);
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
		Weapon_active &= ~((unsigned short)1<<(index));
		
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
void Weapons_spawn(char type, short x, short y, char xVelocity, char yVelocity, char time, short properties)
{
	// find a free slot, if none are available, we are unable to spawn this weapon (should never happen)
	short slot = findFreeWeaponSlot();
	if(slot==-1)
		return;
		
	// set the weapon active:
	Weapon_active |= (unsigned short)1<<(slot);
	
	// set it's varius properties
	Weapon_type[slot] = type;
	Weapon_x[slot] = x;
	Weapon_y[slot] = y;
	Weapon_xVelo[slot] = xVelocity;
	Weapon_yVelo[slot] = yVelocity;
	Weapon_time[slot] = time;
	Weapon_props[slot] = properties;
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
		char activeWeapon = (char)((Weapon_active & (unsigned short)((unsigned short)1<<(i))) > 0);
		if(activeWeapon)
		{
			
			// based on the properties it has, call each subroutine with it's index
			/*
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
			*/
			
			// all weapons will need to update their collision.
			// some weapons will detonate on impact, others will bounce or something else
			updateCollision(i);
			
		}// endif active
	}// next i
}


// when user fires a weapon
void Weapons_fire(short charge)
{
	
}

