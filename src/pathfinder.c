#include <stdio.h>
#include <stdlib.h>
#include "./constants.h"

#define ROWS GRID_SIZE
#define COLS GRID_SIZE
#define INF 999999

typedef struct {
    int x, y; // coordinates
} Point;

typedef struct {
    Point parent;
    double f, g, h; // f = g + h
    int obstacle;
} Node;

// A* algorithm function
void astar(Node grid[ROWS][COLS], Point start, Point end) {
    int i, j;
    Node* current;
    Node* openList[ROWS*COLS];
    int openListCount = 0;

    // Initialize the open list
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            grid[i][j].f = INF;
            grid[i][j].g = INF;
            grid[i][j].h = INF;
            grid[i][j].parent.x = -1;
            grid[i][j].parent.y = -1;
        }
    }

    // Add the starting node to the open list
    openList[openListCount++] = &grid[start.x][start.y];
    grid[start.x][start.y].f = 0;
    grid[start.x][start.y].g = 0;
    grid[start.x][start.y].h = 0;

    while (openListCount > 0) {
        // Find the node with the lowest F value in the open list
        int lowest = 0;
        for (i = 0; i < openListCount; i++) {
            if (openList[i]->f < openList[lowest]->f) {
                lowest = i;
            }
        }
        current = openList[lowest];

        // Check if we've reached the end
        if (current->parent.x == end.x && current->parent.y == end.y) {
            return;
        }

        // Remove the current node from the open list
        for (i = lowest; i < openListCount - 1; i++) {
            openList[i] = openList[i + 1];
        }
        openListCount--;

        // Get neighbors
        Point neighbors[] = {
            {current->parent.x - 1, current->parent.y},
            {current->parent.x + 1, current->parent.y},
            {current->parent.x, current->parent.y - 1},
            {current->parent.x, current->parent.y + 1}
        };

        // Loop through neighbors
        for (i = 0; i < 4; i++) {
            int x = neighbors[i].x;
            int y = neighbors[i].y;

            // Check if neighbor is within bounds
            if (x >= 0 && x < ROWS && y >= 0 && y < COLS) {
                Node* neighbor = &grid[x][y];

                // Check if neighbor is not an obstacle and not closed
                if (!neighbor->obstacle) {
                    double tentative_g = current->g + 1; // Assuming uniform cost for simplicity

                    // Check if this path to neighbor is better than any previous one
                    if (tentative_g < neighbor->g) {
                        neighbor->parent.x = current->parent.x;
                        neighbor->parent.y = current->parent.y;
                        neighbor->g = tentative_g;
                        neighbor->h = abs(x - end.x) + abs(y - end.y); // Manhattan distance heuristic
                        neighbor->f = neighbor->g + neighbor->h;

                        // Add neighbor to open list if not already there
                        int inOpenList = 0;
                        for (j = 0; j < openListCount; j++) {
                            if (openList[j]->parent.x == neighbor->parent.x &&
                                openList[j]->parent.y == neighbor->parent.y) {
                                inOpenList = 1;
                                break;
                            }
                        }
                        if (!inOpenList) {
                            openList[openListCount++] = neighbor;
                        }
                    }
                }
            }
        }
    }
    printf("path failed");
}


void AIPath(int startX, int startY, int endX, int endY, Node grid[GRID_SIZE][GRID_SIZE]) {
    Point start = {startX, startY};
    Point end = {endX, endY};
    
    astar(grid, start, end);

}