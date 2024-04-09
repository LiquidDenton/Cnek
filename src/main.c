#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "./constants.h"
#include <time.h>
#include <stdlib.h>

SDL_Window* window;
SDL_Renderer* renderer;
short int gameIsRunning;
short int gamePaused;

short int snekHeadPos[2];
short int snekLength;

short int applePos[2];
 
int lastFrameTicks;

short int snekDir;

SDL_Rect snekRect;

SDL_Rect apple;

short int grid[GRID_SIZE][GRID_SIZE];

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

void placeApple() {
    
    while (1) {
        applePos[0] = rand() % GRID_SIZE;
        applePos[1] = rand() % GRID_SIZE;

        if (grid[applePos[0]][applePos[1]] > 0) {
            continue;
        }
        break;
    }


}

void setup(void) {
    snekRect.h = SNEK_THICC;
    snekRect.w = SNEK_THICC;
    apple.h = SNEK_THICC;
    apple.w = SNEK_THICC;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 0;
        }
    }

    snekDir = 3;

    snekLength = 2;

    lastFrameTicks = SDL_GetTicks();

    snekHeadPos[0] = GRID_SIZE / 2;
    snekHeadPos[1] = GRID_SIZE / 2;

    srand(time(NULL));

    placeApple();

    gamePaused = 1;
}

void processInput() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT: 
            gameIsRunning = 0;
            break; 
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_DOWN:
                    if (snekDir != 3) {
                    snekDir = 1;
                    }
                    break;
                case SDLK_UP:
                    if (snekDir != 1) {
                    snekDir = 3;
                    }
                    break;
                case SDLK_RIGHT:
                    if (snekDir != 2) {
                    snekDir = 0;
                    }
                    break;
                case SDLK_LEFT:
                    if (snekDir != 0) {
                    snekDir = 2;
                    }
                    break;
                case SDLK_ESCAPE:
                    gamePaused = 1;
                    break;
                case SDLK_SPACE:
                    gamePaused = 0;
                    break;
            }
            break;
    }
}

void update() {
    if (SDL_TICKS_PASSED(SDL_GetTicks(), lastFrameTicks + DESIRED_FRAME_TIME) && !gamePaused) {
        lastFrameTicks = SDL_GetTicks();
        switch (snekDir) {
            case 0:
                snekHeadPos[0] += 1;
                break;
            case 1: 
                snekHeadPos[1] += 1;
                break;
            case 2:
                snekHeadPos[0] -= 1;
                break;
            case 3: 
                snekHeadPos[1] -= 1;
                break;
        }

        // printf("x: %d y: %d\n", snekHeadPos[0], snekHeadPos[1]);

        if (snekHeadPos[0] >= GRID_SIZE || snekHeadPos[1] >= GRID_SIZE || snekHeadPos[0] < 0 || snekHeadPos[1] < 0 || grid[snekHeadPos[0]][snekHeadPos[1]] > 0) {
            printf("\nL\n");
            setup();
            return NULL;
        }
        grid[snekHeadPos[0]][snekHeadPos[1]] = snekLength + 1;
        if (snekHeadPos[0] == applePos[0] && snekHeadPos[1] == applePos[1]) {
            for (int i = 0; i < GRID_SIZE; i++) {
                for (int j = 0; j < GRID_SIZE; j++) {
                    if (grid[i][j] > 0) {
                        grid[i][j] += 1;
                    }
                }
            }
            placeApple();
            snekLength += 1;
        }



        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (grid[i][j] > 0) {
                    grid[i][j] -= 1;
                }
            }
        }

    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    apple.x = applePos[0] * SNEK_THICC;
    apple.y = applePos[1] * SNEK_THICC;
    SDL_RenderFillRect(renderer, &apple);

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] > 0) {
                snekRect.x = i * SNEK_THICC;
                snekRect.y = j * SNEK_THICC;
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderFillRect(renderer, &snekRect);
            }
        }
    }



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