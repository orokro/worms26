/*
	UI.c
	----

	This file will compartmentalize all the HUD / UI elements of the main game.
	(note: not menus, those go elsewhere)

	This will be in-line included in Draw.c to keep that file more readable.
*/


/**
 * @brief Draws the shot ray for gun-type weapons
 */
void drawShotRay(){
	
	// gtfo if no shot to draw
	if(shot_active==FALSE)
		return;
	shot_active=FALSE;

	GrayDrawClipLine2B(shot_sx, shot_sy, shot_ex, shot_ey, 3, lightPlane, darkPlane);
}


/**
 * @brief Sets up the shot ray coordinates for drawing
 * 
 * @param sx - start x
 * @param sy - start y
 * @param ex - end x
 * @param ey - end y
 */
extern void Draw_setShotRay(short sx, short sy, short ex, short ey)
{
	shot_ex = ex;
	shot_ey = ey;
	shot_sx = sx;
	shot_sy = sy;
	shot_active = TRUE;
}


/**
 * Draws the X picked in arrow mode
 */
void drawCursorAndXSpot()
{	
	// GTFO if our current weapon doesn't use cursors anyway
	if((Game_currentWeaponProperties & usesCursor)==0)
		return;
	
	short screenX = Game_cursorX;
	short screenY = Game_cursorY;
	
	// if we're actually in cursor mode draw the cursor on screen
	if(Game_mode==gameMode_Cursor)
	{		
		if(worldToScreen(&screenX, &screenY))
		{							
			const unsigned char* cursorOutlineSprite = (Game_currentWeaponState & strikeLeft) ? spr_CursorOutline : spr_CursorOutlineFlipped;
			const unsigned char* cursorFillSprite = (Game_currentWeaponState & strikeLeft) ? spr_CursorFill : spr_CursorFillFlipped;
			
			GrayClipSprite8_AND_R(screenX, screenY, 12, cursorOutlineSprite, cursorOutlineSprite, lightPlane, darkPlane);
			GrayClipSprite8_OR_R(screenX, screenY, 12, cursorFillSprite, cursorFillSprite, lightPlane, darkPlane);
		}
	}
	
	// GTFO if we don't have an x placed
	if((Game_currentWeaponState & targetPicked)==0)
		return;
		
	// convert to screen space & draw the sprites
	screenX = Game_xMarkSpotX;
	screenY = Game_xMarkSpotY;
	if(worldToScreen(&screenX, &screenY))
	{							
		GrayClipSprite8_AND_R(screenX, screenY, 8, spr_XSpotMask, spr_XSpotMask, lightPlane, darkPlane);
		GrayClipSprite8_OR_R(screenX, screenY, 8, spr_XSpot, spr_XSpot+8, lightPlane, darkPlane);

	}// end if on scree
}


/**
 * Draws either the selection arrow, or current worm arrow in Worm Select mode.
*/
void drawSelectArrow()
{
	static char frame=0;
	frame++;
	if(frame>7)
		frame=0;
	
	short x=Worm_x[(short)Worm_currentWorm]-9;
	short y=Worm_y[(short)Worm_currentWorm]-32;
	
	if(worldToScreen(&x, &y))
	{
		// take advantage of extgrah's sprite method to handle bit shifting and mem copying in one swoop!
		ClipSprite16_XOR_R(x, y, 16, ((frame<4) ? spr_SelectionArrowFrame1 : spr_SelectionArrowFrame2), darkPlane);
		ClipSprite16_XOR_R(x, y, 16, ((frame<4) ? spr_SelectionArrowFrame1 : spr_SelectionArrowFrame2), lightPlane);	
	}
}


/**
 * Draws the games timer
*/
void Draw_timer()
{
	// only draw the timer if the game is in select or turn mode!
	if(Game_timer>0 && (Game_mode==gameMode_WormSelect || Game_mode==gameMode_Turn || Game_mode==gameMode_Cursor || Game_mode==gameMode_WeaponSelect ||  Game_mode==gameMode_Pause))
	{
		// erase space for timer
		ClipSprite16_AND_R(2, 87, 11, spr_timerMask, lightPlane);
		ClipSprite16_AND_R(2, 87, 11, spr_timerMask, darkPlane);
		
		// if we have trace time, blink the border:
		if(Game_graceTimer>0 && (Game_graceTimer/3)%6<3)
		{
			ClipSprite16_OR_R(2, 87, 11, spr_timerBorder, lightPlane);
		}else
		{
			ClipSprite16_OR_R(2, 87, 11, spr_timerBorder, lightPlane);
			ClipSprite16_OR_R(2, 87, 11, spr_timerBorder, darkPlane);
		}
		
		// draw the time in the turn:
		short time = (Game_graceTimer>0) ? (short)(Game_graceTimer/TIME_MULTIPLIER) : (short)(Game_timer/TIME_MULTIPLIER);
		char txt[3];
		sprintf(txt, "%d", time);
		
		FontSetSys(F_6x8);

		GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
		DrawStr(3 + ((time<10) ? 3 : 0), 89, txt, A_NORMAL);
		GrayDBufSetHiddenAMSPlane(DARK_PLANE);
		DrawStr(3 + ((time<10) ? 3 : 0), 89, txt, A_NORMAL);
		
		// if the game timer is less than 5, the inside, not the border
		if((short)(Game_timer/TIME_MULTIPLIER)<=5 && (Game_timer/3)%6<3)
			ClipSprite16_XOR_R(2, 87, 11, spr_timerBlink, darkPlane);
		
	}// end if draw timer	
}


/**
 * Draws the games HUD (i.e. timer, wind, etc)
*/
void Draw_HUD()
{		
	// if the user has a weapon selected and the current mode is TURN, draw it's icon in the top right:
	if(Game_currentWeaponSelected!=-1 && Game_mode==gameMode_Turn)
	{
		// draw the box sprite for the selected weapon
		ClipSprite16_AND_R(2, 2, 13, spr_selectedWeaponBoxMask, lightPlane);
		ClipSprite16_AND_R(2, 2, 13, spr_selectedWeaponBoxMask, darkPlane);
		ClipSprite16_OR_R(2, 2, 13, spr_selectedWeaponBox, lightPlane);
		ClipSprite16_OR_R(2, 2, 13, spr_selectedWeaponBox, darkPlane);
		
		// draw the icon for the selected weapon
		ClipSprite16_OR_R(3, 3, 11, spr_weapons[(short)Game_currentWeaponSelected], lightPlane);
		ClipSprite16_OR_R(3, 3, 11, spr_weapons[(short)Game_currentWeaponSelected], darkPlane);
	}
	
	// copy the wind bar to both buffers:
	ClipSprite32_AND_R(126, 93, 5, spr_WindMeter_Mask, lightPlane);
	ClipSprite32_AND_R(126, 93, 5, spr_WindMeter_Mask, darkPlane);
	ClipSprite32_OR_R(126, 93, 5, spr_WindMeter, lightPlane);
	ClipSprite32_OR_R(126, 93, 5, spr_WindMeter, darkPlane);

	// GrayClipSprite32_AND_R(126, 93, 5, spr_WindMeter_Mask, spr_WindMeter_Mask, lightPlane, darkPlane);
	// GrayClipSprite32_OR_R(126, 93, 5, spr_WindMeter, spr_WindMeter, lightPlane, darkPlane);
	
	// current frame of animation:
	static char frame=0;
	if(++frame>33)
		frame=0;
	
	// draw current frame of animated bar
	ClipSprite32_OR_R(127, 94, 3, windSprites[(short)((frame/3)%3)], darkPlane);
	
	// reuse the wind bar in the center of the screen to draw the team health:
	// GrayClipSprite32_AND_R(64, 93, 5, spr_WindMeter_Mask, spr_WindMeter_Mask, lightPlane, darkPlane);
	// GrayClipSprite32_OR_R(64, 93, 5, spr_WindMeter, spr_WindMeter, lightPlane, darkPlane);
	ClipSprite32_AND_R(64, 93, 5, spr_WindMeter_Mask, lightPlane);
	ClipSprite32_AND_R(64, 93, 5, spr_WindMeter_Mask, darkPlane);
	ClipSprite32_OR_R(64, 93, 5, spr_WindMeter, lightPlane);
	ClipSprite32_OR_R(64, 93, 5, spr_WindMeter, darkPlane);

	// // draw the health for both teams:
	GrayClipSprite32_OR_R(64, 94, 3, teamHealthSprites_light, teamHealthSprites_dark, lightPlane, darkPlane);

	// draw the timer
	Draw_timer();	
}


/**
 * draws cake loading screen
 * @param amount - progress
 * @param total - total to calculate percentage
 */
void Draw_cake(short amount, short total)
{
	GrayDBufSetHiddenAMSPlane(DARK_PLANE);
	ClrScr();
	GrayDBufSetHiddenAMSPlane(LIGHT_PLANE);
	ClrScr();
	
	ClipSprite32_OR_R(64, 38, 23, spi_CandleCake_Dark, darkPlane);
	ClipSprite32_OR_R(64, 38, 23, spi_CandleCake_Light, lightPlane);	
	
	char progStr[40];
	sprintf(progStr, "Loading... %d%%", (short)((float)((float)amount/(float)total)*100));
	DrawStr(40,70, progStr, A_NORMAL);	

	// now flip the planes
	GrayDBufToggleSync();
	
	lightPlane = GrayDBufGetHiddenPlane(LIGHT_PLANE);
	darkPlane = GrayDBufGetHiddenPlane(DARK_PLANE);
}


/**
 * Draw's the cross hair and/or charge bar for the selected weapon
*/
void drawWeaponDetails()
{
	short i, x, y;

	// get the direction our current worm is facing
	char facingLeft = (Worm_dir & (unsigned short)1<<(Worm_currentWorm))>0;
	
	// get the x/y position of the current worm
	short wormX = Worm_x[(short)Worm_currentWorm];
	short wormY = Worm_y[(short)Worm_currentWorm];
	
	// the center is slightly off center
	if(facingLeft)
		wormX-=4;

	// if the weapon requires aiming or charge
	if(
		(Game_currentWeaponProperties & usesAim)
		||
		(Game_currentWeaponProperties & usesCharge)
		||
		(Game_currentWeaponState &= keepAimDuringUse)
		)
	{
	
		/*
			get the x/y components to use for the crosshair or the charge
			
			Charge is stored as a value between 0 and 18 (19 positions, using center twice)
			but our array only has 10 values... so we gotta pick values from this.
			
			The first value represents position 18, so the first 10 are:
				19-(Game_aimAngle-9), which would be 0-9 in the array
				
			Therefore, if the Game_aimAngle is less than 9, we want to use:
				Game_aimAngle, but flip the y component.
				
			The x component will be flipped if facing left
		*/
		char xComponent = Weapon_aimPosList[(Game_aimAngle<9) ? Game_aimAngle : 9-(Game_aimAngle-9)][0];
		char yComponent = Weapon_aimPosList[(Game_aimAngle<9) ? Game_aimAngle : 9-(Game_aimAngle-9)][1];
		
		if(Game_aimAngle>=10)
			yComponent *= -1;
		if(facingLeft)
			xComponent *= -1;
		
		// get screen pos
		x=wormX+xComponent;
		y=wormY+yComponent+4;
		worldToScreen(&x, &y);

		// draw aline if the user has laser sight enabled
		if(Game_stateFlags & gs_laserSight){

			short startX = wormX;
			short startY = wormY+4;
			worldToScreen(&startX, &startY);
			const short endX = startX + (xComponent * 10);
			const short endY = startY + (yComponent * 10);
			GrayDrawClipLine2B(x, y, endX, endY, 3, lightPlane, darkPlane);
		}

		// if the weapon is being charged, draw the charge circles
		if(Game_currentWeaponCharge>0)
		{
			/*
				Drawing the charge:
				
				We want to draw in reverse order, from closer to the cross hairs
				to the worm, so the sprites overlap nicely.
				
				Every other sprite will will alternate white and black.
				
				We have 9 total positions:
					- 3 large
					- 3 med
					- 3 small
					
				The charge total goes between 0 and 255, so if we integer divide by 31,
				we should get a value between 0-8
				
				we need to interpolate the sprites positions from the center of the worm
				to the target on the steps 0-8
				
				Note that x and y are already converted to screen coords, so for this
				we'll just convert the worms coords to screen and work just in screen
			*/
			
			// get worms screen pos (worldToScreen mutates)
			short wx = wormX;
			short wy = wormY+4;
			worldToScreen(&wx, &wy);
			
			// list of sprites based on size from 0-2
			const char *spritePtrs[] = {spr_chargeSmall, spr_chargeMed, spr_chargeLarge};
			
			// calc the charge amount in our scale
			char chargeAmt = (Game_currentWeaponCharge/31);
	
			// loop from furthest to nearest
			for(i=chargeAmt; i>=0; i--)
			{
				// calc the scale index we're in
				char scaleIdex = (i/3);
				
				// calculate the size of the sprite for this step
				char spriteHeight = 4 + (2*scaleIdex);
				
				// interpolate sprite position and offset:
				short sx = wx + (x-wx)*(float)((float)i/8.0f) - (spriteHeight/2);
				short sy = wy + (y-wy)*(float)((float)i/8.0f) - (spriteHeight/2);
				
				// on even frames we draw dark gray
				if(i%2==0)
				{
					ClipSprite8_AND_R(sx, sy, spriteHeight, spritePtrs[(short)scaleIdex]+spriteHeight, lightPlane);
					ClipSprite8_OR_R(sx, sy, spriteHeight, spritePtrs[(short)scaleIdex], darkPlane);
				}else
				{
					ClipSprite8_OR_R(sx, sy, spriteHeight, spritePtrs[(short)scaleIdex], lightPlane);
					ClipSprite8_AND_R(sx, sy, spriteHeight, spritePtrs[(short)scaleIdex]+spriteHeight, darkPlane);
				}
				
			
				// draw sprite based on size
			}// next i
		
		}// end if has charge
		
		// draw the crosshair last so they're always on top of the charge
		ClipSprite8_OR_R(x-4, y-4, 8, spr_CrossHair, lightPlane);
		ClipSprite8_OR_R(x-4, y-4, 8, spr_CrossHair, darkPlane);
		
	}// end if uses aim or charge
	

	// if the current weapon is -1, 
	// if the weapon needs to draw itself being held, do that now
	if(Game_currentWeaponProperties & holdsSelf)
	{
		// for guns, they get consumed on use, but we still want to draw them because they can aim while firing
		const short weaponToDraw = (Game_currentWeaponSelected==-1) ? Game_lastWeaponSelected : Game_currentWeaponSelected;

		const unsigned short* heldSprite = facingLeft ? spr_weapons_flipped[(short)weaponToDraw] : spr_weapons[(short)weaponToDraw];
		short wx = facingLeft ? wormX-14 : wormX+2;
		short wy = wormY-5;
		worldToScreen(&wx, &wy);
		
		ClipSprite16_OR_R(wx, wy, 11, heldSprite, lightPlane);
		ClipSprite16_OR_R(wx, wy, 11, heldSprite, darkPlane);
					
	}else if(Game_currentWeaponProperties & holdsLauncher)
	{

		short wx = facingLeft ? wormX-2 : wormX-2;
		short wy = wormY-1;
		worldToScreen(&wx, &wy);
		
		GrayClipSprite8_AND_R(wx, wy, 5, spr_launcher, spr_launcher, lightPlane, darkPlane);
		GrayClipSprite8_OR_R(wx, wy, 5, spr_launcher, spr_launcher, lightPlane, darkPlane);
					
	}else if((Game_currentWeaponProperties & usesCursor) && (Game_currentWeaponState & targetPicked)==0)
	{

		short wx = facingLeft ? wormX-2 : wormX-2;
		short wy = wormY-2;
		worldToScreen(&wx, &wy);
		
		// CORRECT: 6 arguments
		GrayClipSprite8_AND_R(wx, wy, 8, spr_remoteMask, spr_remoteMask, lightPlane, darkPlane);
		GrayClipSprite8_OR_R(wx, wy, 8, spr_remote, spr_remote, lightPlane, darkPlane);
					
	}
}

