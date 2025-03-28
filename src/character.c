#include "character.h"
#include <raylib.h>

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
        // Calculate ray from mouse position
        Ray ray = GetMouseRay(mousePosition, *camera);
        
        // Ground plane is at y = 0, we need to calculate where the ray intersects it
        // Ray-plane intersection formula: t = (planeD - dot(planeNormal, rayOrigin)) / dot(planeNormal, rayDirection)
        // Where planeD = 0 (for y=0 plane) and planeNormal = (0,1,0)
        
        float t = -ray.position.y / ray.direction.y;
        
        // Check if ray is parallel to plane or going away from it
        if (t <= 0) {
            TraceLog(LOG_WARNING, "Ray does not intersect ground plane (parallel or wrong direction)");
            return;
        }
        
        // Calculate intersection point
        Vector3 targetPoint = {
            ray.position.x + ray.direction.x * t,
            0.0f,
            ray.position.z + ray.direction.z * t
        };
        
        TraceLog(LOG_INFO, "Ray hit ground at: (%f, %f, %f)", 
                 targetPoint.x, targetPoint.y, targetPoint.z);
        
        // Get an inactive projectile
        for (int i = 0; i < projectileCount; i++) {
            if (!projectiles[i].active) {
                // Set projectile position (slightly above character to match "gun" height)
                Vector3 shootPos = character->position;
                shootPos.y += character->size.y * 0.50f;
                
                // Set initial position
                projectiles[i].position = shootPos;
                
                // Calculate direction from character to hit point
                Vector3 direction = Vector3Subtract(targetPoint, shootPos);
                
                // Project onto XZ plane (set Y to 0)
                direction.y = 0.0f;
                
                // Ensure direction is normalized
                projectiles[i].direction = Vector3Normalize(direction);
                
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
                
                break;
            }
        }
    } else {
        TraceLog(LOG_INFO, "Player tried to shoot but cooldown active: %.2f", character->shootTimer);
    }
} 