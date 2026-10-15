/**
 * Starter code for Project 2. Good luck!
 *
 * We recommending copy/pasting your HAL folder from Project 1
 * into this project.
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
/* HAL and Application includes */
#include <App_ColorJump.h>
#include <HAL/HAL.h>
#include <HAL/Timer.h>

extern const Graphics_Image colors8BPP_UNCOMP;
extern const Graphics_Image sad8BPP_UNCOMP;
extern const Graphics_Image happy8BPP_UNCOMP;

// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void InitNonBlockingLED()
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}

// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void PollNonBlockingLED()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == 0)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}

int main()
{
    WDT_A_holdTimer();

    // Do not remove this line. This is your non-blocking check.
    InitNonBlockingLED();

    //Initialize the system clock and background hardware timer, used to enable
    // software timers to time their measurements properly.
    InitSystemTiming();
    HAL hal = HAL_construct();
    int lastState = -1;
    App_ColorJump app = App_ColorJump_construct(&hal);
    App_ColorJump_showTitleScreen(&hal.gfx);
    while (true)
    {

        HAL_refresh(&hal);
        App_ColorJump_loop(&app, &hal);
    }
}

/*
 * This function constructs and initializes an App_ColorJump object
 * It sets the initial game state, timers, joystick cursor, player parameters,
 * color wheel colors, floor colors, and high score array
 */
App_ColorJump App_ColorJump_construct(HAL *hal_p)
{
    App_ColorJump app;

    app.state = TITLE_SCREEN;
    app.timer = SWTimer_construct(TITLE_SCREEN_WAIT);
    SWTimer_start(&app.timer);
    app.cursor = CURSOR_0;
    app.player.positionX = 20;
    app.player.positionY = FLOOR;
    app.player.Jump = false;
    app.colorWheel.center = GRAPHICS_COLOR_WHITE;
    app.colorWheel.up = GRAPHICS_COLOR_YELLOW;
    app.colorWheel.right = GRAPHICS_COLOR_CYAN;
    app.colorWheel.down = GRAPHICS_COLOR_RED;
    app.colorWheel.left = GRAPHICS_COLOR_GREEN;
    app.wheelX = 64;
    app.wheelY = 64;
    app.player.color = app.colorWheel.center;
    //uint32_t floor = app.colorWheel.center;
    app.score = 0;
    app.scoreTimerSec = SWTimer_construct(1000);
    SWTimer_start(&app.scoreTimerSec);
    app.currentFloorColor = GRAPHICS_COLOR_WHITE;
    app.newFloorColor = GRAPHICS_COLOR_WHITE;
    app.changingColors = false;
    app.firstFrame = true;
    app.difficulty = EASY;
    int i;
    App_ColorJump_NewColor(&app);
    for (i = 0; i < COL; i++)
    {
        if (i < COL / 2)
            app.floorColumns[i] = app.currentFloorColor;
        else
            app.floorColumns[i] = app.newFloorColor;
    }
    for (i = 0; i < HIGH_SCORES; i++)
    {
        app.highScores[i] = 0;
    }

    return app;
}

/*
 * This is the main game loop
 * It checks various inputs buttons/joysticks and dispatches the
 * execution to the proper handler function depending on the current state
 */

void App_ColorJump_loop(App_ColorJump *app_p, HAL *hal_p)
{

    if (Button_isPressed(&hal_p->boosterpackS1))
        LED_turnOn(&hal_p->boosterpackRed);
    else
        LED_turnOff(&hal_p->boosterpackRed);

    if (Joystick_isPressedToLeft(&hal_p->joystick))
        LED_turnOn(&hal_p->boosterpackGreen);
    else
        LED_turnOff(&hal_p->boosterpackGreen);

    if (Joystick_isTappedToLeft(&hal_p->joystick))
        LED_toggle(&hal_p->boosterpackBlue);

    switch (app_p->state)
    {
    case TITLE_SCREEN:
        App_ColorJump_handleTitleScreen(app_p, hal_p);
        break;

    case INSTRUCTIONS_SCREEN:
        App_ColorJump_handleInstructionsScreen(app_p, hal_p);
        break;

    case GAME_SCREEN:
        App_ColorJump_handleGameScreen(app_p, hal_p);
        break;

    case OPTIONS_SCREEN:
        App_ColorJump_handleOptionsScreen(app_p, hal_p);
        break;
    case HIGHSCORES_SCREEN:
        App_ColorJump_handleHighScoresScreen(app_p, hal_p);
        break;
    case MENU_SCREEN:
        App_ColorJump_handleMenuScreen(app_p, hal_p);
        break;
    case GAME_OVER_SCREEN:
        App_ColorJump_handleGameOverScreen(app_p, hal_p);
        break;
    default:
        break;
    }
}
/*
 This functions handles the Title Screen
 It checks a timer to see if the title screen should be wiped and we should move to menu
 */
void App_ColorJump_handleTitleScreen(App_ColorJump *app_p, HAL *hal_p)
{
    if (SWTimer_expired(&app_p->timer))
    {
        app_p->state = MENU_SCREEN;
        App_ColorJump_showMenuScreen(app_p, &hal_p->gfx);
    }
}

/*
 * Displays the main menu screen
 */
void App_ColorJump_showMenuScreen(App_ColorJump *app_p, GFX *gfx_p)
{
    GFX_clear(gfx_p);
    GFX_print(gfx_p, "Play Game", 2, 1);
    GFX_print(gfx_p, "Instructions", 3, 1);
    GFX_print(gfx_p, "High Scores", 4, 1);
    GFX_print(gfx_p, "Options", 5, 1);
    GFX_print(gfx_p, "Press BB1", 7, 1);

}

/*
 * Displays the title screen
 */
void App_ColorJump_showTitleScreen(GFX *gfx_p)
{

    GFX_clear(gfx_p);
    GFX_print(gfx_p, "Color Jump  ", 0, 0);
    GFX_print(gfx_p, "---------------------", 1, 0);
    GFX_print(gfx_p, "By: Brittany Linares", 2, 0);
}

/*
 * Handles menu navigation using the joystick
 */
void App_ColorJump_handleMenuScreen(App_ColorJump *app_p, HAL *hal_p)
{
    if (Joystick_isTappedToUp(&hal_p->joystick))
    {
        app_p->cursor = (Cursor) (((int) app_p->cursor - 1 + NUM_TEST_OPTIONS)
                % NUM_TEST_OPTIONS);
        App_ColorJump_updateMenuScreen(app_p, &hal_p->gfx);
    }
    if (Joystick_isTappedToDown(&hal_p->joystick))
    {
        app_p->cursor = (Cursor) (((int) app_p->cursor + 1) % NUM_TEST_OPTIONS);
        App_ColorJump_updateMenuScreen(app_p, &hal_p->gfx);
    }

    if (Button_isTapped(&hal_p->boosterpackS1))
    {
        switch (app_p->cursor)
        {

        case CURSOR_0:
            App_ColorJump_resetGamePlay(app_p, hal_p);
            break;
        case CURSOR_1:
            app_p->state = INSTRUCTIONS_SCREEN;
            break;

        case CURSOR_2:
            app_p->state = HIGHSCORES_SCREEN;
            break;
        case CURSOR_3:
            app_p->state = OPTIONS_SCREEN;
            break;
        case CURSOR_4:
            break;
        default:
            break;
        }
    }
}

/*
 * Redraws the menu cursor by printing a ">" next to the selected option
 */
void App_ColorJump_updateMenuScreen(App_ColorJump *app_p, GFX *gfx_p)
{
    GFX_print(gfx_p, " ", 2, 0);
    GFX_print(gfx_p, " ", 3, 0);
    GFX_print(gfx_p, " ", 4, 0);
    GFX_print(gfx_p, " ", 5, 0);
    GFX_print(gfx_p, " ", 6, 0);

    switch (app_p->cursor)
    {
    case 0:

        GFX_print(gfx_p, ">", 2, 0);
        break;
    case 1:
        GFX_print(gfx_p, ">", 3, 0);
        break;
    case 2:
        GFX_print(gfx_p, ">", 4, 0);
        break;
    case 3:
        GFX_print(gfx_p, ">", 5, 0);
        break;
    case 4:
        GFX_print(gfx_p, ">", 6, 0);
        break;
    }
}
/*
 * Updates the instructions screen with control information
 */

void App_ColorJump_updateInstructionsScreen(App_ColorJump *app_p, GFX *gfx_p)
{
    GFX_clear(gfx_p);
    GFX_print(gfx_p, "Instructions", 0, 0);
    GFX_print(gfx_p, "Use the joystick", 1, 0);
    GFX_print(gfx_p, "to change color", 2, 0);
    GFX_print(gfx_p, "Press BB1 to jump.", 3, 0);
    GFX_print(gfx_p, "Match the color of", 4, 0);
    GFX_print(gfx_p, "the floor to keep", 5, 0);
    GFX_print(gfx_p, "running", 6, 0);
    GFX_print(gfx_p, "Press BB1 to return", 15, 0);

}


/*
 * Handles the instructions screen. Displays instructions once until
 * the player taps a button to return to the menu
 */
void App_ColorJump_handleInstructionsScreen(App_ColorJump *app_p, HAL *hal_p)
{
    static bool InstructionsScreen = false;
    if (!InstructionsScreen)
    {
        App_ColorJump_updateInstructionsScreen(app_p, &hal_p->gfx);
        InstructionsScreen = true;
    }

    if (Button_isTapped(&hal_p->boosterpackS1))
    {
        app_p->state = MENU_SCREEN;
        App_ColorJump_showMenuScreen(app_p, &hal_p->gfx);

    }
}
/*
 * Handles the options screen, easy and hard **
 */
void App_ColorJump_handleOptionsScreen(App_ColorJump *app_p, HAL *hal_p)
{
    static bool OptionsScreen = false;
    if (!OptionsScreen)
    {
        App_ColorJump_updateOptionsScreen(app_p, &hal_p->gfx);
        OptionsScreen = true;
    }
    if (Button_isTapped(&hal_p->boosterpackS2))
    {
        if (app_p->difficulty == EASY)
        {
            app_p->difficulty = HARD;
        }
        else
        {
            app_p->difficulty = EASY;
        }
        App_ColorJump_updateOptionsScreen(app_p, &hal_p->gfx);
    }

    if (Button_isTapped(&hal_p->boosterpackS1))
    {
        OptionsScreen = false;
        app_p->state = MENU_SCREEN;
        App_ColorJump_showMenuScreen(app_p, &hal_p->gfx);
    }
}

/*
 * Updates the high scores screen by calling the display function
 */
void App_ColorJump_updateHighScoresScreen(App_ColorJump *app_p, GFX *gfx_p)
{
    App_ColorJump_displayHighScores(app_p, gfx_p);
}

/*
 * Handles the high scores screen
 */
void App_ColorJump_handleHighScoresScreen(App_ColorJump *app_p, HAL *hal_p)
{
    static bool HighScoresScreen = false;
    if (!HighScoresScreen)
    {
        App_ColorJump_updateHighScoresScreen(app_p, &hal_p->gfx);
        HighScoresScreen = true;
    }
    if (Button_isTapped(&hal_p->boosterpackS1))
    {
        HighScoresScreen = false;
        app_p->state = MENU_SCREEN;
        App_ColorJump_showMenuScreen(app_p, &hal_p->gfx);
    }
}

/*
 * Handles swapping the colors in the color wheel when the joystick is tapped
 */
void App_ColorJump_handleJoystickSwap(App_ColorJump *app_p, HAL *hal_p)
{
    if (Joystick_isTappedToUp(&hal_p->joystick))
    {
        swap(&app_p->colorWheel.center, &app_p->colorWheel.up);
    }
    else if (Joystick_isTappedToDown(&hal_p->joystick))
    {
        swap(&app_p->colorWheel.center, &app_p->colorWheel.down);
    }
    else if (Joystick_isTappedToLeft(&hal_p->joystick))
    {
        swap(&app_p->colorWheel.center, &app_p->colorWheel.left);
    }
    else if (Joystick_isTappedToRight(&hal_p->joystick))
    {
        swap(&app_p->colorWheel.center, &app_p->colorWheel.right);
    }
}

/*
 * Handles the score counter, updating it based on the difficulty,
 * and displays the current score on the screen
 */
void App_ColorJump_handleScoreCounter(App_ColorJump *app_p, HAL *hal_p)
{
    uint32_t increment;
    if (app_p->difficulty == EASY)
    {
        increment = 1;
    }
    else
    {
        increment = 3;
    }

    if (SWTimer_expired(&app_p->scoreTimerSec))
    {
        app_p->score += increment;
        SWTimer_start(&app_p->scoreTimerSec);
    }
    char scorelength[16];
    sprintf(scorelength, "Score: %lu", app_p->score);
    GFX_print(&hal_p->gfx, scorelength, 0, 0);
}

/*
 * Handles the main game screen, Clears screen on first frame,
 * Updates the floor, joystick swaps, score counter, and redraws the game elements
 */
void App_ColorJump_handleGameScreen(App_ColorJump *app_p, HAL *hal_p)
{

    if (app_p->firstFrame)
    {
        GFX_clear(&hal_p->gfx);
        app_p->firstFrame = false;
    }
    App_ColorJump_DrawAndUpdateFloor(app_p, hal_p);
    App_ColorJump_handleJoystickSwap(app_p, hal_p);
    App_ColorJump_handleScoreCounter(app_p, hal_p);
    int wheelCenterX = 110;
    int wheelCenterY = 15;
    App_ColorJump_handleScoreCounter(app_p, hal_p);
    App_ColorWheel_drawGameScreen(&app_p->colorWheel, &hal_p->gfx.context,
                                  wheelCenterX, wheelCenterY);
    app_p->player.color = app_p->colorWheel.center;
    Player *player = &app_p->player;

    Graphics_Rectangle clearRectangle;
    clearRectangle.xMin = app_p->player.positionX - PLAYER_RADIUS * 2;
    clearRectangle.yMin = app_p->player.positionY - PLAYER_RADIUS * 2;
    clearRectangle.xMax = app_p->player.positionX + PLAYER_RADIUS * 2;
    clearRectangle.yMax = app_p->player.positionY + PLAYER_RADIUS * 2;

    Graphics_setForegroundColor(&hal_p->gfx.context, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangle(&hal_p->gfx.context, &clearRectangle);
    Graphics_setForegroundColor(&hal_p->gfx.context, app_p->player.color);
    Graphics_fillCircle(&hal_p->gfx.context, app_p->player.positionX, app_p->player.positionY, 2);

    if (Button_isTapped(&hal_p->boosterpackS1) && !player->Jump)
    {
        player->Jump = true;
        player->velocityY = VELOCITY;
    }
    App_ColorJump_updatePlayer(player, &hal_p->gfx);
    CheckAndHandleFloorCollision(app_p);
}

/*
 * Updates the player's position.
 */

void App_ColorJump_updatePlayer(Player *player, GFX *gfx_p)
{
    static int previousY = -1;
    static int previousX = -1;
    if (player->Jump)

    previousX = player->positionX;
    previousY = player->positionY;
    player->positionY += player->velocityY;
    player->velocityY += GRAVITY;
    Graphics_Rectangle prevRect;
    prevRect.xMin = previousX - PLAYER_HALF;
    prevRect.yMin = previousY - PLAYER_HALF;
    prevRect.xMax = previousX + PLAYER_HALF;
    prevRect.yMax = previousY + PLAYER_HALF;

    Graphics_Rectangle currRect;
    currRect.xMin = player->positionX - PLAYER_HALF;
    currRect.yMin = player->positionY - PLAYER_HALF;
    currRect.xMax = player->positionX + PLAYER_HALF;
    currRect.yMax = player->positionY + PLAYER_HALF;
    Graphics_setForegroundColor(&gfx_p->context, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangle(&gfx_p->context, &prevRect);
    Graphics_setForegroundColor(&gfx_p->context, player->color);
    Graphics_fillRectangle(&gfx_p->context, &currRect);
}
/*
 * Swaps two colors by exchanging the values at the provided pointers
 */
void swap(uint32_t *center, uint32_t *change)
{
    uint32_t temporary = *center;
    *center = *change;
    *change = temporary;
}

/*
 * Draws the color wheel on the game screen.
 */

void App_ColorWheel_drawGameScreen(ColorWheel *wheel, Graphics_Context *gfx_p,
                                   int centerX, int centerY)
{
    Graphics_Rectangle rectangle;

    Graphics_setForegroundColor(gfx_p, wheel->center);
    rectangle.xMin = centerX - HALF_SQUARE;
    rectangle.yMin = centerY - HALF_SQUARE;
    rectangle.xMax = centerX + HALF_SQUARE;
    rectangle.yMax = centerY + HALF_SQUARE;
    Graphics_fillRectangle(gfx_p, &rectangle);

    //up
    Graphics_setForegroundColor(gfx_p, wheel->up);
    rectangle.xMin = centerX - HALF_SQUARE;
    rectangle.yMin = centerY - SQUARE - HALF_SQUARE;
    rectangle.xMax = rectangle.xMin + SQUARE;
    rectangle.yMax = rectangle.yMin + SQUARE;
    Graphics_fillRectangle(gfx_p, &rectangle);

    //down
    Graphics_setForegroundColor(gfx_p, wheel->down);
    rectangle.xMin = centerX - HALF_SQUARE;
    rectangle.yMin = centerY + SQUARE - HALF_SQUARE;
    rectangle.xMax = rectangle.xMin + SQUARE;
    rectangle.yMax = rectangle.yMin + SQUARE;
    Graphics_fillRectangle(gfx_p, &rectangle);

    //left
    Graphics_setForegroundColor(gfx_p, wheel->left);
    rectangle.xMin = centerX - SQUARE - HALF_SQUARE;
    rectangle.yMin = centerY - HALF_SQUARE;
    rectangle.xMax = rectangle.xMin + SQUARE;
    rectangle.yMax = rectangle.yMin + SQUARE;
    Graphics_fillRectangle(gfx_p, &rectangle);

    //right
    Graphics_setForegroundColor(gfx_p, wheel->right);
    rectangle.xMin = centerX + SQUARE - HALF_SQUARE;
    rectangle.yMin = centerY - HALF_SQUARE;
    rectangle.xMax = rectangle.xMin + SQUARE;
    rectangle.yMax = rectangle.yMin + SQUARE;
    Graphics_fillRectangle(gfx_p, &rectangle);
}

/*
 * Draws and updates the floor, based on the game difficulty, shifts the floor columns more quickly
 */
void App_ColorJump_DrawAndUpdateFloor(App_ColorJump *app_p, HAL *hal_p)
{
    int speed;
    if (app_p->difficulty == HARD)
    {
        speed = 5;
    }
    else
    {
        speed = 3;
    }
    if (app_p->changingColors)
    {
        int shift;
        for (shift = 0; shift < speed; shift++)
        {
            int i;
            for (i = 0; i < COL - 1; i++)
            {
                app_p->floorColumns[i] = app_p->floorColumns[i + 1];
            }

            app_p->floorColumns[COL - 1] = app_p->newFloorColor;
        }
        int complete = 1;
        int i;
        for (i = 0; i < COL; i++)
        {
            if (app_p->floorColumns[i] != app_p->newFloorColor)
            {
                complete = 0;
                break;
            }
        }
        if (complete)
        {
            app_p->currentFloorColor = app_p->newFloorColor;
            App_ColorJump_NewColor(app_p);
        }
    }
    int x;
    for (x = 0; x < COL; x++)
    {
        Graphics_Rectangle colRectangle;
        colRectangle.xMin = x;
        colRectangle.xMax = x + 1;
        colRectangle.yMin = FLOOR_OF_FLOOR;
        colRectangle.yMax = FLOOR_OF_FLOOR + ROW;
        Graphics_setForegroundColor(&hal_p->gfx.context,app_p->floorColumns[x]);
        Graphics_fillRectangle(&hal_p->gfx.context, &colRectangle);
    }
}

/*
 * Chooses a new random color for the floor
 */
void App_ColorJump_NewColor(App_ColorJump *app_p)
{
    int colors[NUM_OF_COLORS] = { GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_RED,
                                  GRAPHICS_COLOR_CYAN, GRAPHICS_COLOR_YELLOW,
                                  GRAPHICS_COLOR_GREEN };
    int pickRandomColor = rand() % NUM_OF_COLORS;
    app_p->newFloorColor = colors[pickRandomColor];
    app_p->changingColors = true;
}
/*
 * Checks for a collision between the player and the floor
 */
void CheckAndHandleFloorCollision(App_ColorJump *app_p)
{
    int floorTop = FLOOR_OF_FLOOR;
    int playerBottom = app_p->player.positionY + PLAYER_HALF;

    if (playerBottom >= floorTop)
    {

        int col = app_p->player.positionX;
        if (col < 0)
            col = 0;
        if (col >= COL)
            col = COL - 1;

        if (app_p->floorColumns[col] == app_p->player.color)
        {

            app_p->player.positionY = floorTop - PLAYER_HALF;
            app_p->player.velocityY = 0;
            app_p->player.Jump = false;
        }

    }

    if (app_p->player.positionY - PLAYER_HALF > COL)
    {
        app_p->state = GAME_OVER_SCREEN;

    }
}
/*
 * Handles the game over screen
 */

void App_ColorJump_handleGameOverScreen(App_ColorJump *app_p, HAL *hal_p)
{
    static bool GameOverScreen = false;
    if (!GameOverScreen)
    {
        App_ColorJump_updateGameOverScreen(app_p, &hal_p->gfx);
        App_ColorJump_updateHighScores(app_p, app_p->score);
        GameOverScreen = true;
    }

    if (Button_isTapped(&hal_p->boosterpackS1))
    {
        App_ColorJump_resetGamePlay(app_p, hal_p);
        GameOverScreen = false;
        app_p->state = GAME_SCREEN;
    }

    if (Button_isTapped(&hal_p->boosterpackS2))
    {
        GFX_clear(&hal_p->gfx);
        GameOverScreen = false;
        app_p->state = MENU_SCREEN;
        App_ColorJump_showMenuScreen(app_p, &hal_p->gfx);
    }
}

/*
 * Updates the game over screen
 */

void App_ColorJump_updateGameOverScreen(App_ColorJump *app_p, GFX *gfx_p)
{
    GFX_clear(gfx_p);
    GFX_print(gfx_p, "GAME OVERRRRRRRRR",5,1);
    GFX_print(gfx_p, "Press BB1 to Restart", 7, 1);
    GFX_print(gfx_p, "Press BB2 for Menu", 8, 1);
}

/*
 * Updates the high scores list
 */
void App_ColorJump_handleHighScores(App_ColorJump *app, uint32_t score)
{
    int start = -1;
    int i;
    for (i = 0; i < HIGH_SCORES; i++)
    {
        if (score > app->highScores[i])
        {
            start = i;
            break;
        }
    }

    if (start == -1)
        return;
    for (i = HIGH_SCORES - 1; i > start; i--)
    {
        app->highScores[i] = app->highScores[i - 1];
    }
    app->highScores[start] = score;
}
/*
 * Displays the list of high scores on the screen
 */
void App_ColorJump_displayHighScores(App_ColorJump *app, GFX *gfx_p)
{
    char length[20];
    GFX_clear(gfx_p);
    GFX_print(gfx_p, "High Scores", 0, 0);
    int i;
    for (i = 0; i < HIGH_SCORES; i++)
    {
        sprintf(length, "%d. %lu", i + 1, app->highScores[i]);
        GFX_print(gfx_p, length, i + 2, 0);
    }
    GFX_print(gfx_p, "Press BB1 to return", 15, 0);
}


/*
 * Updates the high scores list with the final score after a game session
 */
void App_ColorJump_updateHighScores(App_ColorJump *app, uint32_t finalScore)
{
    int scoreupdate = -1;
    int i;
    for (i = 0; i < HIGH_SCORES; i++)
    {
        if (finalScore > app->highScores[i])
        {
            scoreupdate = i;
            break;
        }
    }

    if (scoreupdate == -1)
        return;

    for (i = HIGH_SCORES - 1; i > scoreupdate; i--)
    {
        app->highScores[i] = app->highScores[i - 1];
    }

    app->highScores[scoreupdate] = finalScore;
}
/*
 * Resets the game state for a new play session ,couldn't figure out how to do this with application loop so its kind of the same
 */
void App_ColorJump_resetGamePlay(App_ColorJump *app_p, HAL *hal_p)
{

    app_p->firstFrame = true;
    app_p->score = 0;
    app_p->scoreTimerSec = SWTimer_construct(1000);
    SWTimer_start(&app_p->scoreTimerSec);

    app_p->player.positionX = 20;
    app_p->player.positionY = FLOOR;
    app_p->player.Jump = false;
    app_p->player.velocityY = 0;

    app_p->colorWheel.center = GRAPHICS_COLOR_WHITE;
    app_p->colorWheel.up = GRAPHICS_COLOR_YELLOW;
    app_p->colorWheel.right = GRAPHICS_COLOR_CYAN;
    app_p->colorWheel.down = GRAPHICS_COLOR_RED;
    app_p->colorWheel.left = GRAPHICS_COLOR_GREEN;

    app_p->currentFloorColor = GRAPHICS_COLOR_WHITE;
    app_p->newFloorColor = GRAPHICS_COLOR_WHITE;
    app_p->changingColors = false;
    App_ColorJump_NewColor(app_p);
    int i;
    for (i = 0; i < COL; i++)
    {
        if (i < COL / 2)
        {
            app_p->floorColumns[i] = app_p->currentFloorColor;
        }
        else
        {
            app_p->floorColumns[i] = app_p->newFloorColor;
        }
    }
    GFX_clear(&hal_p->gfx);
    app_p->state = GAME_SCREEN;
}
/*
 * Updates the options screen with the current difficulty setting
 */
void App_ColorJump_updateOptionsScreen(App_ColorJump *app_p, GFX *gfx_p)
{
    GFX_clear(gfx_p);
    GFX_print(gfx_p, "Difficulty", 0, 4);

    if (app_p->difficulty == EASY)
    {
        GFX_print(gfx_p, "> Easy", 2, 2);
        GFX_print(gfx_p, "  Hard", 3, 2);
    }
    else
    {
        GFX_print(gfx_p, "  Easy", 2, 2);
        GFX_print(gfx_p, "> Hard", 3, 2);
    }

    GFX_print(gfx_p, "BB2: Toggle", 5, 1);
    GFX_print(gfx_p, "BB1: Menu", 6, 1);
}

