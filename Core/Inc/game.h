/*
 * game.h
 *
 *  Created on: Apr 23, 2025
 *      Author: brand
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_

#include "screens.h"


#define ROWS 6
#define COLS 7

#define EMPTY  0
#define RED    1
#define YELLOW 2

extern uint8_t board[ROWS][COLS];

void Game_Init(void); // Main Game Call

void Start_Menu(void); // Start Menu

void Start_Game(bool is1p); // Main Game Loop
void Game_CleanBoard(void); // Init an empty board as board[r][c]
void Restart_Game(void); // Just call Start_Game with the is1p bool

int  Game_PlacingToken(int currentPlayer); // Player choosing where to place token
int  Game_DropToken(uint8_t col, uint8_t player); // Place a token in the board
bool ColumnHasSpace(int col); // For the 1P to work, we need to check that the random row chosen isn't full

bool Game_CheckWin(uint8_t player); // Check: Horizontal, Vertical, Diagonal right, & Diagonal Left
bool BoardIsFull(void); // Check top most row, if full then tie


#endif /* INC_GAME_H_ */
