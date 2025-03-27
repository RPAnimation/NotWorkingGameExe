#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Game constants
#define MAX_ENEMIES 10
#define GRID_SIZE 20
#define CELL_SIZE 1.0f
#define MAX_NODES 100

// Collision detection functions
BoundingBox GetBoundingBox(Vector3 position, Vector3 size);
Vector3 GetCorrectedPosition(Vector3 currentPos, Vector3 newPos, Vector3 entitySize, 
                            Vector3 otherPos, Vector3 otherSize);

#endif // COMMON_H 