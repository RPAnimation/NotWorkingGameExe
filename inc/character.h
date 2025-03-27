#ifndef CHARACTER_H
#define CHARACTER_H

#include "common.h"
#include "projectile.h"

// Character structure
typedef struct {
    Vector3 position;      // 3D position
    Vector3 size;          // Size of the character
    float rotation;        // Rotation angle
    float speed;           // Movement speed
    Color color;           // Color of the character
    float shootCooldown;   // Cooldown time between shots
    float shootTimer;      // Current timer for shooting cooldown
} Character;

// Function declarations
void InitCharacter(Character *character);
void UpdateCharacter(Character *character, float deltaTime);
void DrawCharacter(Character *character);
void ShootPlayerProjectile(Character *character, Projectile *projectiles, int projectileCount, 
                           Camera3D *camera, Vector2 mousePosition);

#endif // CHARACTER_H 