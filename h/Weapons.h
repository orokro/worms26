#ifndef WEAPONS_H
#define WEAPONS_H

/* ======================================================================================================================================
   WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WEAPONS +++ WE
   ====================================================================================================================================== */

// Weapons defines
#define MAX_WEAPONS 10


// Define bitmask flags for the types of properties a weapon can have:
// when spawning a weapon, these can be ORed together to create it's logic
#define usesAim 							0b0000000000000001
#define usesCharge 						0b0000000000000010
#define usesCursor 						0b0000000000000100
#define usesPhysics 					0b0000000000001000
#define usesWind 							0b0000000000010000
#define usesHoming 						0b0000000000100000
#define usesFuse 							0b0000000001000000
#define usesDetonation 				0b0000000010000000
#define usesController 				0b0000000100000000
#define usesDetonateOnImpact 	0b0000001000000000
#define isAnimal 							0b0000010000000000
#define isCluster 						0b0000100000000000
#define isParticle 						0b0001000000000000				
#define isMele 								0b0010000000000000
#define spawnsSelf 						0b0100000000000000
#define multiUse 							0b1000000000000000


/*
  enumerate our list of weapons, with matching index positions as described in the array above
  note: the last row is for secondary weapons: they don't use the same game logic as the primaries
  
  also note: these enumerations are also used for the current selected weapon, which doesn't
  necessarily have an in-game object, or might be different.
  
  For isntance, a bow looks like a bow in the hand of the worm, but bow game objet is drawn as an arrow
  Where as a cluster bomb is drawn as a grenade in game, but spawns WFragments, which aren't a selectable
  weapon, only a weapon that exists as a side effect of a cluster or mortar, or etc. exploding.
  
*/
enum Weapons {
	WJetPack,				WBazooka, 				WGrenade, 	WShotGun,		WFirePunch, 	WDynamite, 		WAirStrike, 		WBlowTorch, 	WNinjaRope, 			WSuperBanana, 		WPetrolBomb, 	WMadCows, 				WSkipGo,
	WLowGravity, 		WHomingMissle, 		WCluster,		WHandGun,		WDragonBall,	WMine, 				WNapalmStrike, 	WDrill, 			WBungeeCord,			WHolyHandGrenade,	WSkunk, 			WOldLady, 				WSurrender,
	WFastWalk,			WMortar, 					WBanana, 		WUzi,				WKakamaze, 		WSheep, 			WMailStrike, 		WGirder, 			WParachute,				WFlameThrower, 		WMingVase, 		WConcreteDonkey, 	WSelectWorm,
	WLaserSight, 		WHomingPigeon, 		WAxe,				WMiniGun,		WSuicideBomb, WSuperSheep, 	WMineStrike, 		WBaseballBat, WTeleport, 				WSalvationArmy, 	WSheepStrike, WNuclearTest, 		WFreeze,
	WInvisibility,	WSheepLauncher,		WQuake, 		WLongbow,  	WProd, 				WMole, 				WMoleSquadron, 	WGirderPack, 	WScalesOfJustice,	WMBBomb, 					WCarpetBomb, 	WArmageddon, 			WMagicBullet,
	WFragment, 			WFire,						WSkunkGas,	WComet, 		WMail, 				WCarpet, 			WFakeMine
};

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
		- Coment (from Armegeddon)
	
	Below is an array of 16 bit shorts we will use to bitwise store the properties of
	each weapon type, so we can optimize the weapon routines to reuse code
*/
extern unsigned short Weapon_props[72];


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
 * @param yVelocity the staarting y velocity of the weapon
 * @param time weapons use time for different purposes, (e.g. fuse length)
 * @param properties a char that is used as a bitmasked variable for the types of properties this weapon requires
*/
extern void Weapons_spawn(char, short, short, char, char, unsigned short);


/**
 * Updates all the currently active weapons, should be called once per frame.
*/
extern void Weapons_update();


/**
 * When a weapon is a targeted weapon, such as Air Strike or Homing Missle, this sets the current X/Y target.
 *
 * @param x the x position in world space for the weapon target.
 * @param y the y posiiion in world space for the weapon target.
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

#endif
