/*
	GameOver.c
	----------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the GameOver state machine specific code.
*/


// true if it's a draw
static char isDraw = FALSE;

// which team one
static char winningTeam = -1;

// we'll precompute once how many worms are left on each team
static char team1WormsLeft = 0;
static char team2WormsLeft = 0;

// pre-compute the strings to print
static char winnerBuffer[32];
static char loserBuffer[32];

// precalculate their x position and length length for centering calculations
static char winXPos = 0;
static char loseYPos = 0;

// animation timer to make the winners jump
static char animationTimer = 0;


/**
 * @brief Draws a stack of worms for the pause menu worm counter
 * 
 * @param yPos - y position to draw at
 * @param teamIndex - which team to draw worms for (0 or 1)
 */
void drawGameOverTeam(short yPos, short teamIndex)
{
    // get live worms for this team, if any
    const short liveCount = (teamIndex==0) ? team1WormsLeft : team2WormsLeft;

    if(liveCount == 0)
        return;

    const short isWinningTeam = (winningTeam == teamIndex);

    // Initial X position
    short xPos = 80 - (liveCount * 5);
    short i;

    for(i = 0; i < liveCount; i++)
    {
        // 1. DEFAULT STATE (Standing)
        // Point to the standard "standing" sprite data by default
        const void* currentMask = spr_WormRight_Mask;
        const void* currentOutline = spr_WormRight_Outline;
        short currentHeight = 13;
        short currentY = yPos;

        // 2. JUMP LOGIC
        if(isWinningTeam)
        {
            // We create a 'local' timer for this specific worm.
            // (i * 7) offsets each worm so they don't jump in a straight wave.
            // & 31 creates a repeating 32-frame cycle (0 to 31).
            short cycleTime = (animationTimer + (i * 7)) & 31;

            // If we are in the first 3 frames of this worm's cycle, make it jump.
            // Adjust '3' to '5' if you want them to stay in the air longer.
            if(cycleTime < 6)
            {
                currentMask = spr_WormFlip1_Right_Mask;
                currentOutline = spr_WormFlip1_Right_Outline;
                currentHeight = 17;
                
                // Move the sprite UP by 4 pixels to simulate the jump height
                currentY -= (cycleTime>1 && cycleTime<5) ? 6 : 4; 
            }
        }

        // 3. DRAWING
        // We call the draw routines once, using the pointers we set above.
        // This is much more code-efficient than an if/else block duplicating these calls.
        ClipSprite16_AND_R(xPos, currentY, currentHeight, currentMask, darkPlane);
        ClipSprite16_AND_R(xPos, currentY, currentHeight, currentMask, lightPlane);
        ClipSprite16_OR_R(xPos, currentY, currentHeight, currentOutline, darkPlane);
        ClipSprite16_OR_R(xPos, currentY, currentHeight, currentOutline, lightPlane);

        xPos += 10;
    }
}


/**
 * Draws the game over screen
 */
void drawGameOver()
{
	// draw the game as normal..
	Draw_renderGame();
	
	// make it darker
	FastFillRect(darkPlane, 0, 0, 159, 99, A_NORMAL);

	// draw title
	FontSetSys(F_8x10);
	
	// dark plane is solid, so we can XOR the shadow area to make it WHITE on the dark plane
	GrayDrawStr2B(40, 2, "Game Over!", A_XOR, darkPlane, darkPlane);
	
	// draw shadow JUST on light plane
	GrayDrawStr2B(40, 2, "Game Over!", A_NORMAL, lightPlane, lightPlane);
	
	// draw it to both planes in black, then XOR it to make it WHITE
	GrayDrawStr2B(39, 1, "Game Over!", A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(39, 1, "Game Over!", A_XOR, lightPlane, darkPlane);

	// use small font
	FontSetSys(F_4x6);

	// draw winner text
	GrayDrawStr2B(winXPos, 20, winnerBuffer, A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(winXPos, 20, winnerBuffer, A_XOR, lightPlane, darkPlane);

	// draw winning worms, if any
	drawGameOverTeam(35, winningTeam);

	// draw loser text
	GrayDrawStr2B(loseYPos, 60, loserBuffer, A_NORMAL, lightPlane, darkPlane);
	GrayDrawStr2B(loseYPos, 60, loserBuffer, A_XOR, lightPlane, darkPlane);

	// draw losing worms, if any
	drawGameOverTeam(70, (winningTeam==0) ? 1 : 0);
}


/**
	Called on the first-frame when the Games state machine is set to GameOver mode.
*/
static void GameOver_enter()
{
	// Game_surrenderedTeam
	// build masks for each team
	const unsigned short team1Mask = 0b0000000011111111;
	const unsigned short team2Mask = 0b1111111100000000;
	
	// get alive worms for each team
	unsigned short team1Alive = Worm_active & ~Worm_isDead & team1Mask;
	unsigned short team2Alive = Worm_active & ~Worm_isDead & team2Mask;

	// get how many worms are alive for this team

	// count how many bits are set in liveWorms
	team1WormsLeft = 0;
	short i;
	for(i=0; i<MAX_WORMS; i++){
		if((team1Alive & (1<<i))!=0)
			team1WormsLeft++;
		if((team2Alive & (1<<i))!=0)
			team2WormsLeft++;
	}

	// if it's a draw, set the draw flag & we're done
	if((team1Alive==0) && (team2Alive==0))
	{
		isDraw = TRUE;
		winningTeam = -1;

		sprintf(winnerBuffer, "It's a Draw!");
		sprintf(loserBuffer, "Both teams eliminated!");
	}
	// if a team surrendered, the other team wins
	else if(Game_surrenderedTeam!=-1)
	{
		winningTeam = (Game_surrenderedTeam==1) ? 0 : 1;
		sprintf(winnerBuffer, "%s Wins!", Match_teamNames[(short)(winningTeam)]);
		sprintf(loserBuffer, "%s surrendered!", Match_teamNames[(short)(Game_surrenderedTeam)]);
	}
	else
	{
		// if one team is dead, the other team wins
		winningTeam = (team1Alive==0) ? 1 : 0;		
		sprintf(winnerBuffer, "%s Wins!", Match_teamNames[(short)(winningTeam)]);
		sprintf(loserBuffer, "%s has been eliminated!", Match_teamNames[(short)((winningTeam==1) ? 0 : 1)]);
	}

	// calculate positions to center the text
	winXPos = 80 - ((strlen(winnerBuffer) * 4) >> 1);
	loseYPos = 80 - ((strlen(loserBuffer) * 4) >> 1);

	// reset timer
	animationTimer = 0;
}


/**
	Called every frame that the Games state machine is in GameOver mode.
*/
static void GameOver_update()
{
	// increment animation timer
	animationTimer++;

	// the game
	drawGameOver();

	// if the user presses the action key, we should either quit the game, or exit the pause menu
	if(Keys_keyDown(keyF5))
	{
		// setting this will exit the next tick of our main loop
		GameRunning = FALSE;
		return;		
	}
}


/**
	Called on the first-frame when the Games state machine leaves GameOver mode.
*/
static void GameOver_exit()
{
	
}
