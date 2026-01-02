/*
	Keys.h
	------
	
	Main header file for the Keys.c module
*/

#ifndef KEYS_H
#define KEYS_H


// key defines

// our bitmasks for the keys we care about. we will use names that either reflect the physical button
// or the buttons game-logic name
// note the powers of 2! moving the bit one place increased by powers of 2. Yay computer science
#define keyAction (long)1			//00000000000000000000000000000001
#define keyJump (long)2				//00000000000000000000000000000010
#define keyBackflip (long)4			//00000000000000000000000000000100
#define keyCameraControl (long)8	//00000000000000000000000000001000
#define keyEscape (long)16			//00000000000000000000000000010000
#define keyWeaponsSelect (long)32	//00000000000000000000000000100000
#define keyWormSelect (long)64		//00000000000000000000000001000000
#define keyLeft (long)128			//00000000000000000000000010000000
#define keyRight (long)256			//00000000000000000000000100000000
#define keyUp (long)512				//00000000000000000000001000000000
#define keyDown (long)1024			//00000000000000000000010000000000
#define keyGirderRight (long)2048	//00000000000000000000100000000000
#define keyGirderLeft (long)4096	//00000000000000000001000000000000
#define key1 (long)8192				//00000000000000000010000000000000
#define key2 (long)16384			//00000000000000000100000000000000
#define key3 (long)32768			//00000000000000001000000000000000
#define key4 (long)65536			//00000000000000010000000000000000
#define key5 (long)131072			//00000000000000100000000000000000
#define keyAction2 (long)262144		//00000000000001000000000000000000
#define keyExit (long)524288		//00000000000010000000000000000000
#define keyF1 (long)1048576			//00000000000100000000000000000000
#define keyF2 (long)2097152			//00000000001000000000000000000000
#define keyF5 (long)4194304			//00000000010000000000000000000000
#define keyPlus (long)8388608		//00000000100000000000000000000000
#define keyMinus (long)16777216		//00000001000000000000000000000000


// note this key: because it's all 1's, it will always return true when & on any other int,
// so long as at least ONE bit is set. This way, we can test for any-key
#define keyAny (long)67108863		//00000000000001111111111111111111

// this works similar to keyAny, except just for the direction keys
#define keyCursors (long)1920			//00000000000000000000011110000000


// keys function prototypes

/**
 * Reads the current state of the key unput, and updates our bitwise buffers.
 * 
 * Three longs are defined: keysDown, keysState, keysUp.
 * These bits in these longs mirror the key states for each key matching our #defined bitmasks.
 * keysDown only has it's bits true on the single-frame a key is down on.
 * keysUp only has it's bits true for the single-frame a key is up on.
 * keysState has it's bits true on every frame the key is pressed.
*/
extern void Keys_update();


/**
 * Checks if a key is FIRST down ON THIS FRAME using it's bitmask.
 *
 * @param keyCode the bitmask for our logical key
 * @return a boolean char set to 0 or 1 if the key is down
*/
extern char Keys_keyDown(long);


/**
 * Checks if a key is pressed on this frame using it's bitmask.
 *
 * @param keyCode the bitmask for our logical key
 * @return a boolean char set to 0 or 1 if the key is pressed
*/
extern char Keys_keyState(long);


/**
 * Checks if a key is LET up ON THIS FRAME using it's bitmask.
 *
 * @param keyCode the bitmask for our logical key
 * @return a boolean char set to 0 or 1 if the key is let up
*/
extern char Keys_keyUp(long);

#endif