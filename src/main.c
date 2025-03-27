/*******************************************************************************************
*
*   Isometric Shooter Game
*
*   A basic shooter game with isometric view and character movement
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ENEMIES 10
#define GRID_SIZE 20
#define CELL_SIZE 1.0f
#define MAX_NODES 100

// Character structure
typedef struct {
    Vector3 position;      // 3D position
    Vector3 size;          // Size of the character
    float rotation;        // Rotation angle
    float speed;           // Movement speed
    Color color;           // Color of the character
} Character;

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

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Not Working Game Exe");

    // Initialize character
    Character player = {
        .position = (Vector3){ 0.0f, 0.0f, 0.0f },
        .size = (Vector3){ 1.0f, 2.0f, 1.0f },
        .rotation = 0.0f,
        .speed = 0.2f,
        .color = RED
    };

    // Initialize enemies
    Enemy enemies[MAX_ENEMIES];
    InitEnemies(enemies, MAX_ENEMIES, player.position);

    // Initialize camera
    Camera3D camera = {
        .position = (Vector3){ 10.0f, 10.0f, 10.0f },    // Camera position
        .target = player.position,                       // Camera looking at player
        .up = (Vector3){ 0.0f, 1.0f, 0.0f },             // Camera up vector (rotation towards target)
        .fovy = 45.0f,                                   // Camera field-of-view Y
        .projection = CAMERA_PERSPECTIVE                 // Perspective projection
    };

    // Create a grid for the floor
    const int gridSize = GRID_SIZE;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        
        // Process keyboard input independently for each direction
        // This ensures multiple keys can be processed simultaneously
        bool upPressed = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
        bool downPressed = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
        bool leftPressed = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
        bool rightPressed = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
        
        // Build movement vector based on key states
        Vector3 moveDirection = {0.0f, 0.0f, 0.0f};
        
        // Apply individual directional inputs for isometric movement
        if (upPressed) {
            moveDirection.x -= 1.0f; // Move left and forward for up
            moveDirection.z -= 1.0f;
        }
        if (downPressed) {
            moveDirection.x += 1.0f; // Move right and backward for down
            moveDirection.z += 1.0f;
        }
        if (leftPressed) {
            moveDirection.x -= 1.0f; // Move left and backward for left
            moveDirection.z += 1.0f;
        }
        if (rightPressed) {
            moveDirection.x += 1.0f; // Move right and forward for right
            moveDirection.z -= 1.0f;
        }
        
        // Apply movement if there is any
        float moveLength = Vector3Length(moveDirection);
        if (moveLength > 0.0f) {
            // Normalize using raylib's function
            moveDirection = Vector3Normalize(moveDirection);
            
            // Update player position
            player.position.x += moveDirection.x * player.speed;
            player.position.z += moveDirection.z * player.speed;
        }
        
        // Update enemies with A* pathfinding
        UpdateEnemies(enemies, MAX_ENEMIES, player.position);
        
        // Update camera to follow the player with isometric perspective
        camera.target = player.position;
        camera.position = (Vector3){
            player.position.x + 10.0f,
            player.position.y + 10.0f,
            player.position.z + 10.0f
        };
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            BeginMode3D(camera);
                
                // Draw grid floor
                DrawGrid(gridSize, 1.0f);
                
                // Draw the player character (as a cube)
                DrawCube(player.position, player.size.x, player.size.y, player.size.z, player.color);
                DrawCubeWires(player.position, player.size.x, player.size.y, player.size.z, BLACK);
                
                // Draw enemies
                DrawEnemies(enemies, MAX_ENEMIES);
                
            EndMode3D();
            
            // Draw UI
            DrawText("Use WASD or Arrow Keys to move", 10, 10, 20, BLACK);
            DrawText(TextFormat("Enemies: %d", MAX_ENEMIES), 10, 40, 20, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

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
