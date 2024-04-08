#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "./constants.h"

SDL_Window* window;
SDL_Renderer* renderer;
short int gameIsRunning;

SDL_Rect snek[150];
short int snekHeadPos[2];
short int prevSnekHeadPos[2];
short int snekLength;
 
int lastFrameTicks;

short int snekDir;

SDL_Rect apple;

short int gridSize;

short int initSDL(void) {
    if (0 != SDL_Init(SDL_INIT_EVERYTHING)) {
        perror("SDL init failed\n");
        return 0;
    }
    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE, SDL_WINDOW_BORDERLESS);
    if (!window) {
        perror("Error initialising window\n");
        return 0;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        perror("Error initialising SDL renderer\n");
        return 0;
    }
    return 1;

}

void setup(void) {
    for (short int i = 0; i < 150; i++) {
        snek[i].h = SNEK_THICC;
        snek[i].w = SNEK_THICC;
        snek[i].x = WINDOW_SIZE / 2;
        snek[i].y = WINDOW_SIZE / 2;     
    }
    apple.h = SNEK_THICC;
    apple.w = SNEK_THICC;

    snekDir = 0;

    gridSize = WINDOW_SIZE / SNEK_THICC;

    lastFrameTicks = SDL_GetTicks();
}

void processInput() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT: 
            gameIsRunning = 0;
            break; 
    }
}

void update() {
    if (SDL_TICKS_PASSED(SDL_GetTicks(), lastFrameTicks + DESIRED_FRAME_TIME)){
        lastFrameTicks = SDL_GetTicks();
        switch (snekDir) {
            case 0:
                snek[0].y += 1;
                break;
            case 1: 
                snek[0].y -= 1;
                break;
            case 3:
                snek[0].x += 1;
                break;
            case 4: 
                snek[0].x -= 1;
                break;
        }
    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &snek[0]);

    SDL_RenderPresent(renderer);
}

void destroyWindow(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {

    gameIsRunning = initSDL();

    setup();

    while (gameIsRunning) {
        processInput();
        update();
        render();
    }

    destroyWindow();

    return 0;
}