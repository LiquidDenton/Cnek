#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./constants.h"
#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

SDL_Window* window;
SDL_Renderer* renderer;
short int gameIsRunning;
short int gamePaused;
short int inputLock;

Mix_Music *music;
Mix_Chunk *munch;

SDL_Rect menuRect;


struct Text {
    SDL_Color color;
    TTF_Font* font;
    SDL_Surface* message;
    SDL_Rect rect;
    SDL_Texture* texture;
};

struct Snek {
    short int headPos[2];
    unsigned short int length;
    unsigned short int dir;
    SDL_Rect rect;
    SDL_Surface *headSurface;
    SDL_Texture *headTexture;
};

struct Snek snek;
struct Text loserText;
struct Text menuText;
struct Text pointText;

int lastFrameTicks;

struct Apple {
    SDL_Rect rect;
    SDL_Texture* texture;
    short int pos[2];
};

struct Background {
    SDL_Color color1;
    SDL_Color color2;
    SDL_Rect rect;
    SDL_Texture* texture;
    SDL_PixelFormat pixelFormat;
    SDL_Rect ioRect;
};

struct Background background;

struct Apple apple;

short int grid[GRID_SIZE][GRID_SIZE];

short int initSDL(void) {
    if (0 != SDL_Init(SDL_INIT_EVERYTHING)) {
        perror("SDL init failed\n");
        return 0;
    }
    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE, SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);
    if (!window) {
        perror("Error initialising window\n");
        return 0;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        perror("Error initialising SDL renderer\n");
        return 0;
    }
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    Mix_Init(MIX_INIT_MP3);

    if (!SDL_SetHint("SDL_RENDER_SCALE_QUALITY", "2")) {
        exit(1);
    }

    loserText.font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 24);
    loserText.color.r = 150;
    loserText.color.g = 150;
    loserText.color.b = 150;
    loserText.color.a = 255;

    loserText.message = TTF_RenderText_Solid(loserText.font, "L", loserText.color);
    loserText.rect.h = 80;
    loserText.rect.w = 50;
    loserText.rect.x = WINDOW_SIZE / 2 - 30;
    loserText.rect.y = WINDOW_SIZE / 2 - 30;
    loserText.texture = SDL_CreateTextureFromSurface(renderer, loserText.message);

    menuText.font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 15);
    menuText.color.r = 150;
    menuText.color.g = 150;
    menuText.color.b = 150;
    menuText.color.a = 255;

    menuText.message = TTF_RenderText_Solid(menuText.font, "Press space to continue", menuText.color);
    menuText.rect.h = 30;
    menuText.rect.w = 300;
    menuText.rect.x = WINDOW_SIZE / 2 - 150;
    menuText.rect.y = WINDOW_SIZE / 2 + 50;
    menuText.texture = SDL_CreateTextureFromSurface(renderer, menuText.message);

    pointText.font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 15);
    pointText.color.r = 150;
    pointText.color.g = 150;
    pointText.color.b = 150;
    pointText.color.a = 255;

    pointText.rect.h = 15;
    pointText.rect.w = 60;
    pointText.rect.x = 0;
    pointText.rect.y = 0;

    apple.texture = IMG_LoadTexture(renderer, "./apple.png");
    snek.headTexture = IMG_LoadTexture(renderer, "./head.png");

    background.color1.r = 0;
    background.color1.g = 0;
    background.color1.b = 100;
    background.color1.a = 255;

    background.color2.r = 0;
    background.color2.g = 0;
    background.color2.b = 50;
    background.color2.a = 255;

    background.rect.h = SNEK_THICC;
    background.rect.w = SNEK_THICC;

    background.ioRect.h = WINDOW_SIZE;
    background.ioRect.w = WINDOW_SIZE;
    background.ioRect.x = 0;
    background.ioRect.y = 0;

    background.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, WINDOW_SIZE, WINDOW_SIZE);



    SDL_SetRenderTarget(renderer, background.texture);

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            background.rect.x = i * SNEK_THICC;
            background.rect.y = j * SNEK_THICC;

            if (i % 2) {
                if (j % 2) {
                    SDL_SetRenderDrawColor(renderer, background.color1.r, background.color1.g, background.color1.b, 255);
                    SDL_RenderFillRect(renderer, &background.rect);
                    continue;
                }
                SDL_SetRenderDrawColor(renderer, background.color2.r, background.color2.g, background.color2.b, 255);
                SDL_RenderFillRect(renderer, &background.rect);
                continue;
            }
            if (j % 2) {
                SDL_SetRenderDrawColor(renderer, background.color2.r, background.color2.g, background.color2.b, 255);
                SDL_RenderFillRect(renderer, &background.rect);
                continue;
            }
            SDL_SetRenderDrawColor(renderer, background.color1.r, background.color1.g, background.color1.b, 255);
            SDL_RenderFillRect(renderer, &background.rect);
        }

        menuRect.h = SNEK_THICC * 8;
        menuRect.w = SNEK_THICC * 8;
        menuRect.x = (WINDOW_SIZE / 2) - (menuRect.h / 2);
        menuRect.y = 20;



    }
    SDL_SetRenderTarget(renderer, NULL);

    srand(time(NULL));

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

    music = Mix_LoadMUS("./music.mp3");
    munch = Mix_LoadWAV("./munch.wav");

    Mix_PlayMusic(music, 32000);

    return 1;
}

void placeApple() {
    
    while (1) {
        apple.pos[0] = rand() % GRID_SIZE;
        apple.pos[1] = rand() % GRID_SIZE;

        if (grid[apple.pos[0]][apple.pos[1]] > 0) {
            continue;
        }

        SDL_DestroyTexture(pointText.texture);
        char PointMessage[] = "Length:";
        char renderPointsText[12];
        snprintf(renderPointsText, 12, "%s %u", PointMessage, snek.length);
        pointText.message = TTF_RenderText_Solid(pointText.font, renderPointsText, pointText.color);
        pointText.texture = SDL_CreateTextureFromSurface(renderer, pointText.message);

        break;
    }


}

void setup(void) {

    snek.rect.h = SNEK_THICC;
    snek.rect.w = SNEK_THICC;
    apple.rect.h = SNEK_THICC;
    apple.rect.w = SNEK_THICC;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 0;
        }
    }

    snek.dir = 3;

    snek.length = 2;

    lastFrameTicks = SDL_GetTicks();

    snek.headPos[0] = GRID_SIZE / 2;
    snek.headPos[1] = GRID_SIZE / 2;
    placeApple();
    gamePaused = 1;

    if (loserText.font == NULL || menuText.font == NULL || pointText.font == NULL) {
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
                    if (snek.dir != 3 && !inputLock) {
                    snek.dir = 1;
                    inputLock = 1;
                    }
                    break;
                case SDLK_UP:
                    if (snek.dir != 1 && !inputLock) {
                    snek.dir = 3;
                    inputLock = 1;
                    }
                    break;
                case SDLK_RIGHT:
                    if (snek.dir != 2 && !inputLock) {
                    snek.dir = 0;
                    inputLock = 1;
                    }
                    break;
                case SDLK_LEFT:
                    if (snek.dir != 0 && !inputLock) {
                    snek.dir = 2;
                    inputLock = 1;
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
    if (SDL_TICKS_PASSED(SDL_GetTicks(), lastFrameTicks + 1000 / ((snek.length * SPEED_INCREASE_FACTOR) + SPEED_OFFSET)) && !gamePaused) {
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
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (grid[i][j] > 0) {
                    grid[i][j] -= 1;
                }
            }
        }

        // printf("x: %d y: %d\n", snek.headPos[0], snek.headPos[1]);

        if (snek.headPos[0] >= GRID_SIZE || snek.headPos[1] >= GRID_SIZE || snek.headPos[0] < 0 || snek.headPos[1] < 0 || grid[snek.headPos[0]][snek.headPos[1]] > 0) {
            setup();
            return;
        }
        grid[snek.headPos[0]][snek.headPos[1]] = snek.length;
        if (snek.headPos[0] == apple.pos[0] && snek.headPos[1] == apple.pos[1]) {
            for (int i = 0; i < GRID_SIZE; i++) {
                for (int j = 0; j < GRID_SIZE; j++) {
                    if (grid[i][j] > 0) {
                        grid[i][j] += 1;
                    }
                }
            }
            snek.length += 1;
            placeApple();
            Mix_PlayChannel(-1, munch, 0);
        }




        inputLock = 0;

    }





}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, background.texture, 0, 0);

    if (!gamePaused) {
    
        apple.rect.x = apple.pos[0] * SNEK_THICC;
        apple.rect.y = apple.pos[1] * SNEK_THICC;
        SDL_RenderCopy(renderer, apple.texture, 0, &apple.rect);

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
        snek.rect.x = snek.headPos[0] * SNEK_THICC;
        snek.rect.y = snek.headPos[1] * SNEK_THICC;
        SDL_RenderCopy(renderer, snek.headTexture, 0, &snek.rect);

    } else {
        SDL_RenderCopy(renderer, snek.headTexture, 0, &menuRect);
    }

    SDL_RenderCopy(renderer, pointText.texture, NULL, &pointText.rect);

    if (gamePaused) {
        SDL_RenderCopy(renderer, loserText.texture, NULL, &loserText.rect);
        SDL_RenderCopy(renderer, menuText.texture, NULL, &menuText.rect);
    }
    SDL_RenderPresent(renderer);
}

void destroyWindow(void) {
    SDL_DestroyTexture(apple.texture);
    SDL_FreeSurface(menuText.message);
    SDL_DestroyTexture(menuText.texture);
    SDL_FreeSurface(loserText.message);
    SDL_DestroyTexture(loserText.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    Mix_Quit();
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