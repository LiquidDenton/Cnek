#define WINDOW_SIZE (SNEK_THICC * GRID_SIZE)
#define SNEK_THICC 32
#define GRID_SIZE 20

#define MENU_BOUNCEAMOUNT 5
#define MENU_INVERTED_BOUNCESPEED 200

#define SPEED_INCREASE_FACTOR 0.05
#define SPEED_OFFSET 50

#define FPS 60
#define FRAME_TIME (1000/FPS)

#define AI_ENABLED 0x01
#define AI_REPATH 0x02
#define AI_PATH_COMPLETE 0x03

#define AI_WALKABLE 0x01
#define AI_EXPLORED 0x02
#define AI_DIR 0x0C

#define RIGHT 0
#define DOWN 1
#define LEFT 2
#define UP 3

#define MAX_DEPTH (GRID_SIZE * GRID_SIZE)