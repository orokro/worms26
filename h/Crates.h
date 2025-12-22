
#ifndef CRATES_H
#define CRATES_H

// crate defines
#define MAX_CRATES 8
#define crateHealth 0
#define crateWeapon 1
#define crateTool 2


// crate globals
extern short Crate_x[MAX_CRATES];
extern short Crate_y[MAX_CRATES];
extern char Crate_health[MAX_CRATES];
extern char Crate_type[MAX_CRATES];
extern unsigned short Crate_active;
extern unsigned short Crate_settled;
extern char parachuteCrate;


// crate function prototypes

/**
 * This spawns a single Crate in the game, of the given type.
 * 
 * @param type the type of crate to spawn, as one of our defines: crateHealth, crateWeapon, crateTool
 * @return TRUE or FALSE if a crate was successfully spawned
*/
extern char Crates_spawnCrate();


/**
 * Updates the Crates currently active, should be called once per frame.
*/
extern void Crates_update();

/**
 * Allows a worm to pick up the crate
 *
 * @param index the crate index to pickup
 * @param worm the index of the worm who picked up the crate
*/
extern void Crates_pickUp(short, short);


/**
 * @brief Draws all the crates
 * 
 */
extern void Crates_drawAll();

#endif