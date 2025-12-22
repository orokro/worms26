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
	
	The game only cares about the following keys:
	
	2nd - fire, place target, select weapon from weapons menu
	Enter - same as 2nd, but also acts as secondary fire for for weapons or droping from ropes, etc
	diamond - jump
	alpha - backflip
	shift - enable manual camera scrolling
	escape - pause game, or exit weapons menu
	catalog - open weapon menu
	F1 - open weapon menu
	apps - select worm in worm-select mode	
	up/down/left/right - move worm, aim, pan camera, move arrow, select weapon
	plus - rotate girder clockwise
	minus - rotate girder counter-clockwise
	1/2/3/4/5 - set fuse length or cluster size on timed / cluster weapons
	
	So we have a total of 20 keys we care about:
	2nd, Enter, diamond, alpha, shift, escape, catalog, apps, F1, up, down, left, right, plus, minus, 1, 2, 3, 4, 5

	Instead of having inline key tests and waiting for the key to release, we can do something better.
	
	This key system defines three longs, and some bitmasks. On every frame we will check all keys and update
	our longs appropriately.
	
	In the game logic, we can test these longs with their bitmasks.
	
	One long bit will only be 1 when keys are first down, one long bit will always reflect the state of the key
	and one long bit will only be true on the frame the key is released.
	
	This way, the key can test the keysDown long for the desired key, and it will only be true
	on the frame that the key was first pressed down.
	
	Some things, like walking or moving the cursor only care if the key is currently down,
	and instead can test the keysState long.
	
	This header will define the public keytest functions as well as the bitmasks for the keys we care about,
	and a key-update method that we will call at the beginning of every frame.
	
	This way, by the time the game logic is ready to check keys, the latest-states will be available.
	
	We will define three longs:
	keysDown
	keysState
	keysUp
	
	and provide public methods for testing keys with our custom defines, instead of the TI-89 keycodes
	
	For reference, our logical keys are:
	keyAction
	keyJump
	keyBackflip
	keyCameraControl
	keyEscape
	keyWeaponsSelect
	keyWormSelect
	keyLeft
	keyRight
	keyUp
	keyDown
	keyGirderRight
	keyGirderLeft
	key1
	key2
	key3
	key4
	key5
	
	C Source File
	Created 11/11/2017; 11:34:17 PM
*/


// includes
#include "Main.h"
#include "Keys.h"


// longs to store our bit wise key states in. We don't need all 32 bits.
long keysDown, keysState, keysUp;



// --------------------------------------------------------------------------------------------------------------------------------------



// this will be called early in our main-update loop, so all further game logic
// can have access to the current state of the logical keys
void Keys_update()
{
	// save the current keystate before we update it below, so we can compare agsint it
	// to see what changed
	long lastKeysState = keysState;
	
	// each bit-mask is a binary place, so we can just multiply its values and sum them
	// by the trutiness of those keys
	keysState = 0;
	keysState += (_keytest(RR_2ND) || _keytest(RR_ENTER)) * keyAction;
	keysState += _keytest(RR_DIAMOND) * keyJump;
	keysState += _keytest(RR_ALPHA) * keyBackflip;
	keysState += _keytest(RR_SHIFT) * keyCameraControl;
	keysState += _keytest(RR_ESC) * keyEscape;
	keysState += (_keytest(RR_F1) || _keytest(RR_CATALOG)) * keyWeaponsSelect;
	keysState += _keytest(RR_APPS) * keyWormSelect;
	keysState += _keytest(RR_LEFT) * keyLeft;
	keysState += _keytest(RR_RIGHT) * keyRight;
	keysState += _keytest(RR_UP) * keyUp;
	keysState += _keytest(RR_DOWN) * keyDown;
	keysState += _keytest(RR_PLUS) * keyGirderRight;
	keysState += _keytest(RR_MINUS) * keyGirderLeft;
	keysState += _keytest(RR_1) * key1;
	keysState += _keytest(RR_2) * key2;
	keysState += _keytest(RR_3) * key3;
	keysState += _keytest(RR_4) * key4;
	keysState += _keytest(RR_5) * key5;
	
	
	// if we XOR our current state (keysState) and our previous state (lastKeysState)
	// we will get just the keys that changed ON or OFF from the last frame
	long changedKeys = (lastKeysState ^ keysState);
	
	// if we AND the changed keys to our current keys, we can determine which keys were down this frame:
	keysDown = (long)(keysState & changedKeys);
	
	// if we AND it to our last state, we can detmine which keys were let go
	keysUp = (long)(lastKeysState & changedKeys);
}


// test if a key is down for the first time this frame
char Keys_keyDown(long keyCode)
{
	// return if the key is down on this frame:
	return (char)((keysDown & keyCode)>0);
}


// test the current state of any key
char Keys_keyState(long keyCode)
{
	// return if key is currently down on this frame
	return (char)((keysState & keyCode)>0);
}


// test if a key was let-go on this frame
char Keys_keyUp(long keyCode)
{
	// return if the key is let up on this frame:
	return (char)((keysUp & keyCode)>0);
}
