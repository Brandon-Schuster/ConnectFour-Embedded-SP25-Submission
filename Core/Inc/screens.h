/*
 * screens.h
 *
 *  Created on: Apr 23, 2025
 *      Author: brand
 */

#ifndef INC_SCREENS_H_
#define INC_SCREENS_H_

#include "LCD_Driver.h"
#include "game.h"
#include <stdio.h>

void StartScreen(void);
void GameScreen(void);
void EndScreen(uint8_t winner, uint32_t duration_ms, int redWins, int yellowWins);
void Debug_Mode(void);

// Game Screen Drawing and helpers
void DrawToken(int row, int col, uint8_t player);
void DrawCursor(int col, uint8_t player);
void EraseCursor(int col);

void DrawCircleAtCell(int col, int row, uint16_t color);

// Debug Drawing
void DrawDebugGrid(void);
void DrawDebugBug(void);


#endif /* INC_SCREENS_H_ */
