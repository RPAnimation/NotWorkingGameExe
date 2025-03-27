/*******************************************************************************************
*
*   Isometric Shooter Game
*
*   A basic shooter game with isometric view and character movement
*
********************************************************************************************/

#include "../inc/common.h"
#include "../inc/character.h"
#include "../inc/enemy.h"
#include "../inc/projectile.h"

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
    
    // No cursor capture - cursor remains visible and free

    // Initialize character
    Character player;
    InitCharacter(&player);

    // Initialize enemies
    Enemy enemies[MAX_ENEMIES];
    InitEnemies(enemies, MAX_ENEMIES, player.position);

    // Initialize projectiles
    Projectile projectiles[MAX_PROJECTILES];
    InitProjectiles(projectiles, MAX_PROJECTILES);

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
        // Calculate delta time
        float deltaTime = GetFrameTime();
        
        // Update
        //----------------------------------------------------------------------------------
        
        // Update character
        UpdateCharacter(&player, deltaTime);
        
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
            
            // Calculate new position
            Vector3 newPosition = player.position;
            newPosition.x += moveDirection.x * player.speed;
            newPosition.z += moveDirection.z * player.speed;
            
            // Check collision with each enemy
            bool collision = false;
            for (int i = 0; i < MAX_ENEMIES; i++) {
                BoundingBox playerBox = GetBoundingBox(newPosition, player.size);
                BoundingBox enemyBox = GetBoundingBox(enemies[i].position, enemies[i].size);
                
                if (CheckCollisionBoxes(playerBox, enemyBox)) {
                    collision = true;
                    // Get corrected position that doesn't collide
                    newPosition = GetCorrectedPosition(player.position, newPosition, player.size, 
                                                        enemies[i].position, enemies[i].size);
                }
            }
            
            // Update player position with collision-aware position
            player.position = newPosition;
        }
        
        // Handle player shooting with mouse
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ShootPlayerProjectile(&player, projectiles, MAX_PROJECTILES, &camera, GetMousePosition());
        }
        
        // Update enemies with steering behaviors and shooting
        UpdateEnemies(enemies, MAX_ENEMIES, player.position, projectiles, MAX_PROJECTILES, deltaTime);
        
        // Update projectiles
        UpdateProjectiles(projectiles, MAX_PROJECTILES, deltaTime);
        
        // Check for projectile collisions with player
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            // Only check enemy projectiles
            if (projectiles[i].active && projectiles[i].type == PROJECTILE_ENEMY) {
                if (CheckProjectileCollision(projectiles[i], 
                                            (Vector3){player.position.x, player.position.y + 1.0f, player.position.z}, 
                                            0.5f)) {
                    // Player hit by projectile
                    projectiles[i].active = false;
                    
                    // You could implement player health/damage here
                    // For example: player.health -= 10;
                }
            }
        }
        
        // Check for player projectile collisions with enemies
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            // Only check player projectiles
            if (projectiles[i].active && projectiles[i].type == PROJECTILE_PLAYER) {
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (CheckProjectileCollision(projectiles[i], 
                                                (Vector3){enemies[j].position.x, enemies[j].position.y + 1.0f, enemies[j].position.z}, 
                                                0.5f)) {
                        // Enemy hit by projectile
                        projectiles[i].active = false;
                        
                        // Damage enemy
                        enemies[j].health -= 25.0f;
                        
                        // Change enemy color when hit
                        enemies[j].color = PURPLE;
                        
                        // If enemy health drops to 0 or below, "kill" it
                        if (enemies[j].health <= 0) {
                            // Reset enemy position far away
                            enemies[j].position.x = GetRandomValue(-GRID_SIZE, GRID_SIZE);
                            enemies[j].position.z = GetRandomValue(-GRID_SIZE, GRID_SIZE);
                            enemies[j].health = 100.0f;
                            enemies[j].color = BLUE;
                        }
                        
                        break; // Break out of enemy loop after hit
                    }
                }
            }
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
                
                // Draw the player character
                DrawCharacter(&player);
                
                // Draw enemies
                DrawEnemies(enemies, MAX_ENEMIES);
                
                // Draw projectiles
                DrawProjectiles(projectiles, MAX_PROJECTILES);
                
            EndMode3D();
            
            // Draw UI
            DrawText("Use WASD or Arrow Keys to move", 10, 10, 20, BLACK);
            DrawText("Left-click to shoot at cursor position", 10, 40, 20, BLACK);
            
            // Display debug information
            DrawText(TextFormat("Cursor position: %i, %i", GetMouseX(), GetMouseY()), 10, 70, 20, BLACK);
            DrawText(TextFormat("Player cooldown: %.2f", player.shootTimer), 10, 100, 20, BLACK);
            DrawText(TextFormat("Active projectiles: %i", CountActiveProjectiles(projectiles, MAX_PROJECTILES)), 10, 130, 20, BLACK);
            
            DrawFPS(screenWidth - 100, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}