// C Source File
// Created 11/11/2017; 11:34:17 PM

#include "../Main.h"

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
	// we can use a temporary integer to save the new state of the keys
	// then we can use that temporary integer to compare for the keysDown and keysUp state integers
	long tempKeysState = 0;
	
	// use ternary operators to apply our bit masks to the keyState integer.
	// with the OR operation, we can turn the proper bits to 1
	// and with AND ~ we can turn off bits
	(_keytest(RR_2ND) || _keytest(RR_ENTER)) ? (tempKeysState |= keyAction) : (tempKeysState &= ~keyAction);
	(_keytest(RR_DIAMOND)) ? (tempKeysState |= keyJump) : (tempKeysState &= ~keyJump);
	(_keytest(RR_ALPHA)) ? (tempKeysState |= keyBackflip) : (tempKeysState &= ~keyBackflip);
	(_keytest(RR_SHIFT)) ? (tempKeysState |= keyCameraControl) : (tempKeysState &= ~keyCameraControl);
	(_keytest(RR_ESC)) ? (tempKeysState |= keyEscape) : (tempKeysState &= ~keyEscape);
	(_keytest(RR_F1) || _keytest(RR_CATALOG)) ? (tempKeysState |= keyWeaponsSelect) : (tempKeysState &= ~keyWeaponsSelect);
	(_keytest(RR_APPS)) ? (tempKeysState |= keyWormSelect) : (tempKeysState &= ~keyWormSelect);
	(_keytest(RR_LEFT)) ? (tempKeysState |= keyLeft) : (tempKeysState &= ~keyLeft);
	(_keytest(RR_RIGHT)) ? (tempKeysState |= keyRight) : (tempKeysState &= ~keyRight);
	(_keytest(RR_UP)) ? (tempKeysState |= keyUp) : (tempKeysState &= ~keyUp);
	(_keytest(RR_DOWN)) ? (tempKeysState |= keyDown) : (tempKeysState &= ~keyDown);
	(_keytest(RR_PLUS)) ? (tempKeysState |= keyGirderRight) : (tempKeysState &= ~keyGirderRight);
	(_keytest(RR_MINUS)) ? (tempKeysState |= keyGirderLeft) : (tempKeysState &= ~keyGirderLeft);
	(_keytest(RR_1)) ? (tempKeysState |= key1) : (tempKeysState &= ~key1);
	(_keytest(RR_2)) ? (tempKeysState |= key2) : (tempKeysState &= ~key2);
	(_keytest(RR_3)) ? (tempKeysState |= key3) : (tempKeysState &= ~key3);
	(_keytest(RR_4)) ? (tempKeysState |= key4) : (tempKeysState &= ~key4);
	(_keytest(RR_5)) ? (tempKeysState |= key5) : (tempKeysState &= ~key5);
	
	// reset our buffers, since we will write all the bits in the upcomming loop anyway
	keysDown = 0;
	keysUp = 0;
	
	// loop to update our our keysDown and keysUp variables
	long i=0;
	for(i=0; i<32; i++)
	{
		
		// get the previus value of the key
		char previousState = (char)(((keysState) & ((long)1<<(i))) > 0);
		
		// current state
		char currentState = (char)(((tempKeysState) & ((long)1<<(i))) > 0);
		
		// set keydown state:
		// if it was down on the last frame, then this should just be 0
		(!previousState && currentState) ? (keysDown |= (long)((long)1<<(i))) : (keysDown &= ~(long)((long)1<<(i)));
		
		// if it was down on the last frame, and is no longer down,
		// then it should set keyUp to true:
		(previousState && !currentState) ? (keysUp |= (long)((long)1<<(i))) : (keysUp &= ~(long)((long)1<<(i)));
	}
	
	// now that we've updated the keysDown and keysUp int buffers, we can just copy over the current state
	keysState = tempKeysState;
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

