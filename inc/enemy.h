#ifndef ENEMY_H
#define ENEMY_H

#include "common.h"
#include "projectile.h"

// Enemy structure
typedef struct {
    Vector3 position;      // 3D position
    Vector3 size;          // Size of the enemy
    Vector3 velocity;      // Current velocity
    Vector3 steeringForce; // Accumulated steering force
    float health;          // Enemy health
    float speed;           // Maximum movement speed
    float maxForce;        // Maximum steering force
    float separationRadius; // Radius to maintain separation from other enemies
    Color color;           // Color of the enemy
    float shootTimer;      // Timer for shooting
    float shootInterval;   // Time between shots
} Enemy;

// Function declarations
void InitEnemies(Enemy *enemies, int count, Vector3 playerPos);
void UpdateEnemies(Enemy *enemies, int count, Vector3 playerPos, Projectile *projectiles, int projectileCount, float deltaTime);
void DrawEnemies(Enemy *enemies, int count);
Vector3 CalculateSteeringForce(Enemy *enemy, Enemy *enemies, int count, int ownIndex, Vector3 playerPos);
Vector3 SeekForce(Enemy *enemy, Vector3 targetPos);
Vector3 SeparationForce(Enemy *enemy, Enemy *enemies, int count, int ownIndex);
Vector3 RandomForce(Enemy *enemy);

#endif // ENEMY_H 