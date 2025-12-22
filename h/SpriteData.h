/*
	SpriteData.h
	------------
	
	Main header file for the SpriteData.c module
*/

#ifndef SPRITEDATA_H
#define SPRITEDATA_H

/* NOTE:
	These must be defined in SpriteData.c as non-static, ideally const.
	Example change:
		static unsigned long spr_Cloud_Dark[]  ->  const unsigned long spr_Cloud_Dark[];
*/

/* CandleCake */
extern const unsigned long spi_CandleCake_Dark[];
extern const unsigned long spi_CandleCake_Light[];

/* Charge meters */
extern const unsigned char spr_chargeLarge[];
extern const unsigned char spr_chargeMed[];
extern const unsigned char spr_chargeSmall[];

/* Cloud */
extern const unsigned long spr_Cloud_Dark[];
extern const unsigned long spr_Cloud_Light[];
extern const unsigned long spr_Cloud_Mask[];

/* Crates / UI */
extern const unsigned short spr_CrateHealth[];
extern const unsigned short spr_CrateTool[];
extern const unsigned short spr_CrateWeapon[];

extern const unsigned char spr_CrossHair[];
extern const unsigned char spr_CursorFill[];
extern const unsigned char spr_CursorOutline[];

/* Misc */
extern const unsigned short spr_Leaf[];

extern const unsigned char spr_Mine_Dark[];
extern const unsigned char spr_Mine_Light[];

/* Mountain: 2D array with 38 rows per mountain */
extern const unsigned long spr_Mountain[][38];

/* Oil */
extern const unsigned short spr_Oil_Dark[];
extern const unsigned short spr_Oil_Light[];

/* Parachute */
extern const unsigned short spr_Parachute[];

/* Weapon select */
extern const unsigned short spr_selectedWeaponBox[];
extern const unsigned short spr_selectedWeaponBoxMask[];

extern const unsigned short spr_SelectionArrowFrame1[];
extern const unsigned short spr_SelectionArrowFrame2[];

extern const unsigned short spr_timerBlink[];
extern const unsigned short spr_timerBorder[];
extern const unsigned short spr_timerMask[];

extern const unsigned long spr_tinyFont[];

/* Water */
extern const unsigned long spr_Water_Dark[];
extern const unsigned long spr_Water_Light[];
extern const unsigned long spr_Water_Mask[];

/* Weapons UI / icons */
extern const unsigned short spr_weaponSelect[];
extern const unsigned short spr_weapons[][11];

/* Wind meter */
extern const unsigned long spr_WindMeter[];
extern const unsigned long spr_WindMeter_Mask[];

/* Worm animations */
extern const unsigned short spr_WormFlip1_Left_Mask[];
extern const unsigned short spr_WormFlip1_Left_Outline[];
extern const unsigned short spr_WormFlip1_Right_Mask[];
extern const unsigned short spr_WormFlip1_Right_Outline[];

extern const unsigned short spr_WormFlip2_Left_Mask[];
extern const unsigned short spr_WormFlip2_Left_Outline[];
extern const unsigned short spr_WormFlip2_Right_Mask[];
extern const unsigned short spr_WormFlip2_Right_Outline[];

extern const unsigned short spr_WormFlip3_Left_Mask[];
extern const unsigned short spr_WormFlip3_Left_Outline[];
extern const unsigned short spr_WormFlip3_Right_Mask[];
extern const unsigned short spr_WormFlip3_Right_Outline[];

extern const unsigned short spr_WormFlip4_Left_Mask[];
extern const unsigned short spr_WormFlip4_Left_Outline[];
extern const unsigned short spr_WormFlip4_Right_Mask[];
extern const unsigned short spr_WormFlip4_Right_Outline[];

extern const unsigned short spr_WormJump_Left_Mask[];
extern const unsigned short spr_WormJump_Left_Outline[];
extern const unsigned short spr_WormJump_Right_Mask[];
extern const unsigned short spr_WormJump_Right_Outline[];

extern const unsigned short spr_WormLeft_Mask[];
extern const unsigned short spr_WormLeft_Outline[];
extern const unsigned short spr_WormRight_Mask[];
extern const unsigned short spr_WormRight_Outline[];

#endif /* SPRITEDATA_H */
