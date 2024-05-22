#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./constants.h"
#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "./pathfinder.c"

SDL_Window* window;
SDL_Renderer* renderer;
short int gameIsRunning;
short int gamePaused;

short int inputBuffer[2];

Mix_Music *music;
Mix_Chunk *munch;
Mix_Chunk *gpws;

SDL_Rect menuRect;

struct Text {
    SDL_Color color;
    TTF_Font* font;
    SDL_Surface* message;
    SDL_Rect rect;
    SDL_Texture* texture;
};

struct Snek {
    Uint8 headPos[2];
    unsigned short int length;
    SDL_Rect rect;
    SDL_Surface *headSurface;
    SDL_Texture *headTexture;
    unsigned short int dir; 
};

struct TypeForMyStupidMath {
    Uint8 dir : 2;
};

typedef struct TypeForMyStupidMath typeForMyStupidMath;

struct Snek snek;
struct Text title;
struct Text menuText;
struct Text pointText;

int lastFrameTicks;

SDL_Texture* menuTexture;

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
};

struct Background background;

struct Apple apple;

struct WindowPos {
    short int x;
    short int y;
};

struct WindowPos windowPos;

Uint8 grid[GRID_SIZE][GRID_SIZE];

struct AI_Data {
    Uint8 flags;
    Uint8 grid[GRID_SIZE][GRID_SIZE];
    Uint8 AIDirs[GRID_SIZE * GRID_SIZE];
};

struct AI_Data AIData;

Node AIgrid[GRID_SIZE][GRID_SIZE];



unsigned int gpwsTime;

Uint8 AISquareValue(Uint8 x, Uint8 y, Uint8 targetX, Uint8 targetY) {
    if (x >= GRID_SIZE || y >= GRID_SIZE) {
        return 255;
    }
    if (!(AIData.grid[x][y] & AI_WALKABLE)) {
        return 255;
    }
    return abs((x - targetX)) + abs((y - targetY));
}

Uint8 AIDir(Uint8 x, Uint8 y, Uint8 targetX, Uint8 targetY) {

    // printf("foo\n");
    if (AIData.flags & AI_REPATH) {
        printf("Generating path\n");
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0;j < GRID_SIZE; j++) {
                AIgrid[i][j] = (Node) {(Point) {-1, -1}, 0, 0, 0, 0, 0};
                AIgrid[i][j].obstacle = grid[i][j] > 0;
            }
        }
        AIgrid[x][y].obstacle = 0;
        AIPath(x, y, targetX, targetY, AIgrid);
        printf("Path complete!\n");
    }


    //for (int i = 0; i < 4; i++) {
    //    if (snek.dir != 2 && i == 0 && min > AISquareValue(x + 1, y, targetX, targetY)) {
    //        min = AISquareValue(x + 1, y, targetX, targetY);
    //        bestDir.dir = 0;
    //    } else if (snek.dir != 3 && i == 1 && min > AISquareValue(x, y + 1, targetX, targetY)) {
    //        min = AISquareValue(x, y + 1, targetX, targetY);
    //        bestDir.dir = 1;
    //    } else if (snek.dir != 0 && i == 2 && min > AISquareValue(x - 1, y, targetX, targetY)) {
    //        min = AISquareValue(x - 1, y, targetX, targetY);
    //        bestDir.dir = 2;
    //    } else if (snek.dir != 1 && i == 3 && min > AISquareValue(x, y - 1, targetX, targetY)) {
    //        min = AISquareValue(x, y - 1, targetX, targetY);
    //        bestDir.dir = 3;
    //    }

    Point child = {targetX, targetY};
    Point current = child;
    int depth = 0;

    printf("Finding direction\n");

    while (depth <= MAX_DEPTH) {

        if (current.x == x && current.y == y) {
            break;
        }
        child = current;
        current = AIgrid[current.x][current.y].parent;
        depth++;
        printf("%d %d %d %d\n", current.x, current.y, child.x, child.y);
    }
    printf("Snek found!\n");



    if (current.x < child.x) {
        return 0;
    } else if (current.y < child.y) {
        return 1;
    } else if (current.x > child.x) {
        return 2;
    } else {
        return 3;
    }

}

short int initSDL(void) {
    if (0 != SDL_Init(SDL_INIT_EVERYTHING)) {
        perror("SDL init failed\n");
        return 0;
    }
    window = SDL_CreateWindow("Cnek", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_SIZE, WINDOW_SIZE, 0);
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

    title.font = TTF_OpenFont("./fonts/font.ttf", 50);
    title.color.r = 50;
    title.color.g = 50;
    title.color.b = 50;
    title.color.a = 255;

    title.message = TTF_RenderText_Blended(title.font, "Cnek", title.color);
    title.rect.h = 80;
    title.rect.w = 170;
    title.rect.x = WINDOW_SIZE / 2 - title.rect.w / 2;
    title.rect.y = WINDOW_SIZE / 2 - title.rect.h / 2;
    title.texture = SDL_CreateTextureFromSurface(renderer, title.message);
    SDL_FreeSurface(title.message);

    menuText.font = TTF_OpenFont("./fonts/font.ttf", 30);
    menuText.color.r = 50;
    menuText.color.g = 50;
    menuText.color.b = 50;
    menuText.color.a = 255;

    menuText.message = TTF_RenderText_Blended(menuText.font, "Press space to start", menuText.color);
    menuText.rect.h = 30;
    menuText.rect.w = 300;
    menuText.rect.x = WINDOW_SIZE / 2 - 150;
    menuText.rect.y = WINDOW_SIZE / 2 + 50;
    menuText.texture = SDL_CreateTextureFromSurface(renderer, menuText.message);
    SDL_FreeSurface(menuText.message);

    pointText.font = TTF_OpenFont("./fonts/font.ttf", 30);
    pointText.color.r = 50;
    pointText.color.g = 50;
    pointText.color.b = 50;
    pointText.color.a = 255;

    pointText.rect.h = 30;
    pointText.rect.w = 120;
    pointText.rect.x = 0;
    pointText.rect.y = 0;

    apple.texture = IMG_LoadTexture(renderer, "./apple.png");
    snek.headTexture = IMG_LoadTexture(renderer, "./head.png");
    menuTexture = IMG_LoadTexture(renderer, "./MenuTexture.png");
    SDL_Surface* processImage = IMG_Load("./apple.png");
    SDL_SetWindowIcon(window, processImage);
    SDL_FreeSurface(processImage);

    background.color1.r = 0;
    background.color1.g = 120;
    background.color1.b = 0;
    background.color1.a = 255;

    background.color2.r = 0;
    background.color2.g = 150;
    background.color2.b = 0;
    background.color2.a = 255;

    background.rect.h = SNEK_THICC;
    background.rect.w = SNEK_THICC;

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
    gpws = Mix_LoadWAV("./gpws.wav");

    Mix_PlayMusic(music, 32000);

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_GetWindowPosition(window, (int*) &windowPos.x, (int*) &windowPos.y);

    return 1;
}


void placeApple() {

    if (AIData.flags & AI_ENABLED) {
        AIData.flags |= AI_REPATH;
    }
    
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
        pointText.message = TTF_RenderText_Blended(pointText.font, renderPointsText, pointText.color);
        pointText.texture = SDL_CreateTextureFromSurface(renderer, pointText.message);

        break;
    }


}

void setup(void) {

    printf("%d\n", snek.length);

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

    if (title.font == NULL || menuText.font == NULL || pointText.font == NULL) {
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
                case SDLK_s:
                    if (snek.dir != 3 && inputBuffer[0] < 0) {
                        inputBuffer[0] = 1;
                    } else if (snek.dir != 3 && inputBuffer[1] < 0) {
                        inputBuffer[1] = 1;
                    }

                    break;
                case SDLK_UP:
		        case SDLK_w:
                    if (snek.dir != 1 && inputBuffer[0] < 0) {
                        inputBuffer[0] = 3;
                    } else if (snek.dir != 1 && inputBuffer[1] < 0) {
                        inputBuffer[1] = 3;
                    }
                    break;
                case SDLK_RIGHT:
		        case SDLK_d:
                    if (snek.dir != 2 && inputBuffer[0] < 0) {
                        inputBuffer[0] = 0;
                    } else if (snek.dir != 1 && inputBuffer[1] < 0) {
                        inputBuffer[1] = 0;
                    }
                    break;
                case SDLK_LEFT:
		        case SDLK_a:
                    if (snek.dir != 0 && inputBuffer[0] < 0) {
                        inputBuffer[0] = 2;
                    } else if (snek.dir != 1 && inputBuffer[1] < 0) {
                        inputBuffer[1] = 2;
                    }
                    break;
                case SDLK_z:
                    AIData.flags |= AI_ENABLED;
                    AIData.flags |= AI_REPATH;
                    break;
                case SDLK_x:
                    AIData.flags &= ~AI_ENABLED;
                    break;
                case SDLK_ESCAPE:
                    gamePaused = 1;
                    break;
                case SDLK_SPACE:
                    gamePaused = 0;
                    break;
            }
            break;
    /*  case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_MOVED) {
                SDL_GetWindowPosition(window, (int*) &windowPos.x, (int*) &windowPos.y);
            }*/
    }
}

void update() {
    if (SDL_TICKS_PASSED(SDL_GetTicks(), lastFrameTicks + 1000 / ((snek.length * SPEED_INCREASE_FACTOR) + SPEED_OFFSET)) && !gamePaused) {
        lastFrameTicks = SDL_GetTicks();

        struct TypeForMyStupidMath foo;
        foo.dir = snek.dir;
        foo.dir += 2;

        if (AIData.flags & AI_ENABLED) {
            snek.dir = AIDir(snek.headPos[0], snek.headPos[1], apple.pos[0], apple.pos[1]);
        } else if (inputBuffer[0] >= 0 && inputBuffer[0] != foo.dir) {
            snek.dir = inputBuffer[0];
        }



        inputBuffer[0] = inputBuffer[1];
        inputBuffer[1] = -1;
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

        if (snek.headPos[0] >= GRID_SIZE || snek.headPos[1] >= GRID_SIZE || grid[snek.headPos[0]][snek.headPos[1]] > 0) {
            setup();
            return;
        }
        grid[snek.headPos[0]][snek.headPos[1]] = snek.length + 1;
        if (snek.headPos[0] == apple.pos[0] && snek.headPos[1] == apple.pos[1]) {

            Uint8 currentPos[2];
            currentPos[0] = snek.headPos[0];
            currentPos[1] = snek.headPos[1];
            Uint8 stopScan;
            Uint8 endOfSnek = 0;

            int depth = 0;

            while (!endOfSnek && depth <= MAX_DEPTH) {
                depth++;
                stopScan = 0;
                for (int i = 0; i < 5 && !stopScan; i++) {
                    switch (i) {
                        case 0:
                            if (grid[currentPos[0] + 1][currentPos[1]] == grid[currentPos[0]][currentPos[1]] - 1 && grid[currentPos[0] + 1][currentPos[1]] > 0) {
                                grid[currentPos[0]][currentPos[1]] += 1;

                                currentPos[0] += 1;
                                stopScan = 1;
                                continue;
                            }
                        case 1:
                            if (grid[currentPos[0]][currentPos[1] + 1] == grid[currentPos[0]][currentPos[1]] - 1 && grid[currentPos[0]][currentPos[1] + 1] > 0) {
                                grid[currentPos[0]][currentPos[1]] += 1;
                                currentPos[1] += 1;
                                stopScan = 1;
                                continue;
                            }
                        case 2:
                            if (!currentPos[0]) {
                                continue;
                            }
                            if (grid[currentPos[0] - 1][currentPos[1]] == grid[currentPos[0]][currentPos[1]] - 1 && grid[currentPos[0] - 1][currentPos[1]] > 0) {
                                grid[currentPos[0]][currentPos[1]] += 1;
                                currentPos[0] -= 1;
                                stopScan = 1;
                                continue;
                            }
                        case 3:
                            if (!currentPos[1]) {
                                continue;
                            }
                            if (grid[currentPos[0]][currentPos[1] - 1] == grid[currentPos[0]][currentPos[1]] - 1 && grid[currentPos[0]][currentPos[1] - 1] > 0) {
                                grid[currentPos[0]][currentPos[1]] += 1;
                                currentPos[1] -= 1;
                                stopScan = 1;
                                continue;
                            }
                        case 4:
                            stopScan = 1;
                            endOfSnek = 1;
                            continue;
                    }
                } 
            }
            snek.length += 1;
            placeApple();
            Mix_PlayChannel(-1, munch, 0);
        }
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (grid[i][j] > 0) {
                    grid[i][j] -= 1;
                }
            }
        }

        // SDL_SetWindowPosition(window, windowPos.x + ((snek.headPos[0] - GRID_SIZE / 2) * -SNEK_THICC), windowPos.y + ((snek.headPos[1] - GRID_SIZE / 2) * -SNEK_THICC));

/*
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                printf("%d ", grid[j][i]);
            }
            printf("\n");
        } 
        printf("\n");
    */
        /*for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                printf("%d ", AISquareValue(i, j));
            }
            printf("\n");
        }
        printf("\n");*/
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

        Uint8 currentPos[2];
        currentPos[0] = snek.headPos[0];
        currentPos[1] = snek.headPos[1];
        Uint8 stopScan;
        Uint8 endOfSnek = 0;

        if (snek.headPos[0] == 0 || snek.headPos[0] == GRID_SIZE - 1 || snek.headPos[1] == 0 || snek.headPos[1] == GRID_SIZE - 1) {

            SDL_SetWindowPosition(window, windowPos.x + (rand() % 100 - 50), windowPos.y + (rand() % 100 - 50));

            if (SDL_GetTicks() >= gpwsTime) {
                Mix_PlayChannel(-1, gpws,0);
                gpwsTime = SDL_GetTicks() + 2000;
            }


        }

        int depth = 0;
        while (!endOfSnek && depth <= MAX_DEPTH) {
            depth++;
            stopScan = 0;
            for (int i = 0; i < 5 && !stopScan; i++) {
                switch (i) {
                    case 0:
                        if (grid[currentPos[0] + 1][currentPos[1]] == grid[currentPos[0]][currentPos[1]] - 1 && grid[currentPos[0] + 1][currentPos[1]] > 0) {
                            currentPos[0] += 1;
                            snek.rect.x = currentPos[0] * SNEK_THICC;
                            snek.rect.y = currentPos[1] * SNEK_THICC;
                            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                            SDL_RenderFillRect(renderer, &snek.rect);
                            stopScan = 1;
                            continue;
                        }
                    case 1:
                        if (grid[currentPos[0]][currentPos[1] + 1] == grid[currentPos[0]][currentPos[1]] - 1 && grid[currentPos[0]][currentPos[1] + 1] > 0) {
                            currentPos[1] += 1;
                            snek.rect.x = currentPos[0] * SNEK_THICC;
                            snek.rect.y = currentPos[1] * SNEK_THICC;
                            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                            SDL_RenderFillRect(renderer, &snek.rect);
                            stopScan = 1;
                            continue;
                        }
                    case 2:
                        if (!currentPos[0]) {
                            continue;
                        }
                        if (grid[currentPos[0] - 1][currentPos[1]] == grid[currentPos[0]][currentPos[1]] - 1 && grid[currentPos[0] - 1][currentPos[1]] > 0) {
                            currentPos[0] -= 1;
                            snek.rect.x = currentPos[0] * SNEK_THICC;
                            snek.rect.y = currentPos[1] * SNEK_THICC;
                            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                            SDL_RenderFillRect(renderer, &snek.rect);
                            stopScan = 1;
                            continue;
                        }
                    case 3:
                        if (!currentPos[1]) {
                            continue;
                        }
                        if (grid[currentPos[0]][currentPos[1] - 1] == grid[currentPos[0]][currentPos[1]] - 1 && grid[currentPos[0]][currentPos[1] - 1] > 0) {
                            currentPos[1] -= 1;
                            snek.rect.x = currentPos[0] * SNEK_THICC;
                            snek.rect.y = currentPos[1] * SNEK_THICC;
                            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                            SDL_RenderFillRect(renderer, &snek.rect);
                            stopScan = 1;
                            continue;
                        }
                    case 4:
                        stopScan = 1;
                        endOfSnek = 1;
                        continue;
                }
            } 
        }
        snek.rect.x = snek.headPos[0] * SNEK_THICC;
        snek.rect.y = snek.headPos[1] * SNEK_THICC;
        SDL_RenderCopyEx(renderer, snek.headTexture, 0, &snek.rect, 90 + (90 * snek.dir), NULL, 0);

    } else {
        SDL_RenderCopy(renderer, menuTexture, 0, &menuRect);
    }

    SDL_RenderCopy(renderer, pointText.texture, NULL, &pointText.rect);

    if (gamePaused) {
        SDL_Rect foo = menuText.rect;
        
        foo.y += (int) (sin((double) SDL_GetTicks() / MENU_INVERTED_BOUNCESPEED) * MENU_BOUNCEAMOUNT);

        SDL_RenderCopy(renderer, title.texture, NULL, &title.rect);
        SDL_RenderCopy(renderer, menuText.texture, NULL, &foo);
    }
    SDL_RenderPresent(renderer);
}

void destroyWindow(void) {
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(snek.headTexture);
    SDL_DestroyTexture(apple.texture);
    SDL_DestroyTexture(menuText.texture);
    SDL_DestroyTexture(title.texture);
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
        int cycleStartTime = SDL_GetTicks();
        processInput();
        update();
        render();
        if (FRAME_TIME)  {
                SDL_Delay(FRAME_TIME);
        }
    }

    destroyWindow();

    return 0;
}
