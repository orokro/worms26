/*
	Keys.c
	------
	
	In the previous version of the Worms source code, keys were tested directly
	and there were some ugly hacks associated with doing so.
	
	For instance, if I wanted to make sure a key wasn't tested every frame,
	I would write something ugly like this:
	
	if(_keytest(RR_2ND))
	{
		// wait until key is released
		while(_keytest(RR_2ND));
		
		// do a thing
	}
	
	The while loop would essentially lock (essentially "crash") the program until it's released.
	
	This way, when it detects a key down, it wouldn't resume the code until the
	key is up, ensuring it was only pressed once.
	
	This has a few downsides
		1) it can be abused. If you want to stop the clock, you can hold a button
		2) it will also pause physics
		3) screen goes black and white because flipping the gray buffer is paused
		4) it has those ugly while loops peppered throughout the keytest code
		
	This new key system works a bit differently.
	
	We define logical keys in a bit-mask system, where each key we care about
	has a bit assigned to it in a long variable.

	Each frame, we read the current state of the keys, and update three long
	variables:
	
		keysState - has bits set for every key currently pressed
		keysDown  - has bits set for every key that was JUST pressed this frame
		keysUp    - has bits set for every key that was JUST released this frame
	
	With these three variables, we can now test for key presses in a much cleaner way.
*/


// includes
#include "Main.h"
#include "Keys.h"


// longs to store our bit wise key states in. We don't need all 32 bits.
long keysDown, keysState, keysUp;



// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * Reads the current state of the key unput, and updates our bitwise buffers.
 * 
 * Three longs are defined: keysDown, keysState, keysUp.
 * These bits in these longs mirror the key states for each key matching our #defined bitmasks.
 * keysDown only has it's bits true on the single-frame a key is down on.
 * keysUp only has it's bits true for the single-frame a key is up on.
 * keysState has it's bits true on every frame the key is pressed.
*/
void Keys_update()
{
	// save the current keystate before we update it below, so we can compare against it
	// to see what changed
	long lastKeysState = keysState;
	
	// each bit-mask is a binary place, so we can just multiply its values and sum them
	// by the truthiness of those keys
	keysState = 0;
	keysState += (_keytest(RR_2ND) || _keytest(RR_ENTER)) * keyAction;
	keysState += _keytest(RR_ESC) * keyEscape;
	keysState += _keytest(RR_LEFT) * keyLeft;
	keysState += _keytest(RR_RIGHT) * keyRight;
	keysState += _keytest(RR_UP) * keyUp;
	keysState += _keytest(RR_DOWN) * keyDown;
	keysState += _keytest(RR_CLEAR) * keyExit;
	keysState += _keytest(RR_F1) * keyF1;
	keysState += _keytest(RR_F2) * keyF2;
	keysState += _keytest(RR_F5) * keyF5;
	keysState += _keytest(RR_PLUS) * keyPlus;
	keysState += _keytest(RR_MINUS) * keyMinus;

	// if we XOR our current state (keysState) and our previous state (lastKeysState)
	// we will get just the keys that changed ON or OFF from the last frame
	long changedKeys = (lastKeysState ^ keysState);
	
	// if we AND the changed keys to our current keys, we can determine which keys were down this frame:
	keysDown = (long)(keysState & changedKeys);
	
	// if we AND it to our last state, we can determine which keys were let go
	keysUp = (long)(lastKeysState & changedKeys);
}


/**
 * Checks if a key is FIRST down ON THIS FRAME using it's bitmask.
 *
 * @param keyCode the bitmask for our logical key
 * @return a boolean char set to 0 or 1 if the key is down
*/
char Keys_keyDown(long keyCode)
{
	// return if the key is down on this frame:
	return (char)((keysDown & keyCode)>0);
}


/**
 * Checks if a key is pressed on this frame using it's bitmask.
 *
 * @param keyCode the bitmask for our logical key
 * @return a boolean char set to 0 or 1 if the key is pressed
*/
char Keys_keyState(long keyCode)
{
	// return if key is currently down on this frame
	return (char)((keysState & keyCode)>0);
}


/**
 * Checks if a key is LET up ON THIS FRAME using it's bitmask.
 *
 * @param keyCode the bitmask for our logical key
 * @ret
 */
char Keys_keyUp(long keyCode)
{
	// return if the key is let up on this frame:
	return (char)((keysUp & keyCode)>0);
}
