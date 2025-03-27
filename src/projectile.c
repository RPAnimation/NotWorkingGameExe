#include "projectile.h"

// Initialize all projectiles
void InitProjectiles(Projectile *projectiles, int count) {
    for (int i = 0; i < count; i++) {
        projectiles[i].position = (Vector3){0.0f, 0.0f, 0.0f};
        projectiles[i].direction = (Vector3){0.0f, 0.0f, 0.0f};
        projectiles[i].speed = 0.3f;
        projectiles[i].radius = 0.2f;
        projectiles[i].color = ORANGE;
        projectiles[i].active = false;
        projectiles[i].lifetime = 0.0f;
        projectiles[i].maxLifetime = 3.0f;  // 3 seconds lifetime
        projectiles[i].type = PROJECTILE_ENEMY; // Default type
    }
}

// Update projectiles position and check lifetime
void UpdateProjectiles(Projectile *projectiles, int count, float deltaTime) {
    for (int i = 0; i < count; i++) {
        if (projectiles[i].active) {
            // Update position based on direction and speed
            projectiles[i].position.x += projectiles[i].direction.x * projectiles[i].speed;
            projectiles[i].position.y += projectiles[i].direction.y * projectiles[i].speed;
            projectiles[i].position.z += projectiles[i].direction.z * projectiles[i].speed;
            
            // Update lifetime
            projectiles[i].lifetime += deltaTime;
            
            // Deactivate if lifetime exceeds maximum
            if (projectiles[i].lifetime >= projectiles[i].maxLifetime) {
                projectiles[i].active = false;
            }
        }
    }
}

// Draw all active projectiles
void DrawProjectiles(Projectile *projectiles, int count) {
    for (int i = 0; i < count; i++) {
        if (projectiles[i].active) {
            DrawSphere(projectiles[i].position, projectiles[i].radius, projectiles[i].color);
        }
    }
}

// Shoot a projectile from a position toward a target (for enemies)
void ShootProjectile(Projectile *projectiles, int count, Vector3 position, Vector3 target) {
    // Find an inactive projectile
    for (int i = 0; i < count; i++) {
        if (!projectiles[i].active) {
            // Set projectile properties
            projectiles[i].position = position;
            
            // Calculate direction vector
            Vector3 direction = Vector3Subtract(target, position);
            projectiles[i].direction = Vector3Normalize(direction);
            
            // Adjust y position to aim at player's center
            projectiles[i].position.y = position.y + 1.0f;
            
            // Set properties for enemy projectile
            projectiles[i].color = ORANGE;
            projectiles[i].speed = 0.3f;
            projectiles[i].type = PROJECTILE_ENEMY;
            
            // Activate projectile
            projectiles[i].active = true;
            projectiles[i].lifetime = 0.0f;
            
            break;  // Only shoot one projectile at a time
        }
    }
}

// Check if a projectile collides with a target
bool CheckProjectileCollision(Projectile projectile, Vector3 targetPosition, float targetRadius) {
    if (!projectile.active) return false;
    
    // Calculate distance between projectile and target
    float distance = Vector3Distance(projectile.position, targetPosition);
    
    // Check if distance is less than sum of radii
    return distance < (projectile.radius + targetRadius);
}

// Count the number of active projectiles
int CountActiveProjectiles(Projectile *projectiles, int count) {
    int activeCount = 0;
    
    for (int i = 0; i < count; i++) {
        if (projectiles[i].active) {
            activeCount++;
        }
    }
    
    return activeCount;
} 