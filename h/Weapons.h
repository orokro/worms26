/*
	Weapons.h
	---------
	
	Main header file for the Weapons.c module
*/

#ifndef WEAPONS_H
#define WEAPONS_H

// Weapons defines
#define MAX_WEAPONS 10

#import "PhysCol.h"


// Define bitmask flags for the types of properties a weapon can have:
// when spawning a weapon, these can be ORed together to create it's logic
#define usesAim 				0b00000000000000000000000000000001	// true if this weapon needs to be aimable
#define usesCharge 				0b00000000000000000000000000000010	// true if this weapon also charges while firing
#define usesCursor 				0b00000000000000000000000000000100	// true if requires the cursor to pick an x/y location before firing
#define usesPhysics 			0b00000000000000000000000000001000	// true if the weapon object that is spawned will use physics system
#define usesWind 				0b00000000000000000000000000010000	// true if the weapon should be affected by wind physics
#define usesHoming 				0b00000000000000000000000000100000	// true if weapon object that is spawned needs homing functionality in it's update loop
#define usesFuse 				0b00000000000000000000000001000000	// true if the weapon object that is spawned has a fuse timer logic
#define usesDetonation 			0b00000000000000000000000010000000	// true if the weapon object that is spawned can be de
#define usesController 			0b00000000000000000000000100000000	// true if inputs (i.e. 2nd, or Up/Down/Left/Right) may affect the weapon's uupdate logic
#define usesDetonateOnImpact 	0b00000000000000000000001000000000	// true if it should explode when touching land
#define isAnimal 				0b00000000000000000000010000000000	// true if it has animal update logic
#define isCluster 				0b00000000000000000000100000000000	// true if it needs to spawn other items when it detonates
#define isParticle 				0b00000000000000000001000000000000	// true if it's a object that has alternate physics (particular routine)
#define isMele 					0b00000000000000000010000000000000	// true if it's a mele-type weapon
#define spawnsSelf 				0b00000000000000000100000000000000	// true if the weapon object that is spawned should use the same sprite as it is in the menu
#define multiUse 				0b00000000000000001000000000000000	// true if the weapon doesn't end turn
#define usesRaycast				0b00000000000000010000000000000000	// true if the requires firing uses raycasting instead of spawning objects
#define holdsSelf				0b00000000000000100000000000000000	// true if the weapon should use it's menu sprite in the worms hand
#define holdsLauncher			0b00000000000001000000000000000000	// true if the weapon should use the generic rocket launcher when equipped
#define holdsCustom				0b00000000000010000000000000000000  // true if there needs to be custom switch logic for what the worm should hold
#define usesAirStrike			0b00000000000100000000000000000000  // weapons that spawn groups of things in the sky
#define usesRoutine 			0b00000000001000000000000000000000  // true if the weapon needs custom per-frame logic	
#define isMeta 					0b00000000010000000000000000000000  // if weapon is meta (affects the round / game state)	
#define isDroppable 			0b00000000100000000000000000000000  // true if its droppable (dynamite, mines, ming vase, etc)	
#define usesConstantGravity 	0b00000001000000000000000000000000  // true if weapon is affected by constant gravity (e.g. longbow, magic bullet, etc)
#define usesJumping 			0b00000010000000000000000000000000  // true if animal jumps
#define doesntEndTurn			0b00000100000000000000000000000000  // true if weapon doesn't end turn when used

/*
  enumerate our list of weapons, with matching index positions as described in the array above
  note: the last row is for secondary weapons: they don't use the same game logic as the primaries
  
  also note: these enumerations are also used for the current selected weapon, which doesn't
  necessarily have an in-game object, or might be different.
  
  For instance, a bow looks like a bow in the hand of the worm, but bow game objet is drawn as an arrow
  Where as a cluster bomb is drawn as a grenade in game, but spawns WFragments, which aren't a selectable
  weapon, only a weapon that exists as a side effect of a cluster or mortar, or etc. exploding.
  
*/
enum Weapons {
	WJetPack,			WBazooka, 			WGrenade, 	WShotGun,	WFirePunch, 	WDynamite, 		WAirStrike, 	WBlowTorch, 	WNinjaRope, 		WSuperBanana, 		WPetrolBomb, 	WMadCows, 			WSkipGo,
	WLowGravity, 		WHomingMissile, 	WCluster,	WHandGun,	WDragonBall,	WMine, 			WNapalmStrike, 	WDrill, 		WBungeeCord,		WHolyHandGrenade,	WSkunk, 		WOldLady, 			WSurrender,
	WFastWalk,			WMortar, 			WBanana, 	WUzi,		WKamikaze, 		WSheep, 		WMailStrike, 	WGirder, 		WParachute,			WFlameThrower, 		WMingVase, 		WConcreteDonkey, 	WSelectWorm,
	WLaserSight, 		WHomingPigeon, 		WAxe,		WMiniGun,	WSuicideBomb, 	WSuperSheep, 	WMineStrike, 	WBaseballBat, 	WTeleport, 			WSalvationArmy, 	WSheepStrike,	WNuclearTest, 		WFreeze,
	WInvisibility,		WSheepLauncher,		WQuake, 	WLongbow,  	WProd, 			WMole, 			WMoleSquadron, 	WGirderPack, 	WScalesOfJustice,	WMBBomb, 			WCarpetBomb, 	WArmageddon, 		WMagicBullet,
	WFragment, 			WFire,				WSkunkGas,	WComet, 	WMail, 			WCarpet, 		WFakeMine,
};


// names of all weapons
extern const char weaponNames[65][16];

// weapons globals
extern char Weapon_type[MAX_WEAPONS];
extern short Weapon_x[MAX_WEAPONS];
extern short Weapon_y[MAX_WEAPONS];
extern PhysObj Weapon_physObj[MAX_WEAPONS];
extern char Weapon_xVelo[MAX_WEAPONS];
extern char Weapon_yVelo[MAX_WEAPONS];
extern unsigned short Weapon_time[MAX_WEAPONS];
extern unsigned short Weapon_active;
extern unsigned short Weapon_settled;
extern short Weapon_targetX;
extern short Weapon_targetY;
extern char Weapon_aimPosList[10][2];


/*
	there are currently 69 different types of weapon objects that can be on screen
	and behave differently.
	
	The game has 65 weapons / tools the user can choose from in the menu
	
	And there are four extra weapons that can spawn as a result of another weapon:
		- fragments (from cluster bombs, mortars, etc)
		- Fire / Napalm
		- Skunk Gas / Poison Gas
		- Coment (from Armageddon)
	
	Below is an array of 16 bit shorts we will use to bitwise store the properties of
	each weapon type, so we can optimize the weapon routines to reuse code
*/
extern unsigned long Weapon_props[72];


// weapons function prototypes

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
 * @param properties a char that is used as a bitmasked variable for the types of properties this weapon requires
 * @returns a char that is the index of the weapon slot this weapon was spawned into, or -1 if no slot was available.
*/
extern char Weapons_spawn(char, short, short, char, char, unsigned short);


/**
 * detonates the weapon of index
 *
 * @param index the index of the weapon to detonate
*/
extern void Weapons_detonateWeapon(short index);


/**
 * Updates all the currently active weapons, should be called once per frame.
*/
extern void Weapons_update();


/**
 * When a weapon is a targeted weapon, such as Air Strike or Homing Missile, this sets the current X/Y target.
 *
 * @param x the x position in world space for the weapon target.
 * @param y the y position in world space for the weapon target.
*/
extern void Weapons_setTarget(short, short);


/**
 * When the user fires a weapon
*/
extern void Weapons_fire(short charge);


/**
	Draws all the in-game, on-screen Weapon objects.
*/
extern void Weapons_drawAll();


/**
 * @brief Returns the index of the first active weapon, or -1 if none are active
 * 
 * @return char index of first active weapon
 */
extern char Weapons_getFirstActive();

#endif
