/*
 * API.c
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#include <HAL/HAL.h>

/**
 * Constructs a new API object. The API constructor should simply call the
 * constructors of each of its sub-members with the proper inputs.
 *
 * @return a properly constructed API object.
 */
HAL HAL_construct() {
  // The API object which will be returned at the end of construction
  HAL hal;

  // Initialize all LEDs by calling their constructors with correctly-defined
  // arguments.
  hal.launchpadLED1 = LED_construct(LAUNCHPAD_LED1_PORT, LAUNCHPAD_LED1_PIN);

  hal.launchpadLED2Red =
      LED_construct(LAUNCHPAD_LED2_RED_PORT, LAUNCHPAD_LED2_RED_PIN);
  hal.launchpadLED2Green =
      LED_construct(LAUNCHPAD_LED2_GREEN_PORT, LAUNCHPAD_LED2_GREEN_PIN);
  hal.launchpadLED2Blue =
      LED_construct(LAUNCHPAD_LED2_BLUE_PORT, LAUNCHPAD_LED2_BLUE_PIN);

  hal.boosterpackRed =
      LED_construct(BOOSTERPACK_LED_RED_PORT, BOOSTERPACK_LED_RED_PIN);
  hal.boosterpackGreen =
      LED_construct(BOOSTERPACK_LED_GREEN_PORT, BOOSTERPACK_LED_GREEN_PIN);
  hal.boosterpackBlue =
      LED_construct(BOOSTERPACK_LED_BLUE_PORT, BOOSTERPACK_LED_BLUE_PIN);

  // Initialize all Buttons by calling their constructors with correctly-defined
  // arguments.
  hal.launchpadS1 =
      Button_construct(LAUNCHPAD_S1_PORT, LAUNCHPAD_S1_PIN);  // Launchpad S1
  hal.launchpadS2 =
      Button_construct(LAUNCHPAD_S2_PORT, LAUNCHPAD_S2_PIN);  // Launchpad S2

  hal.boosterpackS1 = Button_construct(BOOSTERPACK_S1_PORT,
                                       BOOSTERPACK_S1_PIN);  // Boosterpack S1
  hal.boosterpackS2 = Button_construct(BOOSTERPACK_S2_PORT,
                                       BOOSTERPACK_S2_PIN);  // Boosterpack S2
  hal.boosterpackJS = Button_construct(BOOSTERPACK_JS_PORT,
                                       BOOSTERPACK_JS_PIN);  // Joystick Button

  // Construct the UART module inside of this HAL struct
  hal.uart = UART_construct(USB_UART_INSTANCE, USB_UART_PORT, USB_UART_PINS);
  //construct for gfx
  hal.gfx = GFX_construct(GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_BLACK);


  // Enable the UART at 9600 BPS
  // TODO: Call UART_SetBaud_Enable to achieve the above goal
UART_SetBaud_Enable(&hal.uart , BAUD_9600);
  // Once we have finished building the API, return the completed struct.


   hal.joystick = Joystick_construct();
  return hal;

}

/**
 * Upon every new cycle of the main super-loop, we MUST UPDATE the status of
 * all inputs. In this program, this function is called only once in the
 * Application_loop() function. Since the Application_loop() function is called
 * once per loop of the while (true) loop in main, we are effectively polling
 * all inputs once per loop.
 *
 * @param hal:  The API whose input modules we wish to refresh
 */
void HAL_refresh(HAL* hal) {
  // Refresh Launchpad buttons
  Button_refresh(&hal->launchpadS1);
  Button_refresh(&hal->launchpadS2);

  // Refresh Boosterpack buttons
  Button_refresh(&hal->boosterpackS1);
  Button_refresh(&hal->boosterpackS2);
  Button_refresh(&hal->boosterpackJS);

  Joystick_refresh(&hal->joystick);

  // Not real TODO: No need to add anything for UART
}
GFX GFX_construct(uint32_t defaultForeground, uint32_t defaultBackground)
{
    GFX gfx;

    gfx.defaultForeground = defaultForeground;
    gfx.defaultBackground = defaultBackground;

    // initializing the display
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    // setting up the graphics
    Graphics_initContext(&gfx.context, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setFont(&gfx.context, &g_sFontFixed6x8);

    GFX_resetColors(&gfx);
    GFX_clear(&gfx);

    return gfx;
}
void GFX_resetColors(GFX* gfx_p)
{
    gfx_p->foreground = gfx_p->defaultForeground;
    gfx_p->background = gfx_p->defaultBackground;

    Graphics_setForegroundColor(&gfx_p->context, gfx_p->defaultForeground);
    Graphics_setBackgroundColor(&gfx_p->context, gfx_p->defaultBackground);
}

void GFX_clear(GFX* gfx_p)
{
    Graphics_clearDisplay(&gfx_p->context);
}

void GFX_print(GFX* gfx_p, char* string, int row, int col)
{
    int yPosition = row * Graphics_getFontHeight(gfx_p->context.font);
    int xPosition = col * Graphics_getFontMaxWidth(gfx_p->context.font);

    Graphics_drawString(&gfx_p->context, (int8_t*) string, -1, xPosition, yPosition, OPAQUE_TEXT);
}

void GFX_setForeground(GFX* gfx_p, uint32_t foreground)
{
    gfx_p->foreground = foreground;
    Graphics_setForegroundColor(&gfx_p->context, foreground);
}

void GFX_setBackground(GFX* gfx_p, uint32_t background)
{
    gfx_p->background = background;
    Graphics_setBackgroundColor(&gfx_p->context, background);
}

void GFX_drawSolidCircle(GFX* gfx_p, int x, int y, int radius)
{
    Graphics_fillCircle(&gfx_p->context, x, y, radius);
}

void GFX_drawHollowCircle(GFX* gfx_p, int x, int y, int radius)
{
    Graphics_drawCircle(&gfx_p->context, x, y, radius);
}

void GFX_removeSolidCircle(GFX* gfx_p, int x, int y, int radius)
{
    uint32_t oldForegroundColor = gfx_p->foreground;
    GFX_setForeground(gfx_p, gfx_p->background);
    GFX_drawSolidCircle(gfx_p, x, y, radius);
    GFX_setForeground(gfx_p, oldForegroundColor);
}

void GFX_removeHollowCircle(GFX* gfx_p, int x, int y, int radius)
{
    uint32_t oldForegroundColor = gfx_p->foreground;
    GFX_setForeground(gfx_p, gfx_p->background);
    GFX_drawHollowCircle(gfx_p, x, y, radius);
    GFX_setForeground(gfx_p, oldForegroundColor);
}

