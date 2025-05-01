/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: Xavion
 */

#include "ApplicationCode.h"

/* Static variables */


extern void initialise_monitor_handles(void); 

#if COMPILE_TOUCH_FUNCTIONS == 1
STMPE811_TouchData StaticTouchData;
#endif // COMPILE_TOUCH_FUNCTIONS


void ApplicationInit(void)
{
	initialise_monitor_handles(); // Allows printf functionality
    LTCD__Init();
    LTCD_Layer_Init(0);
    LCD_SetFont(&Font16x24);
    LCD_Clear(0,LCD_COLOR_BLACK);
	InitializeLCDTouch();    
    __HAL_RCC_RNG_CLK_ENABLE();  // Enable the RNG peripheral clock
    Button_InitInterrupt(); // Button Interrupts
}


