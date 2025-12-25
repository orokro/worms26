/*
	CharacterController.h
	---------------------
	
	Main header file for the CharacterController.c module
*/

#ifndef CHARCTRL_H
#define CHARCTRL_H


// character controller function prototypes

/**
 * Updates the character based on user input for a frame turning the turn game mode.
*/
extern void CharacterController_update();


/**
 * @brief handles clean up after weapon has been decidedly used
 * 
 * @param noEndTurn - if set to 1, do not end the turn after weapon use
 */
extern void CharacterController_weaponConsumed(char noEndTurn);

#endif // CHARCTRL_H
