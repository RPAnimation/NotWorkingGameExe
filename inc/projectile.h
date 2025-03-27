#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "common.h"

#define MAX_PROJECTILES 100

typedef enum {
    PROJECTILE_ENEMY,
    PROJECTILE_PLAYER
} ProjectileType;

typedef struct {
    Vector3 position;
    Vector3 direction;
    float speed;
    float radius;
    Color color;
    bool active;
    float lifetime;      // How long the projectile lives
    float maxLifetime;   // Maximum lifetime in seconds
    ProjectileType type; // Who fired the projectile
} Projectile;

// Function declarations
void InitProjectiles(Projectile *projectiles, int count);
void UpdateProjectiles(Projectile *projectiles, int count, float deltaTime);
void DrawProjectiles(Projectile *projectiles, int count);
void ShootProjectile(Projectile *projectiles, int count, Vector3 position, Vector3 target);
bool CheckProjectileCollision(Projectile projectile, Vector3 targetPosition, float targetRadius);
int CountActiveProjectiles(Projectile *projectiles, int count);

#endif // PROJECTILE_H 