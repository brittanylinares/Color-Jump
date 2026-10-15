/*
 * Application.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef APP_COLORJUMP_H_
#define APP_COLORJUMP_H_
#define NUM_TEST_OPTIONS 4
#define VELOCITY -5
#define GRAVITY 1
#define FLOOR 100
#define PLAYER_HALF 2
#define PLAYER_RADIUS 2
#define FLOOR_OF_FLOOR 105
#define NUM_OF_COLORS 5
#include <HAL/HAL.h>
#include <HAL/Timer.h>
#define TITLE_SCREEN_WAIT   3000  //3 seconds
#define RESULT_SCREEN_WAIT  3000  // 3 seconds
#define ROW 28
#define COL 128
#define HALF_SQUARE 5
#define SQUARE 10
#define HIGH_SCORES 5


enum _Difficulty
{
    EASY = 0, HARD = 1
};

typedef enum _Difficulty Difficulty;

enum _GameState
{
    TITLE_SCREEN, GAME_SCREEN, INSTRUCTIONS_SCREEN, OPTIONS_SCREEN, HIGHSCORES_SCREEN, MENU_SCREEN, GAME_OVER_SCREEN
};

typedef enum _GameState GameState;


enum _Cursor
{
    CURSOR_0 = 0, CURSOR_1 = 1, CURSOR_2 = 2, CURSOR_3 = 3 ,    CURSOR_4 = 4,
};
typedef enum _Cursor Cursor;

enum _JumpState
{
    MOVE_UP, MOVE_DOWN, MOVE_NONE, MOVE_RIGHT, MOVE_LEFT
};
typedef enum _JumpState JumpState;


typedef struct _ColorWheel ColorWheel;
struct _ColorWheel
{
    uint32_t center;
     uint32_t up;
     uint32_t down;
     uint32_t right;
     uint32_t left;

};

typedef struct _Player Player;
struct _Player {
    int positionX;
    int positionY;
    int velocityY;
    int oldPositionY;
    JumpState jumpState;
    int jumpStep;
    uint32_t color;
    bool Jump;

};


typedef struct _App_ColorJump App_ColorJump;


struct _App_ColorJump {
  // Put your application members and FSM state variables here!
  // =========================================================================

  GameState state;
  SWTimer timer;
  UART_Baudrate baudChoice;
  Cursor cursor;
  uint32_t moves;
  Player player;
  ColorWheel colorWheel;
  int wheelX;
  int wheelY;
  Graphics_Context gfx;
  int pixels;
  int floorColumns[COL];
  int currentFloorColor;
  int newFloorColor;
  bool changingColors;
  uint32_t floor;
  uint32_t highScores[HIGH_SCORES];
  uint32_t numScores;
  uint32_t score;
  SWTimer scoreTimerSec;
  SWTimer floorSpeed;
  bool firstFrame;
    Difficulty difficulty;

};

// Boilerplate constructor function for the Application

// Called only a single time - inside of main(), where the application is
// constructed
App_ColorJump

App_ColorJump_construct(HAL* hal_p);
void App_ColorJump_loop(App_ColorJump* app, HAL* hal);

void App_ColorJmp_handleTitleScreen(App_ColorJump* app_p, HAL* hal_p);
void App_ColorJump_showMenuScreen(App_ColorJump* app_p, GFX* gfx_p);
void App_ColorJump_updateMenuScreen(App_ColorJump* app_p, GFX *gfx_p);


void App_ColorJump_updateInstructionsScreen(App_ColorJump *app_p, GFX *gfx_p);
void App_ColorJump_handleInstructionsScreen(App_ColorJump *app_p, HAL *hal_p);

void App_ColorJump_handleOptionsScreen(App_ColorJump *app_p, HAL *hal_p);
void App_ColorJump_updateOptionsScreen(App_ColorJump *app_p, GFX *gfx_p);


void App_ColorJump_updateHighScoresScreen(App_ColorJump *app_p, GFX *gfx_p);
void App_ColorJump_handleHighScoresScreen(App_ColorJump *app_p, HAL *hal_p);

void App_ColorJump_updateGameScreen(App_ColorJump *app_p, GFX *gfx_p);
void App_ColorJump_handleGameScreen(App_ColorJump *app_p, HAL *hal_p);


void App_ColorJump_updatePlayer(Player* player,GFX *gfx_p );


void App_ColorJump_updateGameOverScreen(App_ColorJump *app_p, GFX *gfx_p);

void App_ColorWheel_drawGameScreen(ColorWheel *wheel, Graphics_Context *gfx_p, int x, int y);
void swapColors(uint32_t *a, uint32_t *b);



void App_ColorJump_updatePlayer(Player* player,GFX *gfx_p );
void App_ColorJump_resetGame(App_ColorJump *app_p);

void App_ColorJump_showHighScoresScreen(App_ColorJump *app_p, GFX *gfx_p);

void App_ColorJump_updateHighScores(App_ColorJump *app, uint32_t finalScore);

void App_ColorJump_updateOptionsScreen(App_ColorJump *app_p, GFX *gfx_p);
void App_ColorJump_showGameOverScreen(App_ColorJump *app_p, GFX *gfx_p);
void App_ColorJump_handleGameOverScreen(App_ColorJump *app_p, HAL *hal_p);
void CheckAndHandleFloorCollision(App_ColorJump *app_p);

void App_ColorJump_displayHighScores(App_ColorJump *app, GFX *gfx_p);

void App_ColorJump_resetGamePlay(App_ColorJump *app_p, HAL *hal_p);
#endif /* APP_COLORJUMP_H_ */
