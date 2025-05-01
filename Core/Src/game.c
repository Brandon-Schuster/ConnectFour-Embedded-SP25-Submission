/*
 * game.c
 *
 *  Created on: Apr 23, 2025
 *      Author: brand
 */

#include "game.h"

extern RNG_HandleTypeDef hrng;

static bool is1p; // To remember game mode

static int redWins = 0;
static int yellowWins = 0;

uint8_t board[ROWS][COLS];

volatile bool dropRequested = false; // Button Interrupt Flag

// Interrupt flag triggered by the button
// Sets a flag that the main game loop checks to drop a token
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0)
    {
        dropRequested = true;
    }
}

void Game_Init(void)
{
    Start_Menu(); // Start_Menu is pretty much the main loop, but put inside here just in case I want to add more later
}

void Restart_Game(void)
{
    Start_Game(is1p); // Restart the game with the correct mode
}

void Start_Menu(void)
{
    StartScreen(); // Draw the screen
    while (1)
    {
        STMPE811_TouchData touch;
        touch.orientation = STMPE811_Orientation_Portrait_1;
        STMPE811_State_t state = STMPE811_ReadTouch(&touch); // Grab current touch input and fill struct with .x/.y/.pressed

        if (state == STMPE811_State_Pressed)
        {
            // Debug Mode
            if (TM_STMPE811_TouchInRectangle(&touch, 200, 280, 40, 40))
            {
                Debug_Mode();
                StartScreen();
            }
            // 1P-Mode
            if (TM_STMPE811_TouchInRectangle(&touch, 60, 90, 40, 140))
            {
                is1p = true;
                Start_Game(true);
                return;
            }
            // 2P-Mode
            if (TM_STMPE811_TouchInRectangle(&touch, 120, 90, 40, 140))
            {
                is1p = false;
                Start_Game(false);
                return;
            }
        }

        HAL_Delay(100);
    }
}

void Start_Game(bool is1P)
{
    LCD_Clear(0, LCD_COLOR_BLACK);
    Game_CleanBoard(); // Start with a clean board in mem
    GameScreen();      // Draw the actual board

    int currentPlayer = RED; // Idk, red as first
    bool gameOver = false;

    uint32_t startTime = HAL_GetTick();

    while (!gameOver)
    {
        int result = -1;

        // Player Moves
        if (!is1P || currentPlayer == RED)
        {
            // Either 2P, or it's the human player's turn in 1P
            result = Game_PlacingToken(currentPlayer);
        }
        else
        {
            // CPU Turn in 1P Mode (YELLOW)
            HAL_Delay(150);

            int col = 0;       // column we will finally use
            bool good = false; // becomes true once we find a legal column

            while (!good)
            {
                uint32_t rnd = 0;
                HAL_RNG_GenerateRandomNumber(&hrng, &rnd); // grab a 32-bit random value

                // Mod calculationt to give us a value between 0 to 6
                col = (int)(rnd % COLS);

                // Only use if it isn't full
                if (ColumnHasSpace(col))
                {
                    good = true; // exit
                }
            }

            int row = Game_DropToken(col, currentPlayer);
            DrawToken(row, col, currentPlayer); // draw the piece
            result = col;                       // For the win/tie logic
        }

        // Check the results
        if (result >= 0)
        {
            if (Game_CheckWin(currentPlayer))
            {
                if (currentPlayer == RED)
                    redWins++;
                else
                    yellowWins++;

                uint32_t duration = HAL_GetTick() - startTime;
                EndScreen(currentPlayer, duration, redWins, yellowWins);
                gameOver = true;
            }
            else if (BoardIsFull())
            {
                uint32_t duration = HAL_GetTick() - startTime;
                EndScreen(0, duration, redWins, yellowWins);
                gameOver = true;
            }
            else
            {
                // Next player play
                if (currentPlayer == RED)
                {
                    currentPlayer = YELLOW;
                }
                else
                {
                    currentPlayer = RED;
                }
            }

            HAL_Delay(150);
        }

        HAL_Delay(50);
    }
}

// for the 1p mode, we have to know that the rng is valid so we check that the column is empty
bool ColumnHasSpace(int col)
{
    for (int r = 0; r < ROWS; r++)
    {
        if (board[r][col] == EMPTY)
            return true;
    }
    return false;
}

int Game_PlacingToken(int currentPlayer)
{
    static int currentCol = 3;
    int lastCol = currentCol; // Keep a track of where the cursor last was so we can erase it

    DrawCursor(currentCol, currentPlayer); // Initial draw

    while (1)
    {

        STMPE811_TouchData touch;
        touch.orientation = STMPE811_Orientation_Portrait_1;
        STMPE811_State_t state = STMPE811_ReadTouch(&touch);

        if (state == STMPE811_State_Pressed)
        {
            if (touch.y > 160 && currentCol > 0)
            {
                lastCol = currentCol;
                currentCol--;
            }
            else if (touch.y <= 160 && currentCol < COLS - 1)
            {
                lastCol = currentCol;
                currentCol++;
            }

            // As the cursor moves left and right, erase the prev location
            if (lastCol != currentCol)
            {
                // Erase previous cursor
                EraseCursor(lastCol);

                // Draw new cursor
                DrawCursor(currentCol, currentPlayer);
                HAL_Delay(200);
            }
        }

        // Button Interrupt
        if (dropRequested)
        {
            dropRequested = false; // Reset the flag

            int placedRow = Game_DropToken(currentCol, currentPlayer);

            if (placedRow >= 0)
            {
                DrawToken(placedRow, currentCol, currentPlayer);
                EraseCursor(currentCol);

                // Kinda a debounce guard. had some issues where it mis registerd the button
                while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
                {
                    HAL_Delay(10);
                }

                return currentCol;
            }
        }

        HAL_Delay(50);
    }

    return -1;
}

bool BoardIsFull(void)
{
    for (int c = 0; c < COLS; c++)
    {
        for (int r = 0; r < ROWS; r++)
        {
            if (board[r][c] == EMPTY)
            {
                return false; // Found at least one empty slot
            }
        }
    }
    return true; // No empties found
}

// Genereate/Clean a fresh board
void Game_CleanBoard(void)
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            board[r][c] = EMPTY;
        }
    }
}

int Game_DropToken(uint8_t col, uint8_t player)
{
    int actualCol = (COLS - 1) - col; // Flip the column

    for (int row = ROWS - 1; row >= 0; row--)
    {
        if (board[row][actualCol] == EMPTY)
        {
            board[row][actualCol] = player;
            return row; // Return the row where token landed
        }
    }
    return -1; // Column full
}

bool Game_CheckWin(uint8_t player)
{
    // Horizontal
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c <= COLS - 4; c++)
        {
            if (board[r][c] == player && board[r][c + 1] == player &&
                board[r][c + 2] == player && board[r][c + 3] == player)
                return true;
        }
    }

    // Vertical
    for (int r = 0; r <= ROWS - 4; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if (board[r][c] == player && board[r + 1][c] == player &&
                board[r + 2][c] == player && board[r + 3][c] == player)
                return true;
        }
    }

    // Diagonal right
    for (int r = 0; r <= ROWS - 4; r++)
    {
        for (int c = 0; c <= COLS - 4; c++)
        {
            if (board[r][c] == player && board[r + 1][c + 1] == player &&
                board[r + 2][c + 2] == player && board[r + 3][c + 3] == player)
                return true;
        }
    }

    // Diagonal left
    for (int r = 0; r <= ROWS - 4; r++)
    {
        for (int c = 3; c < COLS; c++)
        {
            if (board[r][c] == player && board[r + 1][c - 1] == player &&
                board[r + 2][c - 2] == player && board[r + 3][c - 3] == player)
                return true;
        }
    }

    return false;
}
