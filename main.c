// Copyright 2018 Vaibhav Sharma. Released under the MIT license.

/*
        BEFORE COMPILING.

        SET YOUR CONSOLE FONT TO CONSOLAS.
        FONT SIZE TO 15.
        SCREEN WIDTH TO 120 (screenWidth).
        SCREEN HEIGHT TO 40 (screenHeight).
*/

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <conio.h>

struct Vec2D{float x,y;}snake, speed, foodpos, tail[1000];

unsigned short KeyEventProc(KEY_EVENT_RECORD ker);
void write(float x, float y, char *str, char *scr);
void draw(float x, float y, char *scr, char p, float xoff, float yoff);
void clearscr(char *scr);
void move(char dir);
void getfood(char *scr, struct Vec2D* pos);
void pause(unsigned int wtime);

int screenWidth = 120;
int screenHeight = 40;

int eaten = 0;

int main(){
    srand(time(NULL));
    system("color 3");
    system("mode con: cols=120 lines=40");
    char screen[screenWidth*screenHeight];
    clearscr(screen);
//==========================================SETTING UP CONSOLE BUFFER============================================================
    COORD coord; coord.X = 0; coord.Y = 0;
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
    SetConsoleTitle("Snake Game @kVaibhav");
    unsigned short  keyPressed;
//=================================================================================================================================
    char q = 254;
    int len = 0;
    int hs = 0;
    char nscore[10];
    char hscore[10];
    int counter = 0;
    speed.x = 1.0;
    speed.y = 0.0;
//===========================================================================================================
    while(1){
//=======================================READING CONSOLE INPUT=================================================
        if(kbhit()){
            ReadConsoleInput(
                hStdin,      // input buffer handle
                irInBuf,     // buffer to read into
                128,         // size of read buffer
                &cNumRead);  // number of records read

            for (i = 0; i < cNumRead; i++){
                if(irInBuf[i].EventType == KEY_EVENT){
                    keyPressed = KeyEventProc(irInBuf[i].Event.KeyEvent);
                    if(keyPressed == VK_UP && speed.x != 0 && speed.y != -1){move('U');}
                    if(keyPressed == VK_DOWN && speed.x != 0 && speed.y != 1){move('D');}
                    if(keyPressed == VK_RIGHT && speed.x != -1 && speed.y != 0){move('R');}
                    if(keyPressed == VK_LEFT && speed.x != 1 && speed.y != 0){move('L');}
                }
            }
        }
//===========================================================================================================
        // getting food for snake.
        if(eaten == 0){
            getfood(screen, &foodpos);
        }
//====================================COLLISION DETECTION=====================================================

        // snake and food collision detection.
        if(snake.x == foodpos.x && snake.y == foodpos.y){
            //printf("\a");
            eaten = 0;
            len++;
        }
        // snake and tail collision detection.
        for(int i =0; i<len; i++){
            if(snake.x == tail[i].x && snake.y == tail[i].y){
                pause(1000);
                len = 0;
            }
        }
        // boundary collision detection.
        if(snake.x < -29){snake.x = 28;}
        if(snake.x > 28){snake.x = -29;}
        if(snake.y < -18){snake.y = 17;}
        if(snake.y > 17){snake.y = -18;}
//======================================SNAKE MOVEMENT=================================================
        if(counter%100==0){
            for(int i = len; i >= 0; i--){
                if(i>0){
                    tail[i].x = tail[i-1].x;
                    tail[i].y = tail[i-1].y;
                }else{
                    tail[i].x = snake.x;
                    tail[i].y = snake.y;
                }
            }
            snake.x += speed.x;
            snake.y += speed.y;
            counter = 0;
        }
        counter++;
//=======================DRAWING EVERYTHING ON THE SCREEN================================================
        // food
        draw(foodpos.x, foodpos.y, screen, 234, 0, 0);
        // snake
        draw(snake.x, snake.y, screen, q, 0, 0);
        // snake tail
        for(int i=0; i<len; i++){
            draw(tail[i].x, tail[i].y, screen, q, 0, 0);
        }
        // game boundary
        for(int i=-30;i<30;i++){
            draw(i,20,screen,q,0,0);
            draw(i,-19,screen,q,0,0);
        }
        for(int i=-30;i<30;i++){
            draw(i,18,screen,q,0,0);
        }
        for(int i=-19;i<=20;i++){
            draw(-30,i,screen,q,0,0);
            draw(29,i,screen,q,0,0);
        }
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
        WriteConsoleOutputCharacter(buff, screen, screenWidth*screenHeight, coord, &dwBytesWritten);
        clearscr(screen);
    }
//---------------------------------------------LOOP ENDS-----------------------------------------------------
    return 0;
}
//========================DEFINING ALL FUNCTIONS===================================================

// FUNCTION FOR KEYBOARD EVENT HANDELING.
unsigned short KeyEventProc(KEY_EVENT_RECORD ker){
    if(ker.bKeyDown){
        return ker.wVirtualKeyCode;
    }
}
// FUNCTION FOR WRITING STRING ON SCREEN.
void write(float x, float y, char *str, char *scr){
    for(int i = 0; str[i] != '\0'; i++){
        draw(x+i, y, scr, str[i], 0, 0);
    }
}
// FUNCTION FOR GIVING RANDOM POSITION TO FOOD.
void getfood(char *scr, struct Vec2D* pos){
    pos->x = -((rand()%29))+((rand()%29));
    pos->y = -((rand()%19))+((rand()%19));
    eaten = 1;
}
// FUNCTION FOR CHANDING DIRECTION OF MOVEMENT.
void move(char dir){
    switch(dir){
        case 'U':
            speed.x = 0.0f;
            speed.y = 1.0f;
        break;
        case 'D':
            speed.x = 0.0f;
            speed.y = -1.0f;
        break;
        case 'L':
            speed.x = -1.0f;
            speed.y = 0.0f;
        break;
        case 'R':
            speed.x = 1.0f;
            speed.y = 0.0f;
        break;
    }
}
// FUNCTION FOR PAUSING IN GAME.
void pause(unsigned int wtime){
    clock_t ptime = wtime + clock();
    while(ptime >= clock());
}
// FUNCTION FOR DRAWING A CHAR ON SCREEN. GIVEN ITS X ANY Y POSITION AND CHAR P.
void draw(float x, float y, char *scr, char p, float xoff, float yoff){
    x += xoff;
    y += yoff;
    x = 2*x;
    y = -y;

    int drawon = (((int)y+screenHeight/2) * screenWidth) + ((int)x+screenWidth/2);

    if(drawon < screenWidth*screenHeight && drawon >= 0){
        scr[drawon] = p;
    }
}
// FUNCTION FOR CLEARING THE SCREEN.
void clearscr(char *scr){
    int j;
    for(j=0;j<screenWidth*screenHeight;j++){
        scr[j] = ' ';
    }
    scr[screenWidth*screenHeight-1] = '\0';
}
