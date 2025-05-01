/*
 * Button_Driver.h
 *
 *  Created on: Feb 20, 2025
 *      Author: Brandon Schuster
 */

#ifndef BUTTON_DRIVER_H_
#define BUTTON_DRIVER_H_

#include "stm32f4xx_hal.h"


#define BUTTON_PORT GPIOA
#define BUTTON_PIN 0

#define BUTTON_IS_PRESSED 1
#define BUTTON_NOT_PRESSED 0

void Button_Init(void);
uint8_t Button_Pressed(void);

void Button_InitInterrupt(void);

#endif /* BUTTON_DRIVER_H_ */
