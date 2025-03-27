#include "../inc/enemy.h"

// Initialize enemies at random positions
void InitEnemies(Enemy *enemies, int count, Vector3 playerPos) {
    for (int i = 0; i < count; i++) {
        // Create random position away from player (at least 5 units away)
        Vector3 pos;
        float dist;
        do {
            pos.x = (float)GetRandomValue(-GRID_SIZE/2, GRID_SIZE/2);
            pos.z = (float)GetRandomValue(-GRID_SIZE/2, GRID_SIZE/2);
            pos.y = 0.0f;
            
            // Calculate distance from player
            dist = Vector3Distance(pos, playerPos);
        } while (dist < 5.0f);
        
        enemies[i].position = pos;
        enemies[i].size = (Vector3){ 0.8f, 1.8f, 0.8f };
        enemies[i].health = 100.0f;
        enemies[i].speed = 0.1f;
        enemies[i].color = BLUE;
        enemies[i].pathCount = 0;
        enemies[i].currentNode = 0;
        
        // Calculate initial path to player
        FindPath(&enemies[i], playerPos);
    }
}

// Draw all enemies
void DrawEnemies(Enemy *enemies, int count) {
    for (int i = 0; i < count; i++) {
        DrawCube(enemies[i].position, enemies[i].size.x, enemies[i].size.y, enemies[i].size.z, enemies[i].color);
        DrawCubeWires(enemies[i].position, enemies[i].size.x, enemies[i].size.y, enemies[i].size.z, BLACK);
        
        // Draw path for debugging
        if (enemies[i].pathCount > 0) {
            for (int j = enemies[i].currentNode; j < enemies[i].pathCount - 1; j++) {
                DrawLine3D(enemies[i].path[j], enemies[i].path[j+1], GREEN);
            }
        }
    }
}

// Update enemy positions using A* paths
void UpdateEnemies(Enemy *enemies, int count, Vector3 playerPos) {
    for (int i = 0; i < count; i++) {
        // Recalculate path periodically or if reached end of path
        if (GetRandomValue(0, 60) == 0 || enemies[i].currentNode >= enemies[i].pathCount) {
            FindPath(&enemies[i], playerPos);
        }
        
        // Follow current path if exists
        if (enemies[i].pathCount > 0 && enemies[i].currentNode < enemies[i].pathCount) {
            Vector3 targetPos = enemies[i].path[enemies[i].currentNode];
            Vector3 direction = Vector3Subtract(targetPos, enemies[i].position);
            
            // Move to next node if close enough
            if (Vector3Length(direction) < 0.1f) {
                enemies[i].currentNode++;
            } else {
                // Move towards next node
                direction = Vector3Normalize(direction);
                enemies[i].position = Vector3Add(enemies[i].position, 
                    Vector3Scale(direction, enemies[i].speed));
            }
        }
    }
}

// A* pathfinding implementation
void FindPath(Enemy *enemy, Vector3 targetPos) {
    // Convert 3D positions to grid coordinates
    int startX = (int)roundf(enemy->position.x);
    int startZ = (int)roundf(enemy->position.z);
    int targetX = (int)roundf(targetPos.x);
    int targetZ = (int)roundf(targetPos.z);
    
    // Simple direct path for basic implementation
    // In a full A* implementation, you would use open/closed lists and calculate proper paths
    
    // For simplicity, we'll do a straight line path
    int dx = abs(targetX - startX);
    int dz = abs(targetZ - startZ);
    int steps = (dx > dz) ? dx : dz;
    
    if (steps == 0) steps = 1;
    
    enemy->pathCount = steps + 1;
    if (enemy->pathCount > MAX_NODES) enemy->pathCount = MAX_NODES;
    
    for (int i = 0; i < enemy->pathCount; i++) {
        float t = (float)i / (float)steps;
        
        enemy->path[i].x = startX + (targetX - startX) * t;
        enemy->path[i].z = startZ + (targetZ - startZ) * t;
        enemy->path[i].y = 0.0f;
    }
    
    enemy->currentNode = 0;
} 