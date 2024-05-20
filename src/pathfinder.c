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

// manhattan distance function
int manhattan(Point start, Point end) {
    return abs(start.x - end.x) + abs(start.y - end.y);
}

// evaluate value of node using distance to start and end points
void evalNode(Point start, Point end, Point current, Node* node) {
    node->g = manhattan(current, start);
    node->h = manhattan(current, end);
    node->f = node->g + node->h;
}

// call evaluate the node and add it to the openlist
void openNode(Point start, Point end, Point parent, Point current, Node* node, Point* openList[]) {

    // set nodes parent

    node->parent = parent;

    // find size of openList

    short int openListSize = GRID_SIZE * GRID_SIZE;

    // find a empty slot in the openList and add node there

    for (int i = 0; i < openListSize; i++) {
        if (openList[i]->x < 0 && openList[i]->y < 0) {
            *openList[i] = current;
            break;
        }
    }

    // evaluate the node

    evalNode(start, end, current, node);

}

void closeNode(Node* node, Point nodePoint, Point* openList[]) {

    // find size of openList

    short int openListSize = GRID_SIZE * GRID_SIZE;

    // remove node from the openList

    for (int i = 0; i < openListSize; i++) {

        // node found; remove and exit
        if (nodePoint.x == openList[i] -> x && nodePoint.y == openList[i] -> y) {
            *openList[i] = (Point) {-1, -1};
            break;
        }
    }

    // set nodes state to closed

    node -> closed = 1;

}

void findNeighbors(Point current, Point* openList[], Node grid[GRID_SIZE][GRID_SIZE], Point start, Point end) {

    // open the node in each direction if the node is not closed and not an obstacle

    if (!grid[current.x + 1][current.y].closed && !grid[current.x + 1][current.y].obstacle) {
        openNode(start, end, current, (Point) {current.x + 1, current.y}, &grid[current.x + 1][current.y], openList);
    }
    if (!grid[current.x - 1][current.y].closed && !grid[current.x - 1][current.y].obstacle) {
        openNode(start, end, current, (Point) {current.x - 1, current.y}, &grid[current.x - 1][current.y], openList);
    }
    if (!grid[current.x][current.y + 1].closed && !grid[current.x][current.y + 1].obstacle) {
        openNode(start, end, current, (Point) {current.x, current.y + 1}, &grid[current.x][current.y + 1], openList);
    }
    if (!grid[current.x][current.y - 1].closed && !grid[current.x][current.y - 1].obstacle) {
        openNode(start, end, current, (Point) {current.x, current.y - 1}, &grid[current.x][current.y - 1], openList);
    }
}

// A* algorithm function
void astar(Node grid[GRID_SIZE][GRID_SIZE], Point start, Point end) {
    Point* openList[GRID_SIZE * GRID_SIZE];
    Point current = start;

    // clear openList
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        *openList[i] = (Point) {-1, -1};
    }

    while (1) {

        // get neighbors of current

        findNeighbors(current, openList, grid, start, end);

        // go to best neighbor

        int minF = 999999; // lowest F value
        int minH = 999999; // lowest H value
        Point bestNode; // coords of best node

        for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
            if (openList[i] -> x < 0) {
                // empty slot; skip

                continue;
            }
            if (grid[openList[i] -> x][openList[i] -> y].f < minF) {
                minF = grid[openList[i] -> x][openList[i] -> y].f;
                bestNode = *openList[i];
            }
        }

        // close the best node and move to it

        closeNode(&grid[bestNode.x][bestNode.y], bestNode, openList);
        current = bestNode;

        if (current.x == end.x && current.x == current.y) {
            // path found
            return;
        }

    }

}


void AIPath(int startX, int startY, int endX, int endY, Node grid[GRID_SIZE][GRID_SIZE]) {
    Point start = {startX, startY};
    Point end = {endX, endY};

    astar(grid, start, end);

}