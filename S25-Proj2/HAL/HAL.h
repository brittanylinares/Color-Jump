/*
 * HAL.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef HAL_HAL_H_
#define HAL_HAL_H_

#include <HAL/Button.h>
#include <HAL/LED.h>
#include <HAL/Timer.h>
#include <HAL/UART.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <HAL/LcdDriver/Crystalfontz128x128_ST7735.h> //lcd display
#include <ti/grlib/grlib.h>
#include <HAL/Joystick.h>


//graphics defines
#define FG_COLOR GRAPHICS_COLOR_WHITE
#define BG_COLOR GRAPHICS_COLOR_BLACK

/**============================================================================
 * The main HAL struct. This struct encapsulates all of the other input structs
 * in this application as individual members. This includes all LEDs, all
 * Buttons, one HWTimer from which all software timers should reference, the
 * Joystick, and any other peripherals with which you wish to interface.
 * ============================================================================
 * USAGE WARNINGS
 * ============================================================================
 * YOU SHOULD HAVE EXACTLY ONE HAL STRUCT IN YOUR ENTIRE PROJECT. We recommend
 * you put this struct inside of a main [Application] object so that every
 * single function in your application has access to the main inputs and outputs
 * which interface with the hardware on the MSP432.
 */
struct _GFX
{
    Graphics_Context context;
    uint32_t foreground;
    uint32_t background;
    uint32_t defaultForeground;
    uint32_t defaultBackground;
};
typedef struct _GFX GFX;
struct _HAL {
  // LEDs - Left Launchpad LED (the LED is only a single color - red)
  LED launchpadLED1;

  // LEDs - Right Launchpad LEDs (one LED struct for red, one for blue, etc.)
  LED launchpadLED2Red;
  LED launchpadLED2Blue;
  LED launchpadLED2Green;

  // LEDs - Boosterpack LED (one LED struct for red, one for blue, etc.)
  LED boosterpackRed;
  LED boosterpackBlue;
  LED boosterpackGreen;

  // Buttons - Launchpad S1 and S2
  Button launchpadS1;
  Button launchpadS2;

  // Buttons - Boosterpack S1, S2, and JS (press down on the joystick)
  Button boosterpackS1;
  Button boosterpackS2;
  Button boosterpackJS;

  // UART - Construct a new UART instance
  UART uart;
  GFX gfx;

  Joystick joystick;
};
typedef struct _HAL HAL;

GFX GFX_construct(uint32_t defaultForeground, uint32_t defaultBackground);
void GFX_resetColors(GFX* gfx_p);
void GFX_clear(GFX* gfx_p);
void GFX_print(GFX* gfx_p, char* string, int row, int col);
void GFX_setForeground(GFX* gfx_p, uint32_t foreground);
void GFX_setBackground(GFX* gfx_p, uint32_t background);

void GFX_drawSolidCircle(GFX* gfx_p, int x, int y, int radius);
void GFX_drawHollowCircle(GFX* gfx_p, int x, int y, int radius);

void GFX_removeSolidCircle(GFX* gfx_p, int x, int y, int radius);
void GFX_removeHollowCircle(GFX* gfx_p, int x, int y, int radius);
// Constructs an HAL object by calling the constructor of each individual member
HAL HAL_construct();

// Refreshes all necessary inputs in the HAL
void HAL_refresh(HAL* api);

#endif /* HAL_HAL_H_ */
