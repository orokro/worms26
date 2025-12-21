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
        spawnsSelf | usesPhysics,
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
        usesCursor | usesPhysics,
        // girder
        0,
        // parachute
        0,
        // flame thrower
        usesAim,
        // priceless ming vase
        spawnsSelf | usesFuse | isCluster | usesPhysics,
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
        usesCursor,
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



// spawns a weapon... simple enough
void Weapons_spawn(char type, short x, short y, char xVelocity, char yVelocity, unsigned short time)
{
	// find a free slot, if none are available, we are unable to spawn this weapon (should never happen)
	short slot = findFreeWeaponSlot();
	if(slot==-1)
		return;

	// set it's varius properties
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
		Physics_setVelocity(&Weapon_physObj[slot], xVelocity, yVelocity, FALSE);
		
		// focus the camera on this weapon (hack for now until better camera logic is made)
		Camera_focusOn(&Weapon_x[(short)slot], &Weapon_y[(short)slot]);
	
	}
	
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
		if(Weapon_active & (unsigned short)((unsigned short)1<<(i)))
		{
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
				// adjust the weapons velocity towards it's target, less agressively as it gets closer
				short deltaX = (short)((Weapon_targetX - Weapon_x[i])*0.25f);
				short deltaY = (short)((Weapon_targetY - Weapon_y[i])*0.25f);
				
				Weapon_xVelo[i] += deltaX;
				Weapon_yVelo[i] += deltaY;
			}//end if uses homing
			
			// if a weapon needs user input to controll it, read that now:
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
					// add gravity to Weapon
					Weapon_yVelo[i]++;
					
					// if it uses wind, lets add it now:
					if(Weapon_props[(short)Weapon_type[i]] & usesWind)
						Weapon_xVelo[i] += (Game_wind/10);
					
					// do physics and collision for Weapon
					Physics_apply(&Weapon_physObj[i]);
					
					// if the weapon has detonate on impact, we should denoate if it hit something..
					if((Weapon_props[(short)Weapon_type[i]] & usesDetonateOnImpact) && (Weapon_physObj[i].col.collisions>0))
						detonateWeapon(i);
						
				}// end if uses physics
						
			}// end if unsettled (needs Physics)			
		}// endif active
	}// next i
}


// when user fires a weapon
void Weapons_fire(short charge)
{
	// before we spawn the weapon we need to make sure we have all the params ready to go...
	
	// get the direction our current worm is facing
	char facingLeft = (Worm_dir & (unsigned short)1<<(Worm_currentWorm))>0;
	
	// spawn weapon at center of worm
	short spawnX = Worm_x[(short)Worm_currentWorm];
	short spawnY = Worm_y[(short)Worm_currentWorm];
	
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





















