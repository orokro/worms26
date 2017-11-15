// C Source File
// Created 11/11/2017; 11:34:17 PM

#include "../Headers/System/Main.h"

/*
	As desribed in our Keys.h file,
	
	these three unsigned its will use bit masks to store the state of each
	logical key we care about. Bitwise defines have been provided in Keys.h
	with the proper bit masks for each key
	
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
*/
long keysDown, keysState, keysUp;

// this will be called early in our main-update loop, so all further game-logic
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

