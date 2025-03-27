#ifndef ENEMY_H
#define ENEMY_H

#include "common.h"

// Enemy structure
typedef struct {
    Vector3 position;      // 3D position
    Vector3 size;          // Size of the enemy
    float health;          // Enemy health
    float speed;           // Movement speed
    Color color;           // Color of the enemy
    Vector3 path[MAX_NODES]; // Path for A* algorithm
    int pathCount;         // Number of nodes in path
    int currentNode;       // Current node in path
} Enemy;

// Node structure for A* pathfinding
typedef struct {
    int x;
    int z;
    float g;      // Cost from start to current node
    float h;      // Heuristic (estimated cost from current to goal)
    float f;      // f = g + h
    int parent;   // Index of parent node
} Node;

// Function declarations
void InitEnemies(Enemy *enemies, int count, Vector3 playerPos);
void UpdateEnemies(Enemy *enemies, int count, Vector3 playerPos);
void DrawEnemies(Enemy *enemies, int count);
void FindPath(Enemy *enemy, Vector3 targetPos);

#endif // ENEMY_H 