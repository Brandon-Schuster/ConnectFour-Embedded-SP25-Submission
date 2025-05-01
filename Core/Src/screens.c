/*
 * screens.c
 *
 *  Created on: Apr 23, 2025
 *      Author: brand
 */

#include "screens.h"

#ifndef LEFT_MODE
#define LEFT_MODE 0
#endif
#ifndef CENTER_MODE
#define CENTER_MODE 1
#endif
#ifndef RIGHT_MODE
#define RIGHT_MODE 2
#endif

#define CELL_SIZE (CELL_RADIUS * 2 + CELL_SPACING)
#define CELL_RADIUS 12 // Smaller = tighter circles
#define CELL_SPACING 4 // Smaller = more compact grid
#define ANCHOR_X 75    // Right edge base X
#define ANCHOR_Y 100   // Top edge base Y

void StartScreen(void)
{
    LCD_Clear(0, LCD_COLOR_BLACK);
    LCD_SetTextColor(LCD_COLOR_YELLOW);

    // Title background
    for (int y = 60; y < 260; y++)
    {
        for (int x = 5; x < 45; x++)
        {
            LCD_Draw_Pixel(x, y, LCD_COLOR_RED);
        }
    }

    LCD_SetTextColor(LCD_COLOR_BLACK);

    // 1P button
    for (int y = 90; y < 230; y++)
    {
        for (int x = 60; x < 100; x++)
        {
            LCD_Draw_Pixel(x, y, LCD_COLOR_YELLOW);
        }
    }

    // 2P button
    for (int y = 90; y < 230; y++)
    {
        for (int x = 120; x < 160; x++)
        {
            LCD_Draw_Pixel(x, y, LCD_COLOR_RED);
        }
    }

    LCD_DisplayString(70, 15, (uint8_t *)"CONNECT FOUR", CENTER_MODE);
    LCD_DisplayString(100, 70, (uint8_t *)"1 PLAYER", CENTER_MODE);
    LCD_DisplayString(100, 130, (uint8_t *)"2 PLAYER", CENTER_MODE);
    LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_DisplayString(5, 215, (uint8_t *)"Brandon Schuster", LEFT_MODE);
    DrawDebugBug(); // My attempt at a debug icon
}

void GameScreen(void)
{
    // Blue board
    for (int y = 50; y < 265; y++)
    {
        for (int x = 55; x < 240; x++)
        {
            LCD_Draw_Pixel(x, y, LCD_COLOR_BLUE2);
        }
    }
    // Slots and pieces placed already
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            uint16_t color = LCD_COLOR_BLACK;

            if (board[r][c] == RED)
                color = LCD_COLOR_RED;
            else if (board[r][c] == YELLOW)
                color = LCD_COLOR_YELLOW;

            DrawCircleAtCell(r, COLS - 1 - c, color);
        }
    }
}

void DrawCircleAtCell(int col, int row, uint16_t color)
{
    int x = ANCHOR_X + col * CELL_SIZE;              // Move left per column
    int y = ANCHOR_Y + (ROWS - 1 - row) * CELL_SIZE; // Move down per row
    LCD_Draw_Circle_Fill(x, y, CELL_RADIUS, color);
}

void DrawToken(int row, int col, uint8_t player)
{
    uint16_t color = LCD_COLOR_WHITE;

    if (player == RED)
    {
        color = LCD_COLOR_RED;
    }
    else if (player == YELLOW)
    {
        color = LCD_COLOR_YELLOW;
    }

    DrawCircleAtCell(row, col, color);
}

void DrawCursor(int currentCol, uint8_t currentPlayer)
{
    uint16_t color = (currentPlayer == RED) ? LCD_COLOR_RED : LCD_COLOR_YELLOW;

    // Draw hovering above +2 rows
    int row = ROWS + 2;

    int drawRow = currentCol;
    int drawCol = COLS - 1 - row;

    DrawCircleAtCell(drawCol, drawRow, color);
}

void EraseCursor(int col)
{
    int offset = -2;
    DrawCircleAtCell(offset, col, LCD_COLOR_BLACK);
}

void EndScreen(uint8_t winner, uint32_t duration_ms, int redWins, int yellowWins)
{
    LCD_Clear(0, LCD_COLOR_BLACK);
    LCD_SetTextColor(LCD_COLOR_WHITE);

    // Draw winner message
    if (winner == RED)
    {
        LCD_DisplayString(50, 20, (uint8_t *)"Red Wins!", LEFT_MODE);
    }
    else if (winner == YELLOW)
    {
        LCD_DisplayString(50, 20, (uint8_t *)"Yellow Wins!", LEFT_MODE);
    }
    else
    {
        LCD_DisplayString(50, 20, (uint8_t *)"It's a Draw!", LEFT_MODE);
    }

    // Draw duration
    char durationMessage[32];
    sprintf(durationMessage, "Time: %lu.%03lus sec", duration_ms / 1000, duration_ms % 1000);
    LCD_DisplayString(50, 60, (uint8_t *)durationMessage, LEFT_MODE);

    // Draw running score
    char scoreMessage[32];
    sprintf(scoreMessage, "Red: %d  Yellow: %d", redWins, yellowWins);
    LCD_DisplayString(50, 100, (uint8_t *)scoreMessage, LEFT_MODE);

    LCD_SetTextColor(LCD_COLOR_BLACK);
    // Draw Restart Button manually (green block)
    for (int y = 80; y < 240; y++)
    {
        for (int x = 140; x < 170; x++)
        {
            LCD_Draw_Pixel(x, y, LCD_COLOR_GREEN);
        }
    }
    LCD_DisplayString(85, 145, (uint8_t *)"Play Again", LEFT_MODE);

    // Draw Exit Button manually (red block)
    for (int y = 80; y < 240; y++)
    {
        for (int x = 190; x < 220; x++)
        {
            LCD_Draw_Pixel(x, y, LCD_COLOR_RED);
        }
    }
    LCD_DisplayString(95, 195, (uint8_t *)"Main Menu", LEFT_MODE);

    while (1)
    {
        STMPE811_TouchData touch;
        touch.orientation = STMPE811_Orientation_Portrait_1;

        STMPE811_State_t state = STMPE811_ReadTouch(&touch);

        if (state == STMPE811_State_Pressed)
        {
            if (touch.x >= 140 && touch.x <= 170 && touch.y >= 80 && touch.y <= 240)
            {
                HAL_Delay(200); // Debounce
                Restart_Game();
            }

            if (touch.x >= 190 && touch.x <= 220 && touch.y >= 80 && touch.y <= 240)
            {
                HAL_Delay(200); // Debounce
                Start_Menu();
            }
        }

        HAL_Delay(50);
    }
}

void Debug_Mode(void)
{
    bool gridDrawn = false;
    bool cleanMode = false;
    bool wasTouching = false;

    uint32_t lastInteraction = HAL_GetTick(); // Track last interaction
    Point prev_p = {-1, -1};                  // Start invalid

    STMPE811_TouchData touch;
    touch.orientation = STMPE811_Orientation_Portrait_1;

    while (1)
    {
        STMPE811_State_t state = STMPE811_ReadTouch(&touch);

        if (state == STMPE811_State_Pressed)
        {
            lastInteraction = HAL_GetTick(); // Reset timeout on touch
        }

        // Exit debug mode after 10 seconds of no interaction
        if (HAL_GetTick() - lastInteraction >= 5000)
        {
            LCD_Clear(0, LCD_COLOR_BLACK);
            return;
        }

        // Draw grid once on entry
        if (!gridDrawn && !cleanMode)
        {
            DrawDebugGrid();
            gridDrawn = true;
        }

        // On first touch, enter clean mode
        if (!cleanMode && state == STMPE811_State_Pressed)
        {
            cleanMode = true;
            LCD_Clear(0, LCD_COLOR_BLACK);
        }

        if (cleanMode)
        {
            if (state == STMPE811_State_Pressed)
            {
                wasTouching = true;

                // Erase prev crosshair
                if (prev_p.x >= 0 && prev_p.y >= 0)
                {
                    for (uint16_t y = 0; y < LCD_PIXEL_HEIGHT; y++)
                    {
                        LCD_Draw_Pixel(prev_p.x, y, LCD_COLOR_BLACK);
                    }
                    for (uint16_t x = 0; x < LCD_PIXEL_WIDTH; x++)
                    {
                        LCD_Draw_Pixel(x, prev_p.y, LCD_COLOR_BLACK);
                    }
                    LCD_Draw_Circle_Fill(prev_p.x, prev_p.y, 4, LCD_COLOR_BLACK);
                }

                // Draw new crosshair
                for (uint16_t y = 0; y < LCD_PIXEL_HEIGHT; y++)
                {
                    LCD_Draw_Pixel(touch.x, y, LCD_COLOR_GREEN);
                }
                for (uint16_t x = 0; x < LCD_PIXEL_WIDTH; x++)
                {
                    LCD_Draw_Pixel(x, touch.y, LCD_COLOR_GREEN);
                }
                LCD_Draw_Circle_Fill(touch.x, touch.y, 4, LCD_COLOR_RED);

                // Save pos
                prev_p.x = touch.x;
                prev_p.y = touch.y;
            }
            else if (wasTouching)
            {
                // If touch is released, then just reset back to debug grid
                cleanMode = false;
                gridDrawn = false;
                wasTouching = false;
                LCD_Clear(0, LCD_COLOR_BLACK);

                prev_p.x = -1;
                prev_p.y = -1;
            }
        }

        HAL_Delay(25); // Frame pacing
    }
}

void DrawDebugGrid(void)
{
    LCD_Clear(0, LCD_COLOR_BLACK);
    LCD_SetFont(&Font16x24);
    LCD_SetTextColor(LCD_COLOR_RED);

    // Center mark
    LCD_Draw_Circle_Fill(LCD_PIXEL_WIDTH / 2, LCD_PIXEL_HEIGHT / 2, 4, LCD_COLOR_GREEN);

    // Grid lines
    for (int x = 0; x < LCD_PIXEL_WIDTH; x += 20)
    {
        for (int y = 0; y < LCD_PIXEL_HEIGHT; y++)
        {
            LCD_Draw_Pixel(x, y, LCD_COLOR_GREY);
        }
    }

    for (int y = 0; y < LCD_PIXEL_HEIGHT; y += 20)
    {
        for (int x = 0; x < LCD_PIXEL_WIDTH; x++)
        {
            LCD_Draw_Pixel(x, y, LCD_COLOR_GREY);
        }
    }
    // Boxes behind coordinates
    for (int y = 0; y < 80; y++)
    {
        for (int x = 0; x < 40; x++)
        {
            LCD_Draw_Pixel(x, y, LCD_COLOR_BLACK);
        }
    }

    for (int y = 221; y < 320; y++)
    {
        for (int x = 0; x < 40; x++)
        {
            LCD_Draw_Pixel(x, y, LCD_COLOR_BLACK);
        }
    }
    for (int y = 0; y < 100; y++)
    {
        for (int x = 201; x < 240; x++)
        {
            LCD_Draw_Pixel(x, y, LCD_COLOR_BLACK);
        }
    }
    for (int y = 201; y < 320; y++)
    {
        for (int x = 201; x < 240; x++)
        {
            LCD_Draw_Pixel(x, y, LCD_COLOR_BLACK);
        }
    }
    LCD_DisplayString(5, 7, (uint8_t *)"(0,0)", LEFT_MODE);
    LCD_DisplayString(225, 7, (uint8_t *)"(320,0)", LEFT_MODE);
    LCD_DisplayString(2, 210, (uint8_t *)"(0,240)", LEFT_MODE);
    LCD_DisplayString(202, 210, (uint8_t *)"(320,240)", LEFT_MODE);
}

void DrawDebugBug(void)
{
    // Head
    for (int y = -5; y <= 5; y++)
    {
        for (int x = -5; x <= 5; x++)
        {
            if (x * x + y * y <= 25)
            {
                LCD_Draw_Pixel(208 + y, 300 - x, LCD_COLOR_DARKGREEN);
            }
        }
    }

    // Eyes
    LCD_Draw_Pixel(210, 298, LCD_COLOR_MAGENTA); // Top eye
    LCD_Draw_Pixel(210, 302, LCD_COLOR_MAGENTA); // Bottom eye

    // Body
    for (int y = -10; y <= 10; y++)
    {
        for (int x = -10; x <= 10; x++)
        {
            if (x * x + y * y <= 100)
            {
                LCD_Draw_Pixel(220 + y, 300 - x, LCD_COLOR_GREEN);
            }
        }
    }

    // Legs, 3 on each side
    for (int i = -1; i <= 1; i++)
    {
        LCD_Draw_Pixel(220 + i * 6, 287, LCD_COLOR_WHITE);
        LCD_Draw_Pixel(220 + i * 6, 286, LCD_COLOR_WHITE);
        LCD_Draw_Pixel(220 + i * 6, 285, LCD_COLOR_WHITE);

        LCD_Draw_Pixel(220 + i * 6, 313, LCD_COLOR_WHITE);
        LCD_Draw_Pixel(220 + i * 6, 314, LCD_COLOR_WHITE);
        LCD_Draw_Pixel(220 + i * 6, 315, LCD_COLOR_WHITE);
    }

    // Antennae or sumthin, idk dude
    LCD_Draw_Pixel(207, 297, LCD_COLOR_LIGHTGRAY);
    LCD_Draw_Pixel(207, 303, LCD_COLOR_LIGHTGRAY);
}
