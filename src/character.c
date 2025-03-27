#include "character.h"

// Initialize character
void InitCharacter(Character *character) {
    character->position = (Vector3){ 0.0f, 0.0f, 0.0f };
    character->size = (Vector3){ 1.0f, 2.0f, 1.0f };
    character->rotation = 0.0f;
    character->speed = 0.2f;
    character->color = RED;
    character->shootCooldown = 0.2f; // 200ms between shots
    character->shootTimer = 0.0f;
}

// Update character
void UpdateCharacter(Character *character, float deltaTime) {
    // Update shooting cooldown timer
    if (character->shootTimer > 0) {
        character->shootTimer -= deltaTime;
        if (character->shootTimer < 0) character->shootTimer = 0;
    }
}

// Draw character
void DrawCharacter(Character *character) {
    DrawCube(character->position, character->size.x, character->size.y, character->size.z, character->color);
    DrawCubeWires(character->position, character->size.x, character->size.y, character->size.z, BLACK);
}

// Shoot a projectile from player toward mouse position
void ShootPlayerProjectile(Character *character, Projectile *projectiles, int projectileCount, 
                          Camera3D *camera, Vector2 mousePosition) {
    // Check if player can shoot (cooldown elapsed)
    if (character->shootTimer <= 0) {
        // Calculate direction from screen coordinates
        Ray ray = GetMouseRay(mousePosition, *camera);
        
        // Debug ray visually
        TraceLog(LOG_INFO, "Mouse Ray: Origin(%f, %f, %f), Direction(%f, %f, %f)",
                ray.position.x, ray.position.y, ray.position.z,
                ray.direction.x, ray.direction.y, ray.direction.z);
        
        // Get an inactive projectile
        for (int i = 0; i < projectileCount; i++) {
            if (!projectiles[i].active) {
                // Set projectile position (slightly above character to match "gun" height)
                Vector3 shootPos = character->position;
                shootPos.y += character->size.y * 0.75f; // Shoot from upper body
                
                // Set initial position
                projectiles[i].position = shootPos;
                
                // Create a direction vector in the horizontal plane (X-Z)
                // Keep X and Z from the ray direction, set Y to 0
                Vector3 projectileDirection = {ray.direction.x, 0.0f, ray.direction.z};
                
                // Normalize to ensure consistent speed
                projectiles[i].direction = Vector3Normalize(projectileDirection);
                
                // Make projectiles faster than enemy projectiles
                projectiles[i].speed = 0.5f;
                
                // Set different color for player projectiles
                projectiles[i].color = YELLOW;
                
                // Set type to player projectile
                projectiles[i].type = PROJECTILE_PLAYER;
                
                // Reset lifetime
                projectiles[i].lifetime = 0.0f;
                
                // Activate projectile
                projectiles[i].active = true;
                
                // Start cooldown
                character->shootTimer = character->shootCooldown;
                
                TraceLog(LOG_INFO, "Player fired projectile: Position(%f, %f, %f), Direction(%f, %f, %f)", 
                         shootPos.x, shootPos.y, shootPos.z,
                         projectiles[i].direction.x, projectiles[i].direction.y, projectiles[i].direction.z);
                
                break;
            }
        }
    } else {
        TraceLog(LOG_INFO, "Player tried to shoot but cooldown active: %.2f", character->shootTimer);
    }
} 