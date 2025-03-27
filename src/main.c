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
// Character structure
typedef struct {
    Vector3 position;      // 3D position
    Vector3 size;          // Size of the character
    float rotation;        // Rotation angle
    float speed;           // Movement speed
    Color color;           // Color of the character
} Character;

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

    // Initialize camera
    Camera3D camera = {
        .position = (Vector3){ 10.0f, 10.0f, 10.0f },    // Camera position
        .target = player.position,                       // Camera looking at player
        .up = (Vector3){ 0.0f, 1.0f, 0.0f },             // Camera up vector (rotation towards target)
        .fovy = 45.0f,                                   // Camera field-of-view Y
        .projection = CAMERA_PERSPECTIVE                 // Perspective projection
    };

    // Create a grid for the floor
    const int gridSize = 20;

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
            printf("moveDirection: %f, %f, %f\n", moveDirection.x, moveDirection.y, moveDirection.z);
            printf("player.position: %f, %f, %f\n", player.position.x, player.position.y, player.position.z);
        
        }
        
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
                
            EndMode3D();
            
            // Draw UI
            DrawText("Use WASD or Arrow Keys to move", 10, 10, 20, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
