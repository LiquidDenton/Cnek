#include <stdio.h>
#include <stdlib.h>
#include "./constants.h"

typedef struct {
    int x, y; // coordinates
} Point;

typedef struct {
    Point parent;
    int f, g, h; // f = g + h
    short int obstacle, closed;
} Node;

int manhattan(Point start, Point end) {
    return abs(start.x - end.x) + abs(start.y - end.y);
}

void evalNode(Point start, Point end, Point current, Node* node) {
    node->g = manhattan(current, start);
    node->h = manhattan(current, end);
    node->f = node->g + node->h;
}

void openNode(Point start, Point end, Point parent, Point current, Node* node, Point* openList[]) {

    if (node->obstacle) {
        return;
    }

    node->parent = parent;

    short int openListSize = (short int) sizeof(**openList) / sizeof(*openList[0]);

    for (int i = 0; i < openListSize; i++) {
        if (openList[i]->x < 0 && openList[i]->y < 0) {
            *openList[i] = current;
            break;
        }
    }

    evalNode(start, end, current, node);

}

void findNeighbors(Point current, Point* openList[], Node* grid[GRID_SIZE][GRID_SIZE], Point start, Point end) {
    openNode(start, end, current, (Point) {current.x + 1, current.y}, grid[current.x + 1][current.y], openList);
}

// A* algorithm function
void astar(Node grid[GRID_SIZE][GRID_SIZE], Point start, Point end) {
    Point* openList[GRID_SIZE * GRID_SIZE];
    Point current = start;
}


void AIPath(int startX, int startY, int endX, int endY, Node grid[GRID_SIZE][GRID_SIZE]) {
    Point start = {startX, startY};
    Point end = {endX, endY};
    


}