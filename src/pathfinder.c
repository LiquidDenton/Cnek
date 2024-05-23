#include <stdio.h>
#include <stdlib.h>
#include "./constants.h"
#include <math.h>

typedef struct {
    int x, y; // coordinates
} Point;

typedef struct {
    Point parent;
    short int f, g, h; // f = g + h
    short int obstacle, closed, tentativeG;
} Node;

// manhattan distance function
int manhattan(Point start, Point end) {
    return abs(start.x - end.x) + abs(start.y - end.y);
}

// evaluate value of node using distance to start and end points
void evalNode(Point start, Point end, Point current, Node* node, int parentTentG) {
    node->g = manhattan(current, start);
    node->h = manhattan(current, end);
    node->f = node->g + node->h;
    node->tentativeG = parentTentG + 1;
}

// call evaluate the node and add it to the openlist
void openNode(Point start, Point end, Point parent, Point current, Node* node, Point* openList[], int parentTentG) {

    // check that node isn't open

    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        if (openList[i] -> x == current.x && openList[i] -> y == current.y) {
            // Node is in list, cancel open
            return;
        } 
    }

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

    evalNode(start, end, current, node, parentTentG);

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

    // set node's state to closed

    node -> closed = 1;

}

void findNeighbors(Point current, Point* openList[], Node fooGrid[GRID_SIZE][GRID_SIZE], Point start, Point end) {

    // open the node in each direction if the node is not closed and not an obstacle

    // check that the node to be opened is in bounds
    if (current.x < GRID_SIZE - 1) {
        if (!fooGrid[current.x + 1][current.y].closed && !fooGrid[current.x + 1][current.y].obstacle) {
            openNode(start, end, current, (Point) {current.x + 1, current.y}, &fooGrid[current.x + 1][current.y], openList, fooGrid[current.x][current.y].tentativeG);
        }
    }
    if (current.x > 0) {
        if (!fooGrid[current.x - 1][current.y].closed && !fooGrid[current.x - 1][current.y].obstacle && current.x > 0) {
            openNode(start, end, current, (Point) {current.x - 1, current.y}, &fooGrid[current.x - 1][current.y], openList, fooGrid[current.x][current.y].tentativeG);
        }
    }
    if (current.y < GRID_SIZE - 1){
        if (!fooGrid[current.x][current.y + 1].closed && !fooGrid[current.x][current.y + 1].obstacle) {
            openNode(start, end, current, (Point) {current.x, current.y + 1}, &fooGrid[current.x][current.y + 1], openList, fooGrid[current.x][current.y].tentativeG);
        }
    }
    if (current.y > 0) {
        if (!fooGrid[current.x][current.y - 1].closed && !fooGrid[current.x][current.y - 1].obstacle) {
            openNode(start, end, current, (Point) {current.x, current.y - 1}, &fooGrid[current.x][current.y - 1], openList, fooGrid[current.x][current.y].tentativeG);
        }
    }
}

// A* algorithm function
void astar(Node fooGrid[GRID_SIZE][GRID_SIZE], Point start, Point end) {
    Point* openList[GRID_SIZE * GRID_SIZE];
    Point current = start;
    int depth = 0;

    // clear openList
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        openList[i] = malloc(sizeof(Point));
        *openList[i] = (Point) {-1, -1};
    }

    while (depth <= MAX_DEPTH) {

        // get neighbors of current

        findNeighbors(current, openList, fooGrid, start, end);

        // go to best neighbor

        int minF = 99999; // lowest F value
        int minH = 99999; // lowest H value
        Point bestNode = {0, 0}; // coords of best node
        int openNodeCount = 0; // Number of open nodes in openList

        for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
            if (openList[i] -> x < 0) {
                // empty slot; skip

                continue;
            }
            // open node found, increment node count and evaluate node
            openNodeCount++;
            if (fooGrid[openList[i] -> x][openList[i] -> y].f < minF) {
                minF = fooGrid[openList[i] -> x][openList[i] -> y].f;
                minH = fooGrid[openList[i] -> x][openList[i] -> y].h;
                bestNode = *openList[i];
            } else if (fooGrid[openList[i] -> x][openList[i] -> y].f == minF && fooGrid[openList[i] -> x][openList[i] -> y].h < minH) {
                minF = fooGrid[openList[i] -> x][openList[i] -> y].f;
                minH = fooGrid[openList[i] -> x][openList[i] -> y].h;
                bestNode = *openList[i];
            }
        }

        // if there arent any open nodes, the snek is trapped, exit

        if (!(openNodeCount > 0)) {
            break;
        }

        // close the best node and move to it

        closeNode(&fooGrid[bestNode.x][bestNode.y], bestNode, openList);
        current = bestNode;

        if (current.x == end.x && current.y == end.y) {
            // path found
            break;
        }

        depth++;

    }

    // free memory
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        free(openList[i]);
    }
}

void AIPath(int startX, int startY, int endX, int endY, Node fooGrid[GRID_SIZE][GRID_SIZE]) {
    Point start = {startX, startY};
    Point end = {endX, endY};

    astar(fooGrid, start, end);

}