/*
 Joystick.c
 Created on: Apr 1, 2025
 Author: Brittany Linares
 */

#include <HAL/Joystick.h>

#define LEFT_THRESHOLD 3000
#define UP_THRESHOLD 10000
#define DOWN_THRESHOLD 3000
#define RIGHT_THRESHOLD 10000
//12K

enum _JoystickDebounceStateLeft{LEFT, NOT_LEFT };
typedef enum _JoystickDebounceStateLeft JoystickDebounceStateLeft;


enum _JoystickDebounceStateRight{RIGHT, NOT_RIGHT };
typedef enum _JoystickDebounceStateRight JoystickDebounceStateRight;

enum _JoystickDebounceStateUp{UP, NOT_UP};
typedef enum _JoystickDebounceStateUp JoystickDebounceStateUp;

enum _JoystickDebounceStateDown{DOWN, NOT_DOWN };
typedef enum _JoystickDebounceStateDown JoystickDebounceStateDown;


void initADC() {
    ADC14_enableModule();

    ADC14_initModule(ADC_CLOCKSOURCE_SYSOSC,
                     ADC_PREDIVIDER_1,
                     ADC_DIVIDER_1,
                      0
                     );

    // This configures the ADC to store output results
    // in ADC_MEM0 for joystick X.
    // Todo: if we want to add joystick Y, then, we have to use more memory locations
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);

    // This configures the ADC in manual conversion mode
    // Software will start each conversion.
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
}


void startADC() {
   // Starts the ADC with the first conversion
   // in repeat-mode, subsequent conversions run automatically
   ADC14_enableConversion();
   ADC14_toggleConversionTrigger();
}


// Interfacing the Joystick with ADC (making the proper connections in software)
void initJoyStick() {

    // This configures ADC_MEM0 to store the result from
    // input channel A15 (Joystick X), in non-differential input mode
    // (non-differential means: only a single input pin)
    // The reference for Vref- and Vref+ are VSS and VCC respectively
    ADC14_configureConversionMemory(ADC_MEM0,
                                  ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                  ADC_INPUT_A15,                 // joystick X
                                  ADC_NONDIFFERENTIAL_INPUTS);


    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                               GPIO_PIN0,
                                               GPIO_TERTIARY_MODULE_FUNCTION);


    ADC14_configureConversionMemory(ADC_MEM1,
                                      ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                      ADC_INPUT_A9,                 // joystick X
                                      ADC_NONDIFFERENTIAL_INPUTS);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                                   GPIO_PIN4,
                                                   GPIO_TERTIARY_MODULE_FUNCTION);


}


Joystick Joystick_construct() {
  // The Joystick object which will be returned at the end of construction
  Joystick Joystick;

  initADC();
  initJoyStick();
  startADC();

//  // Initialize all buffered outputs of the Joystick
//  Joystick.pushState = RELEASED;
//  Joystick.isTapped = false;

  // Return the constructed Joystick object to the user
  return Joystick;
}


/**
 * Refreshes the input of the provided Joystick by polling its x and y channel
 *
 * @param joystick_p:   The Joystick object to refresh
 */
void Joystick_refresh(Joystick* joystick_p)
{
    joystick_p->x = ADC14_getResult(ADC_MEM0);
    joystick_p->y = ADC14_getResult(ADC_MEM1);
    joystick_p->isTappedToLeft = false;
    joystick_p->isTappedToUp = false;
    joystick_p->isTappedToDown = false;
    joystick_p->isTappedToRight = false;

    static JoystickDebounceStateLeft leftState = NOT_LEFT;
    switch(leftState){
    case NOT_LEFT:
    if(joystick_p-> x < LEFT_THRESHOLD)
    {
        leftState = LEFT;
        joystick_p->isTappedToLeft = true;
    }
    break;
    case LEFT:
        if(joystick_p->x > LEFT_THRESHOLD)
           {
               leftState = NOT_LEFT;
           }
}



    static JoystickDebounceStateRight rightState = NOT_RIGHT;
      switch(rightState){
      case NOT_RIGHT:
      if(joystick_p-> x > RIGHT_THRESHOLD)
      {
          rightState = RIGHT;
          joystick_p->isTappedToRight = true;
      }
      break;
      case RIGHT:
          if(joystick_p->x < RIGHT_THRESHOLD)
             {
                 rightState = NOT_RIGHT;
             }
  }

      static JoystickDebounceStateUp upState = NOT_UP;
          switch(upState){
          case NOT_UP:
          if(joystick_p-> y > UP_THRESHOLD)
          {
              upState = UP;
              joystick_p->isTappedToUp = true;
          }
          break;
          case UP:
              if(joystick_p->y < UP_THRESHOLD)
                 {
                     upState = NOT_UP;
                 }
      }

          static JoystickDebounceStateUp downState = NOT_DOWN;
                    switch(downState){
                    case NOT_DOWN:
                    if(joystick_p-> y < DOWN_THRESHOLD)
                    {
                        downState = DOWN;
                        joystick_p->isTappedToDown = true;
                    }
                    break;
                    case DOWN:
                        if(joystick_p->y > DOWN_THRESHOLD)
                           {
                               downState = NOT_DOWN;
                           }
                }

}


/** Given a Joystick, determines if thejoystickis currently pressed to left */
bool Joystick_isPressedToLeft(Joystick* joystick_p)
{
    return(joystick_p->x < LEFT_THRESHOLD);
}

/** Given a Joystick, determines if it was "tapped" - pressed down and released */
bool Joystick_isTappedToLeft(Joystick* joystick_p)
{
    return(joystick_p->isTappedToLeft);
}

bool Joystick_isPressedToUp(Joystick* joystick_p)
{
    return(joystick_p->y < UP_THRESHOLD);
}

bool Joystick_isTappedToUp(Joystick* joystick_p)
{
    return(joystick_p->isTappedToUp);
}

bool Joystick_isPressedToDown(Joystick* joystick_p)
{
    return(joystick_p->y < DOWN_THRESHOLD);
}

bool Joystick_isTappedToDown(Joystick* joystick_p)
{
    return(joystick_p->isTappedToDown);
}

bool Joystick_isPressedToRight(Joystick* joystick_p)
{
    return(joystick_p->x < RIGHT_THRESHOLD);
}

bool Joystick_isTappedToRight(Joystick* joystick_p)
{
    return(joystick_p->isTappedToRight);
}


