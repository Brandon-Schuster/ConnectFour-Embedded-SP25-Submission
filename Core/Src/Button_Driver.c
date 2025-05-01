/*
 * Button_Driver.c
 *
 *  Created on: Feb 20, 2025
 *      Author: Brandon Schuster
 */

#include "Button_Driver.h"

// Polling Based; didn't actually use this, but just in case
void Button_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef buttonConfig = {0};

    buttonConfig.Pin   = GPIO_PIN_0;
    buttonConfig.Mode  = GPIO_MODE_INPUT;
    buttonConfig.Pull  = GPIO_PULLDOWN;
    buttonConfig.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &buttonConfig);
}
// Here is the previous setup we had...
//    GPIO_ClockControl(GPIOA, ACTIVE);
//
//    GPIO_PinConfig_t buttonConfig;
//    buttonConfig.PinNumber      = BUTTON_PIN;
//    buttonConfig.PinMode        = INPUT_RESET_MODE;
//    buttonConfig.PinSpeed       = LOW_SPEED;
//    buttonConfig.PinPuPdControl = PULL_DOWN;
//    buttonConfig.OPType         = PUSH_PULL_STATE;
//
//    buttonConfig.PinInterruptMode = INTERRUPT_MODE_NONE;
//
//    GPIO_Init(BUTTON_PORT, buttonConfig);


uint8_t Button_Pressed(void) {
//	return (read_pin_input(BUTTON_PORT, BUTTON_PIN) == BUTTON_IS_PRESSED) ? 1 : 0;
    return (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET);
}

//Interrupt Based
void Button_InitInterrupt(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
//    __HAL_RCC_SYSCFG_CLK_ENABLE(); // Needed to remap EXTI lines

    GPIO_InitTypeDef buttonConfig = {0};
    buttonConfig.Pin   = GPIO_PIN_0;
    buttonConfig.Mode  = GPIO_MODE_IT_RISING; // Generate an interrupt on rising/falling edge
    buttonConfig.Pull  = GPIO_PULLDOWN;
    buttonConfig.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &buttonConfig);

    // Clear any pending interrupt (just in case something is set from before)
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0); // Priority 2, sub-priority 0. Not sure if they're the correct values tho
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
// Previous setup...
//    GPIO_ClockControl(BUTTON_PORT, ACTIVE);
//
//    GPIO_PinConfig_t buttonConfig;
//    buttonConfig.PinNumber      = BUTTON_PIN;
//    buttonConfig.PinMode        = INPUT_RESET_MODE;
//    buttonConfig.PinSpeed       = LOW_SPEED;
//    buttonConfig.PinPuPdControl = PULL_DOWN;
//    buttonConfig.OPType         = PUSH_PULL_STATE;
//    buttonConfig.PinInterruptMode = INTERRUPT_MODE_RFT;
//
//    GPIO_Init(BUTTON_PORT, buttonConfig);
//
//    GPIO_IRQConfig(EXTI0_IRQ_NUMBER, ENABLE);

