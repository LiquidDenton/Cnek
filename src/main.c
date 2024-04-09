#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./constants.h"
#include <time.h>
#include <stdlib.h>

SDL_Window* window;
SDL_Renderer* renderer;
short int gameIsRunning;
short int gamePaused;

short int applePos[2];

struct Text {
    SDL_Color color;
    TTF_Font* font;
    SDL_Surface* message;
    SDL_Rect rect;
    SDL_Texture* texture;
};

struct Snek {
    short int headPos[2];
    short int length;
    short int dir;
    SDL_Rect rect;
};

struct Snek snek;
struct Text loserText;

int lastFrameTicks;

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
    TTF_Init();
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

    snek.rect.h = SNEK_THICC;
    snek.rect.w = SNEK_THICC;
    apple.h = SNEK_THICC;
    apple.w = SNEK_THICC;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 0;
        }
    }

    loserText.font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 24);
    loserText.color.r = 150;
    loserText.color.g = 150;
    loserText.color.b = 150;
    loserText.color.a = 255;

    loserText.message = TTF_RenderText_Solid(loserText.font, "L", loserText.color);
    loserText.rect.h = 80;
    loserText.rect.w = 50;
    loserText.rect.x = WINDOW_SIZE / 2;
    loserText.rect.y = WINDOW_SIZE / 2;
    loserText.texture = SDL_CreateTextureFromSurface(renderer, loserText.message);

    snek.dir = 3;

    snek.length = 2;

    lastFrameTicks = SDL_GetTicks();

    snek.headPos[0] = GRID_SIZE / 2;
    snek.headPos[1] = GRID_SIZE / 2;

    srand(time(NULL));

    placeApple();

    gamePaused = 1;

    if (loserText.font == NULL) {
        exit(1);
    }
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
                    if (snek.dir != 3) {
                    snek.dir = 1;
                    }
                    break;
                case SDLK_UP:
                    if (snek.dir != 1) {
                    snek.dir = 3;
                    }
                    break;
                case SDLK_RIGHT:
                    if (snek.dir != 2) {
                    snek.dir = 0;
                    }
                    break;
                case SDLK_LEFT:
                    if (snek.dir != 0) {
                    snek.dir = 2;
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
        switch (snek.dir) {
            case 0:
                snek.headPos[0] += 1;
                break;
            case 1: 
                snek.headPos[1] += 1;
                break;
            case 2:
                snek.headPos[0] -= 1;
                break;
            case 3: 
                snek.headPos[1] -= 1;
                break;
        }

        // printf("x: %d y: %d\n", snek.headPos[0], snek.headPos[1]);

        if (snek.headPos[0] >= GRID_SIZE || snek.headPos[1] >= GRID_SIZE || snek.headPos[0] < 0 || snek.headPos[1] < 0 || grid[snek.headPos[0]][snek.headPos[1]] > 0) {
            printf("\nL\n");
            setup();
            return;
        }
        grid[snek.headPos[0]][snek.headPos[1]] = snek.length + 1;
        if (snek.headPos[0] == applePos[0] && snek.headPos[1] == applePos[1]) {
            for (int i = 0; i < GRID_SIZE; i++) {
                for (int j = 0; j < GRID_SIZE; j++) {
                    if (grid[i][j] > 0) {
                        grid[i][j] += 1;
                    }
                }
            }
            placeApple();
            snek.length += 1;
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
                snek.rect.x = i * SNEK_THICC;
                snek.rect.y = j * SNEK_THICC;
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderFillRect(renderer, &snek.rect);
            }
        }
    }




    if (gamePaused) {
        SDL_RenderCopy(renderer, loserText.texture, NULL, &loserText.rect);
    }
    SDL_RenderPresent(renderer);
}

void destroyWindow(void) {
    SDL_FreeSurface(loserText.message);
    SDL_DestroyTexture(loserText.texture);
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