/*
	Credits.c
	---------
	
	This file is a snippet that is included raw in State.c
	
	This handles the Credits state machine specific code.
*/


// scroll position counter
short creditsScrollPos = 0;

// credits raw text
const char *creditsText[] = {
	"Idea Inspiration From:",
	"Jean-Christophe Budin",
	"",
	"Art & Engineering:",
	"Greg Miller",
	"",
	"Special Thanks to",
	"Lionel Debroux",
	"Without whom this project",
	"would not have been possible.",
	"",
	"Thanks to",
	"Brandon Lemesh",
	"I'll always remember",
	"playing Worms with you",
	"every morning.",
	"",
	"Worms IP is",
	"(C)Team17/Everyplay",
	"",
	"This is a fan game,",
	"for the fans.",
	"I thank Team17 for",
	"understanding in",
	"advance.",
	"",
	"Dedicated to my Mom",
	"I miss you.",
	"I love you.",
	"",
	"https://gmiller.net",
	"2026"
};

// layout defines
#define CREDITS_LINE_HEIGHT 8
#define CREDITS_START_Y 100
#define CREDITS_COUNT (sizeof(creditsText) / sizeof(creditsText[0]))


/**
 * @brief Draws the actual credits text
 */
void drawCreditsState()
{
	FontSetSys(F_4x6);
	
	// draw only to dark plane to be lightweight (shows as dark gray)
	PortSet(darkPlane, 239, 127);

	short i;
	// calculate y position (speed: divided by 2 to slow it down)
	short y = CREDITS_START_Y - (creditsScrollPos/2);
	
	for(i=0; i<(short)CREDITS_COUNT; i++)
	{
		// if line is empty, add extra space (reduces simultaneous draw calls)
		if (creditsText[i][0] == '\0') {

			y += CREDITS_LINE_HEIGHT * 6;

		} else {
			
			// clip text at y=16 to avoid overlapping title
			if(y >= 16 && y < 100) {
				short w = (short)strlen(creditsText[i]) * 4;
				DrawStr((160 - w) / 2, y, creditsText[i], A_NORMAL);
			}
			y += CREDITS_LINE_HEIGHT;
		}
	}
	
	// reset if we've scrolled past the last line
	if (y < 16) 
		creditsScrollPos = 0;

	PortRestore();
}


/**
 * main drawing routine for the Credits menu
 */
void Draw_renderCreditsRoll()
{
	// start fresh
	Draw_clearBuffers();

	// draw title, with big font and shadow just on light plane
	Draw_titleText("Credits");

	// draw the x close & check accept buttons
	Draw_XandCheck(BTN_ACCEPT);

	// draw the actual credits text
	drawCreditsState();
}


/**
	Called on the first-frame when the Games state machine is set to Credits mode.
*/
static void Credits_enter()
{
	// reset scroll position
	creditsScrollPos = 0;
}


/**
	Called every frame that the Games state machine is in Credits mode.
*/
static void Credits_update()
{
	// update pos
	creditsScrollPos++;

	// draw the credits
	Draw_renderCreditsRoll();

	// this menu only has accept, so F5 returns to MatchMenu
	if(Keys_keyUp(keyAction|keyF1|keyF5|keyEscape))
		State_changeMode(menuMode_MainMenu);
}


/**
	Called on the first-frame when the Games state machine leaves Credits mode.
*/
static void Credits_exit()
{

}
