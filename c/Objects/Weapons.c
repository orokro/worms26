/*
	Weapons.c
	---------
	
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
	
	Each weapon has it's on unique behavior, however, some behaviors can be
	shared between many weapons. For example, hand grenades and cluster bombs
	both are fired and have physics. When the cluster explodes it will create
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

	WJetPack,			WBazooka, 			WGrenade, 	WShotGun,		WFirePunch, 	WDynamite, 		WAirStrike, 		WBlowTorch, 	WNinjaRope, 		WSuperBanana, 		WPetrolBomb, 	WMadCows, 			WSkipGo,
	WLowGravity, 		WHomingMissle, 		WCluster,	WHandGUn,		WDragonBall,	WMine, 			WNapalmStrike, 		WDrill, 		WBungeeCord,		WHolyHandGrenade,	WSkunk, 		WOldLady, 			WSurrender,
	WFastWalk,			WMortar, 			WBanana, 	WUzi,			WKakamaze, 		WSheep, 		WMailStrike, 		WGirder, 		WParachute,			WFlameThrower, 		WMingVase, 		WConcreteDonkey, 	WSelectWorm,
	WLaserSight, 		WHomingPigeon, 		WAxe,		WMiniGun,		WSuicideBomb, 	WSuperSheep, 	WMineStrike, 		WBaseballBat, 	WTeleport, 			WSalvationArmy, 	WSheepStrike, 	WNuclearTest, 		WFreeze,
	WInvisibility,		WSheepLauncher,		WQuake, 	WLongbow,  		WProd, 			WMole, 			WMoleSquadron, 		WGirderPack, 	WScalesOfJustice,	WMBBomb, 			WCarpetBomb, 	WArmageddon, 		WMagicBullet,
	WFragment, 			WFire,				WSkunkGas,	WComet

	C Source File
	Created 11/13/2017; 10:31:52 PM
*/


// includes
#include "Main.h"
#include "Camera.h"
#include "Game.h"
#include "PhysCol.h"
#include "Worms.h"
#include "Weapons.h"
#include "Explosions.h"
#include "SpriteData.h"
#include "Draw.h"
#include "Map.h"


// the type of the weapon!
char Weapon_type[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// x/y positions of our weapons
short Weapon_x[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
short Weapon_y[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// physics objects and velocities of weapons
PhysObj Weapon_physObj[MAX_WEAPONS];
char Weapon_xVelo[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char Weapon_yVelo[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// timer: various weapons can make use of a fuse timer, or timers for other reasons
unsigned short Weapon_time[MAX_WEAPONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// bit mask if the weapon is active in this slot
unsigned short Weapon_active = 0;

// is the weapon settled on land?
unsigned short Weapon_settled=0;

// because Weapons are defined BEFORE the Game header file, we cant access the cursor target
// thus, wherever a target is set, it will have to update our weapon target
short Weapon_targetX = 0;
short Weapon_targetY = 0;

/*
	Below is a list of relative points to a worm to use for rotating the target
	crosshair around the worm, when aiming a weapon.
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
	to conserve memory, and reduce redundancy.
	
	However, it's important to note that these properties apply in two different
	situations, so you can read them easily.
	
	1) 	When the user selects a weapon from a the menu, the game must let the user
			use that weapon in a way that is appropriate.
			
			For instance, if they choose a weapon that is aimable it must show the cross
			hairs and read Up/Down input to aim the crosshair.
			
			Not all aimable weapons are chargable and fire at a fixed rate of speed, but
			some aimable weapons do require charging.
			
			These types of parameters apply to the worms character controller for activating
			the weapon.
			
	2)	In game / "on screen" weapons are slightly different. They are instances in our
			arrays above. That is, they have physics objects (sometimes), colliders (sometimes),
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
	method of activation, as well as their in game presence once they are spawned
	as a weapon object.
	
	But There's two things that might be a tad bit confusing:
	
		1) 	Some weapons NEVER have an in game presence
				
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
		- properties 0-64 are the in game usable weapons by the user
		- properties 65-68 are the sub-weapons that can only be spawned by other weapons.
		
	To recap:
		- some weapon properties/IDs are used both for the character controller and game object
		- some weapon properties/IDs are never used as in game objects
		- some weapon properties/IDs are never directly used by the character controller
		- properties for both character controlling and in game objects are stored in the same unsigned short
	
	For reference, the following flags show which apply to character controlling and which to game objects
		
		Character Controller:
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
			- isParticle:			no collision or typical physics, but should float up and cause damage
	
	Below some of the properties are set to '0'
	This doesn't mean that the weapon doesn't do anything, but it will default to the
	weapon specific instructions.
	
	Originally I had a flag for usesCustom, for weapons that sometimes need extra logic per frame
	or during the character controller. Many weapons used this feature.
	
	I decided it was easier to assume most weapons will use this, and check all weapons against
	custom logic per frame. This freed a slot in the 16 bits for usesDetonateOnImpact, which is
	more specifically and reusable between weapon types.
	
	NOTE:
	
	Right now the array definition below uses a lot of bitwise OPs to set the flags for the various
	weapon types.
	
	This wastes space.
	
	In the final build I will replace this array with a list of numbers representing the bitwise props
	
	This way, I can avoid the OPs and save space. For debug, it will remain.
*/
unsigned long Weapon_props[72] = {
    // row 1
        // jet pack
        0,

        // bazooka
        usesAim | usesCharge | usesPhysics | usesWind | usesDetonateOnImpact | holdsLauncher,
        
		// grenade
        usesAim | usesCharge | usesPhysics | usesWind | usesFuse | holdsSelf | spawnsSelf,
        
		// shotgun
        usesAim | multiUse | holdsSelf | usesRaycast,
        
		// fire punch
        isMele,
        
		// dynamite
        spawnsSelf | usesPhysics | holdsSelf | isDroppable,
        
		// air strike
        usesCursor | usesAirStrike,
        
		// blow torch
        0,
        
		// ninja rope
        0,
        
		// super banana bomb
        usesAim | usesCharge | usesPhysics | usesController | isCluster | spawnsSelf | holdsSelf,
        
		// petrol bomb
        usesAim | usesCharge | usesPhysics | usesDetonateOnImpact | holdsSelf | spawnsSelf,
        
		// mad cows
        spawnsSelf | usesFuse | usesController | isAnimal | usesPhysics | holdsSelf,
        
		// skip turn
        isMeta | holdsSelf,
    
    // row 2
        // low gravity
        isMeta,
        
		// homing missile
        usesAim | usesCharge | usesPhysics | usesHoming | usesDetonateOnImpact | holdsLauncher,
        
		// cluster bomb
        usesAim | usesCharge | usesPhysics | usesWind | isCluster | holdsSelf | spawnsSelf,
        
		// hand gun
        usesAim | holdsSelf | usesRaycast | usesRoutine,
        
		// dragon ball
        isMele,
        
		// mine
        usesFuse | spawnsSelf | holdsSelf | isDroppable,
        
		// napalm strike
        usesCursor | usesAirStrike,
        
		// pneumatic drill
        holdsCustom,
        
		// bungee
        isMeta,
        
		// holy hand grenade
        usesAim | usesCharge | usesPhysics | holdsSelf | spawnsSelf,
        
		// skunk
        spawnsSelf | isAnimal | usesPhysics | holdsSelf,
        
		// old lady
        spawnsSelf | isAnimal | usesPhysics | usesFuse | holdsSelf,
        
		// surrender
        isMeta | holdsSelf,
    
    // row 3
        
		// fast walk
        isMeta,
        
		// mortar
        usesAim | usesPhysics | isCluster | usesDetonateOnImpact | holdsLauncher,
        
		// banana bomb
        usesAim | usesCharge | usesPhysics | usesFuse | spawnsSelf | holdsSelf,
        
		// uzi
        usesAim | holdsSelf | usesRaycast | usesRoutine,
        
		// kamikaze
        isMele,
        
		// sheep
        spawnsSelf | usesPhysics | isAnimal | holdsSelf,
        
		// mail strike
        usesCursor | usesPhysics | spawnsSelf | usesAirStrike,
        
		// girder
        0,
        
		// parachute
        isMeta,
        
		// flame thrower
        usesAim | usesRoutine | holdsSelf,
        
		// priceless ming vase
        spawnsSelf | usesFuse | isCluster | usesPhysics | holdsSelf | isDroppable,
        
		// concrete donkey
        usesCursor | usesPhysics | usesDetonateOnImpact | usesAirStrike,
        
		// select worm
        isMeta | holdsSelf,
    
    // row 4
        
		// laser sight
        isMeta | usesRaycast,
        
		// homing pigeon
        usesAim | usesHoming | usesCursor | usesDetonateOnImpact | spawnsSelf | holdsSelf,
        
		// battle axe
        isMele | holdsSelf,
        
		// minigun
        usesAim | holdsSelf | usesRaycast | usesRoutine,
        
		// suicide bomber
        0,
        
		// super sheep
        isAnimal | usesController | usesDetonateOnImpact | holdsSelf | spawnsSelf,
        
		// mine strike
        usesCursor | spawnsSelf | usesAirStrike,
        
		// baseball bat
        isMele | usesAim | holdsSelf,
        
		// teleport
        usesCursor | holdsSelf,
        
		// salvation army lady
        spawnsSelf | usesPhysics | isAnimal | isCluster | usesController | holdsSelf,
        
		// sheep strike
        usesCursor | usesAirStrike,
        
		// indian nuclear nuclear test
        usesRoutine | holdsCustom,
        
		// freeze
        isMeta | holdsSelf,
    
    // row 5
        
		// invisibility
        isMeta,
        
		// sheep launcher
        usesAim | usesPhysics | usesController | usesDetonateOnImpact | holdsLauncher,
        
		// earth quake
        usesRoutine | holdsCustom,
        
		// long bow
        usesAim | usesPhysics | multiUse | usesDetonateOnImpact | holdsSelf,
        
		// prod
        isMele | holdsSelf,
        
		// mole
        spawnsSelf | usesPhysics | isAnimal | usesDetonateOnImpact | usesFuse,
        
		// mole squadron
        usesCursor | spawnsSelf | usesAirStrike,
        
		// girder pack
        usesCursor | multiUse,
        
		// scales of justice
        usesRoutine | holdsSelf,
        
		// MB bomb
        usesCursor | usesDetonateOnImpact | usesAirStrike,
        
		// carpet bomb
        usesCursor | usesPhysics | usesDetonateOnImpact | spawnsSelf | usesAirStrike,
        
		// armageddon
        usesRoutine | holdsCustom,
        
		// magic bullet
        usesCursor | usesAim | usesHoming | usesDetonateOnImpact | holdsLauncher,
    
    // sub weapons (ones you cannot directly use, but can spawn as a result of other weapons)
    
        // cluster/mortar/salvation army/ming vase fragment
        usesPhysics | usesDetonateOnImpact | usesFuse | spawnsSelf,
        
		// napalm / fire
        usesPhysics | usesDetonateOnImpact | usesFuse | spawnsSelf,
        
		// skunk gas
        isParticle | usesFuse | spawnsSelf,
        
		// comet from armageddon
        usesPhysics | usesDetonateOnImpact | spawnsSelf,
        
		// mail from mail strike
        usesPhysics | usesDetonateOnImpact | spawnsSelf,
        
		// carpet from carpet bomb
        usesPhysics | usesDetonateOnImpact | spawnsSelf,
        
		// fake mine
        usesPhysics | usesFuse | spawnsSelf
};
    
    

// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * finds a free weapon slot available
 */
short findFreeWeaponSlot()
{
	// loop till an active slot is found:
	short i=0;
	for(i=0; i<MAX_WEAPONS; i++)
	{
		if((Weapon_active & (unsigned short)((unsigned short)1<<(i))) <= 0)
			return i;
	}
	
	// nothing found, return error code
	return -1;
}


/**
 * detonates the weapon of index
 *
 * @param index the index of the weapon to detonate
*/
void detonateWeapon(short index)
{
	// no longer active
	Weapon_active &= ~((unsigned short)1<<(index));
	
	// for debug: always create an explosion for now
	Explosion_spawn(Weapon_x[index], Weapon_y[index], 10, 10, TRUE);
	
	// if this is a cluster weapon, spawn some cluster items
	if(Weapon_props[(short)Weapon_type[index]] & isCluster)
	{
		int i;
		for(i=0; i<5; i++)
			Weapons_spawn(WFragment, Weapon_x[index], Weapon_y[index], -2+i, -3, 5*TIME_MULTIPLIER);
	}
	
	// focus back on current worm
	Camera_focusOn(&Worm_x[(short)Worm_currentWorm], &Worm_y[(short)Worm_currentWorm]);
}


// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * This spawns a Weapon item in the game.
 *
 * Weapons have a type, position, initial x/y velocity, time and various properties.
 * The properties are bitmasked onto a char.
 * Valid Properties:
   - usesVelocity
 * - usesGravity
 * - usesTimer
 * - usesHoming
 * - usesMovement
 * - usesController
 *
 * @param type a char storing the type of weapon item this is, as defined by the enumeration Weapons.
 * @param x the starting x position of the weapon
 * @param y the starting y position of the weapon
 * @param xVelocity the starting x velocity of the weapon
 * @param yVelocity the starting y velocity of the weapon
 * @param time weapons use time for different purposes, (e.g. fuse length)
 * @param properties a char that is used as a bitmarked variable for the types of properties this weapon requires
 * @returns a char that is the index of the weapon slot this weapon was spawned into, or -1 if no slot was available.
*/
char Weapons_spawn(char type, short x, short y, char xVelocity, char yVelocity, unsigned short time)
{
	// find a free slot, if none are available, we are unable to spawn this weapon (should never happen)
	short slot = findFreeWeaponSlot();
	if(slot==-1)
		return slot;

	// set it's various properties
	Weapon_type[slot] = type;
	Weapon_x[slot] = x;
	Weapon_y[slot] = y;
	Weapon_time[slot] = time;
	
	// set the weapon active, and unsettled:
	Weapon_active |= (unsigned short)1<<(slot);
	Weapon_settled &= ~((unsigned short)1<<(slot));
	
	// if this weapon DOESN'T use physics, we don't have to instantiate a physObj,
	// and since this weapon doesn't use physics it will never call physics updates on its null physObj anyway
	if(Weapon_props[(short)type] & usesPhysics)
	{
		// make a new collider and physics object for this weapon
		Collider coll = new_Collider(COL_UDLR, 2, 2, 2, 2);
		Weapon_physObj[slot] = new_PhysObj(&Weapon_x[slot], &Weapon_y[slot], &Weapon_xVelo[slot], &Weapon_yVelo[slot], 0.7f, 1.0f, (char)slot, &Weapon_settled, coll);
		
		// set initial velocity
		Physics_setVelocity(&Weapon_physObj[slot], xVelocity, yVelocity, FALSE, TRUE);
		
		// focus the camera on this weapon (hack for now until better camera logic is made)
		Camera_focusOn(&Weapon_x[(short)slot], &Weapon_y[(short)slot]);
	}

	return slot;	
}


/**
 * sets the target for our weapon
 * @param x - the x target pos
 * @param y - the y target pos
 */
void Weapons_setTarget(short x, short y)
{
	Weapon_targetX = x;
	Weapon_targetY = y;
	
	// if the current weapon is a an airstrike type, go up and spawn some things
	if(Game_currentWeaponProperties & usesAirStrike)
	{

		// air strikes are just bazookas in disguise
		short spawnItem = WBazooka;
		char spawnCount = 5;

		switch(Game_currentWeaponSelected)
		{
			case WNapalmStrike:
				spawnItem = WFire;
				break;
			case WMailStrike:
				spawnItem = WMail;
				break;
			case WMineStrike:
				spawnItem = WFakeMine;
				break;
			case WSheepStrike:
				spawnItem = WSheep;
				break;
			case WCarpetBomb:
				spawnItem = WCarpet;
				break;
			case WMoleSquadron:
				spawnItem = WMole;
				break;
			case WMBBomb:
				spawnItem = WMBBomb;
				spawnCount = 1;
				break;
			case WConcreteDonkey:
				spawnItem = WConcreteDonkey;
				spawnCount = 1;
				break;
			default:
				spawnItem = WBazooka;
				break;
		
		}// swatch 
		
		const char spawnXVelo = (Game_currentWeaponState & strikeLeft) ? -1 : 1;
		
		// if only 1
		if(spawnCount==1)
		{
			Weapons_spawn(spawnItem, x, -50, spawnXVelo, 0, 5*TIME_MULTIPLIER);
			return;
		}
		
		// for now, spawn 5 weapons in a line above the target
		int i;
		for(i=-2; i<=2; i++)
		{
			Weapons_spawn(spawnItem, x+(i*5), -50, spawnXVelo, 0, 5*TIME_MULTIPLIER);
		}
		
		// un-set target picked
		Game_currentWeaponState &= ~targetPicked;
		Game_xMarkPlaced = FALSE;
	}
}


/**
 * update all weapons in game every frame
 */
void Weapons_update()
{
	// loop over all weapons, and update the active ones as necessary
	short i=0;
	for(i=0; i<MAX_WEAPONS; i++)
	{
		// check if it's active
		if(Weapon_active & (unsigned short)((unsigned short)1<<(i)))
		{

			// if weapon is out of bounds deactivate it
			if(Map_isOOB(Weapon_x[(short)i], Weapon_y[(short)i]))
			{
				Weapon_active &= ~(unsigned short)((unsigned short)1<<(i));
				continue;				
			}
			
			// if the weapon uses a timer, decrement and possibly detonate
			/*
			  note: since all weapons can technically "timeout"
			  I should just make all weapons use a default timer...
			  I can free up a flag later for more important biz
			  So for now, we'll add "TRUE ||" so all weapons default timeout
			*/
			if(TRUE || Weapon_props[(short)Weapon_type[i]] & usesFuse)
			{
				Weapon_time[i]--;
				if(Weapon_time[i]<=0)
				{
					detonateWeapon(i);
					continue;		
				}
			}// end if uses fuse/timer
			
			// if a weapon uses homing, adjust it's velocity appropriately
			if(Weapon_props[(short)Weapon_type[i]] & usesHoming)
			{
				// adjust the weapons velocity towards it's target, less aggressively as it gets closer
				short deltaX = (short)((Weapon_targetX - Weapon_x[i])*0.25f);
				short deltaY = (short)((Weapon_targetY - Weapon_y[i])*0.25f);
				
				Weapon_xVelo[i] += deltaX;
				Weapon_yVelo[i] += deltaY;
			}//end if uses homing
			
			// if a weapon needs user input to control it, read that now:
			if(Weapon_props[(short)Weapon_type[i]] & usesController)
			{
				// TO-DO: implement controller logic	
			}
				
			// if this weapon uses physics, lets update that shit
			if(Weapon_props[(short)Weapon_type[i]] & usesPhysics)
			{
				// if the Weapon is considered "settled" no need for physics
				if(!(Weapon_settled & (unsigned short)1<<(i)))
				{
					// apply wind and gravity
					Physics_setVelocity(&Weapon_physObj[i], (Game_wind/10), 1, TRUE, FALSE);
					
					// do physics and collision for Weapon
					Physics_apply(&Weapon_physObj[i]);
					
					// if the weapon has detonate on impact, we should detonate if it hit something..
					if((Weapon_props[(short)Weapon_type[i]] & usesDetonateOnImpact) && (Weapon_physObj[i].col.collisions>0))
						detonateWeapon(i);
						
				}// end if uses physics
						
			}// end if unsettled (needs Physics)

		}// endif active

	}// next i
}


/**
 * when user fires a weapon
 * @param charge - the charge amount fired with
 */
void Weapons_fire(short charge)
{
	// before we spawn the weapon we need to make sure we have all the params ready to go...
	
	// get the direction our current worm is facing
	char facingLeft = (Worm_dir & (unsigned short)1<<(Worm_currentWorm))>0;
	
	// spawn weapon at center of worm
	short spawnX = Worm_x[(short)Worm_currentWorm];
	short spawnY = Worm_y[(short)Worm_currentWorm];
	
	// adjust spawn point if it's a droppable
	if(Game_currentWeaponProperties & isDroppable)
	{
		spawnY += 5;
		spawnX += (facingLeft) ? -14 : 2;
	}

	// default spawn direction unless weapon uses aim or charge (charge is implied with aim)
	short dirX=0;
	short dirY=0;
	if(Game_currentWeaponProperties & usesAim)
	{
		dirX = Weapon_aimPosList[(Game_aimAngle<9) ? Game_aimAngle : 9-(Game_aimAngle-9)][0];
		dirY = Weapon_aimPosList[(Game_aimAngle<9) ? Game_aimAngle : 9-(Game_aimAngle-9)][1];
		
		if(Game_aimAngle>=10)
			dirY *= -1;
		if(facingLeft)
			dirX *= -1;
	}
	
	// default no velocity unless weapon uses charge
	short veloX=0;
	short veloY=0;
	if(Game_currentWeaponProperties & usesCharge)
	{
		veloX = dirX * (float)((float)charge/254.0f);
		veloY = dirY * (float)((float)charge/254.0f);
	}
	
	// for now, we'll always default to a 3 second timer
	short fuseLength = (3 * TIME_MULTIPLIER);
	
	// finally spawn the weapon with it's params!
	Weapons_spawn(Game_currentWeaponSelected, spawnX, spawnY, veloX, veloY, fuseLength);
}


/**
	Draws all the in-game, on-screen Weapon objects.
*/
void Weapons_drawAll()
{
	short screenX, screenY;
	
	// loop over all weapons and draw them if active:
	short i;
	for(i=0; i<MAX_WEAPONS; i++)
	{
		if(Weapon_active & ((unsigned short)1<<(i)))
		{
			screenX=Weapon_x[i];
			screenY=Weapon_y[i];
			if(worldToScreen(&screenX, &screenY))
			{	
				// if weapon uses spawnSelf, we can use it's same sprite as from the menu
				if(Weapon_props[(short)Weapon_type[i]] & spawnsSelf)
				{
					// for debug we will just draw a generic circle (borrowed from the charge sprites) for the weapon
					ClipSprite16_OR_R(screenX-2, screenY-2, 11, spr_weapons[(short)Weapon_type[i]], lightPlane);
					ClipSprite16_OR_R(screenX-2, screenY-2, 11, spr_weapons[(short)Weapon_type[i]], darkPlane);

				}else{	
					// for debug we will just draw a generic circle (borrowed from the charge sprites) for the weapon
					ClipSprite8_OR_R(screenX-2, screenY-2, 4, spr_chargeSmall, lightPlane);
					ClipSprite8_OR_R(screenX-2, screenY-2, 4, spr_chargeSmall, darkPlane);
				}


			}// end if on screen
			
		}// end if active

	}// next i
}


/**
 * returns the index of the first active explosion, or -1 if none are active
 * 
 * @return char index of first active explosion
 */
char Weapons_getFirstActive()
{
	short i;
	for(i=0; i<MAX_WEAPONS; i++)
	{
		if(Weapon_active & (unsigned short)((unsigned short)1<<(i)))
			return i;
	}
	
	return -1;
}
