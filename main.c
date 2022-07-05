// Copyright 2018 Vaibhav Sharma. Released under the MIT license.

/*
        BEFORE COMPILING.

        SET YOUR CONSOLE FONT TO CONSOLAS.
        SET YOUR FONT SIZE IN BETWEEN 5 AND 16.
        LARGER THE FONT SIZE LARGER WILL BE YOUR SCREEN.
        YOU HAVE TO SET IT MANNUALLY IN COMMAND PROMPT.

*/

#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

#define SCREEN_WIDTH 120
#define SCREEN_HEIGHT 40

const int SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;
const int SCREEN_WIDTH_HALF = SCREEN_WIDTH / 2;
const int SCREEN_HEIGHT_HALF = SCREEN_HEIGHT / 2;

int eaten = 0;

enum DIRECTION {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Vec2D {
    float x, y;
} snake, speed, foodpos, tail[1000];

struct Vec2D directions[4] = {
    {0, 1},   // UP
    {0, -1},  // DOWN
    {-1, 0},  // LEFT
    {1, 0}    // RIGHT
};

unsigned short KeyEventProc(KEY_EVENT_RECORD ker);
void write(float x, float y, char *str, char *scr);
void plotLineHigh(float x0, float y0, float x1, float y1, char *scr, char p, float xoff, float yoff);
void plotLineLow(float x0, float y0, float x1, float y1, char *scr, char p, float xoff, float yoff);
void drawline(float x0, float y0, float x1, float y1, char *scr, char p, float xoff, float yoff);
void draw(float x, float y, char *scr, char p, float xoff, float yoff);
void clearscr(char *scr);
void getfood(char *scr, struct Vec2D *pos);
void pause(unsigned int wtime);

int main() {
    srand(time(NULL));
    system("color 3");
    system("mode con: cols=120 lines=40");
    char screen[SCREEN_SIZE];
    clearscr(screen);
    //==========================================SETTING UP CONSOLE BUFFER============================================================
    COORD coord = {0, 0};
    HANDLE buff = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
                                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                                            NULL,
                                            CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(buff);
    //==========================================VARIABLES FOR USER INPUT==============================================================
    DWORD cNumRead, fdwMode, i;
    INPUT_RECORD irInBuf[128];
    HANDLE hStdin;
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
    SetConsoleMode(hStdin, fdwMode);
    SetConsoleTitle("Snake Game BY: KV");
    unsigned short keyPressed;
    //=================================================================================================================================
    char q = 254;
    int len = 0;
    int hs = 0;
    int rate;
    char nscore[10];
    char hscore[10];
    int counter = 0;
    speed = directions[RIGHT];

    //===========================================================================================================
    while (1) {
        //=======================================READING CONSOLE INPUT=================================================
        if (kbhit()) {
            ReadConsoleInput(
                hStdin,      // input buffer handle
                irInBuf,     // buffer to read into
                128,         // size of read buffer
                &cNumRead);  // number of records read

            for (i = 0; i < cNumRead; i++) {
                if (irInBuf[i].EventType == KEY_EVENT) {
                    keyPressed = KeyEventProc(irInBuf[i].Event.KeyEvent);
                    if (keyPressed == VK_UP && speed.x != 0) {
                        speed = directions[UP];
                        break;
                    } else if (keyPressed == VK_DOWN && speed.x != 0) {
                        speed = directions[DOWN];
                        break;
                    } else if (keyPressed == VK_LEFT && speed.y != 0) {
                        speed = directions[LEFT];
                        break;
                    } else if (keyPressed == VK_RIGHT && speed.y != 0) {
                        speed = directions[RIGHT];
                        break;
                    }
                }
            }
        }
        //===========================================================================================================
        // getting food for snake.
        if (eaten == 0) getfood(screen, &foodpos);

        //====================================COLLISION DETECTION=====================================================

        // snake and food collision detection.
        if (snake.x == foodpos.x && snake.y == foodpos.y) {
            eaten = 0;
            len++;
        }

        // snake and tail collision detection.
        for (int i = 0; i < len; i++) {
            if (snake.x == tail[i].x && snake.y == tail[i].y) {
                printf("\a");
                pause(1800);
                printf("\a");
                len = 0;
            }
        }
        // boundary collision detection.
        snake.x = snake.x < -29 ? 28 : snake.x;
        snake.x = snake.x > 28 ? -29 : snake.x;
        snake.y = snake.y < -18 ? 17 : snake.y;
        snake.y = snake.y > 17 ? -18 : snake.y;

        //======================================SNAKE MOVEMENT UPDATE======================================================

        rate = 250 - 2 * (len / 2);  // as the snake length increses the speed  of the snake will also increase.

        if (counter % rate == 0) {
            for (int i = len; i > 0; i--) {
                tail[i] = tail[i - 1];
            }
            tail[0] = snake;
            snake.x += speed.x;
            snake.y += speed.y;
            counter = 0;
        }

        counter++;

        //=======================DRAWING EVERYTHING ON THE SCREEN=========================================================

        // food
        draw(foodpos.x, foodpos.y, screen, 234, 0, 0);

        // snake
        draw(snake.x, snake.y, screen, q, 0, 0);

        // snake tail
        for (int i = 0; i < len; i++) {
            draw(tail[i].x, tail[i].y, screen, q, 0, 0);
        }

        // game boundary
        drawline(-30, 20, 29, 20, screen, q, 0, 0);
        drawline(-30, -19, 29, -19, screen, q, 0, 0);
        drawline(-30, 18, 29, 18, screen, q, 0, 0);
        drawline(-30, 20, -30, -19, screen, q, 0, 0);
        drawline(29, 20, 29, -19, screen, q, 0, 0);

        // score
        hs = len > hs ? len : hs;
        sprintf(nscore, "%d", len);
        sprintf(hscore, "%d", hs);

        write(-28, 19, "SCORE:", screen);
        write(-22, 19, nscore, screen);
        write(15, 19, "HIGH SCORE:", screen);
        write(26, 19, hscore, screen);

        //===================================WRITING UPDATED SCREEN STRING IN SCREEN BUFFER===============================
        DWORD dwBytesWritten = 0;
        WriteConsoleOutputCharacter(buff, screen, SCREEN_SIZE, coord, &dwBytesWritten);
        clearscr(screen);
    }
    //---------------------------------------------LOOP ENDS-----------------------------------------------------
    return 0;
}
//========================DEFINING ALL FUNCTIONS===================================================

// FUNCTION FOR KEYBOARD EVENT HANDELING.
unsigned short KeyEventProc(KEY_EVENT_RECORD ker) {
    if (ker.bKeyDown) {
        return ker.wVirtualKeyCode;
    }
}
// FUNCTION FOR WRITING STRING ON SCREEN.
void write(float x, float y, char *str, char *scr) {
    for (int i = 0; str[i] != '\0'; i++) {
        draw(x + i, y, scr, str[i], 0, 0);
    }
}
// FUNCTION FOR GIVING RANDOM POSITION TO FOOD.
void getfood(char *scr, struct Vec2D *pos) {
    pos->x = -((rand() % 29)) + ((rand() % 29));
    pos->y = -((rand() % 19)) + ((rand() % 19));
    eaten = 1;
}

// FUNCTION FOR PAUSING IN GAME.
void pause(unsigned int wtime) {
    clock_t ptime = wtime + clock();
    while (ptime >= clock())
        ;
}
// FUNCTION FOR DRAWING A CHAR ON SCREEN. GIVEN ITS X ANY Y POSITION AND CHAR P.
void draw(float x, float y, char *scr, char p, float xoff, float yoff) {
    x = (int)(((x + xoff)) + SCREEN_WIDTH_HALF);
    y = (int)((-(y + yoff)) + SCREEN_HEIGHT_HALF);

    int drawon = SCREEN_WIDTH * y + x;

    if (drawon < SCREEN_SIZE && drawon >= 0) {
        scr[drawon] = p;
    }
}
// FUNCTION FOR CLEARING THE SCREEN.
void clearscr(char *scr) {
    for (int i = 0; i < SCREEN_SIZE; i++) {
        scr[i] = ' ';
    }
    scr[SCREEN_SIZE - 1] = '\0';
}

void plotLineLow(float x0, float y0, float x1, float y1, char *scr, char p, float xoff, float yoff) {
    float dx = x1 - x0;
    float dy = y1 - y0;
    float yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    float D = 2 * dy - dx;
    float y = y0;

    for (int x = x0; x <= x1; x++) {
        draw(x, y, scr, p, xoff, yoff);
        if (D > 0) {
            y = y + yi;
            D = D - 2 * dx;
        }
        D = D + 2 * dy;
    }
}

void plotLineHigh(float x0, float y0, float x1, float y1, char *scr, char p, float xoff, float yoff) {
    float dx = x1 - x0;
    float dy = y1 - y0;
    float xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    float D = 2 * dx - dy;
    float x = x0;

    for (int y = y0; y <= y1; y++) {
        draw(x, y, scr, p, xoff, yoff);
        if (D > 0) {
            x = x + xi;
            D = D - 2 * dy;
        }
        D = D + 2 * dx;
    }
}

void drawline(float x0, float y0, float x1, float y1, char *scr, char p, float xoff, float yoff) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            plotLineLow(x1, y1, x0, y0, scr, p, xoff, yoff);
        } else {
            plotLineLow(x0, y0, x1, y1, scr, p, xoff, yoff);
        }
    } else if (y0 > y1) {
        plotLineHigh(x1, y1, x0, y0, scr, p, xoff, yoff);
    } else {
        plotLineHigh(x0, y0, x1, y1, scr, p, xoff, yoff);
    }
}
