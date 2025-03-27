#include "enemy.h"

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
        enemies[i].velocity = (Vector3){ 0.0f, 0.0f, 0.0f };
        enemies[i].steeringForce = (Vector3){ 0.0f, 0.0f, 0.0f };
        enemies[i].health = 100.0f;
        enemies[i].speed = 0.1f;
        enemies[i].maxForce = 0.01f;
        enemies[i].separationRadius = 3.0f;
        enemies[i].color = BLUE;
        
        // Initialize shooting properties
        enemies[i].shootTimer = 0.0f;
        enemies[i].shootInterval = GetRandomValue(2, 5); // Random interval between 2-5 seconds
    }
}

// Draw all enemies
void DrawEnemies(Enemy *enemies, int count) {
    for (int i = 0; i < count; i++) {
        DrawCube(enemies[i].position, enemies[i].size.x, enemies[i].size.y, enemies[i].size.z, enemies[i].color);
        DrawCubeWires(enemies[i].position, enemies[i].size.x, enemies[i].size.y, enemies[i].size.z, BLACK);
        
        // Optionally, visualize the velocity vector for debugging
        Vector3 velocityEnd = Vector3Add(enemies[i].position, 
                                         Vector3Scale(enemies[i].velocity, 10.0f));
        DrawLine3D(enemies[i].position, velocityEnd, GREEN);
    }
}

// Calculate seeking force toward a target
Vector3 SeekForce(Enemy *enemy, Vector3 targetPos) {
    // Direction to target
    Vector3 desired = Vector3Subtract(targetPos, enemy->position);
    
    if (Vector3Length(desired) > 0.0f) {
        // Scale to maximum speed
        desired = Vector3Scale(Vector3Normalize(desired), enemy->speed);
        
        // Steering = desired - velocity
        return Vector3Subtract(desired, enemy->velocity);
    }
    
    return (Vector3){ 0.0f, 0.0f, 0.0f };
}

// Calculate separation force to avoid other enemies
Vector3 SeparationForce(Enemy *enemy, Enemy *enemies, int count, int ownIndex) {
    Vector3 force = { 0.0f, 0.0f, 0.0f };
    int neighbors = 0;
    
    for (int i = 0; i < count; i++) {
        if (i != ownIndex) {
            float distance = Vector3Distance(enemy->position, enemies[i].position);
            
            if (distance < enemy->separationRadius) {
                // Vector pointing away from neighbor
                Vector3 repulsion = Vector3Subtract(enemy->position, enemies[i].position);
                
                // Weight by distance (closer = stronger)
                if (Vector3Length(repulsion) > 0) {
                    repulsion = Vector3Normalize(repulsion);
                    // Scale repulsion force inversely by distance
                    repulsion = Vector3Scale(repulsion, enemy->separationRadius / (distance + 0.01f));
                    force = Vector3Add(force, repulsion);
                    neighbors++;
                }
            }
        }
    }
    
    // Average the force if we have neighbors
    if (neighbors > 0) {
        force = Vector3Scale(force, 1.0f / neighbors);
        
        // Scale to maximum speed and calculate steering
        if (Vector3Length(force) > 0) {
            force = Vector3Scale(Vector3Normalize(force), enemy->speed);
            force = Vector3Subtract(force, enemy->velocity);
        }
    }
    
    return force;
}

// Add a small random force for natural movement
Vector3 RandomForce(Enemy *enemy) {
    // Create a small random force occasionally
    if (GetRandomValue(0, 30) == 0) {
        float randomAngle = ((float)GetRandomValue(0, 360)) * DEG2RAD;
        float randomMagnitude = (float)GetRandomValue(1, 20) / 100.0f;
        
        return (Vector3) {
            cosf(randomAngle) * randomMagnitude,
            0.0f,
            sinf(randomAngle) * randomMagnitude
        };
    }
    
    return (Vector3){ 0.0f, 0.0f, 0.0f };
}

// Calculate the combined steering force for an enemy
Vector3 CalculateSteeringForce(Enemy *enemy, Enemy *enemies, int count, int ownIndex, Vector3 playerPos) {
    // Calculate individual forces
    Vector3 seek = SeekForce(enemy, playerPos);
    Vector3 separation = SeparationForce(enemy, enemies, count, ownIndex);
    Vector3 random = RandomForce(enemy);
    
    // Weight and combine forces (adjust weights for different behaviors)
    seek = Vector3Scale(seek, 1.0f);
    separation = Vector3Scale(separation, 1.5f);
    random = Vector3Scale(random, 0.3f);
    
    // Sum all forces
    Vector3 totalForce = Vector3Add(seek, Vector3Add(separation, random));
    
    // Limit the maximum force
    float magnitude = Vector3Length(totalForce);
    if (magnitude > enemy->maxForce) {
        totalForce = Vector3Scale(totalForce, enemy->maxForce / magnitude);
    }
    
    return totalForce;
}

// Update enemy positions using steering behaviors and handle shooting
void UpdateEnemies(Enemy *enemies, int count, Vector3 playerPos, Projectile *projectiles, int projectileCount, float deltaTime) {
    for (int i = 0; i < count; i++) {
        // Calculate steering force
        Vector3 steeringForce = CalculateSteeringForce(&enemies[i], enemies, count, i, playerPos);
        
        // Apply force to velocity (acceleration)
        enemies[i].velocity = Vector3Add(enemies[i].velocity, steeringForce);
        
        // Limit velocity to maximum speed
        float speed = Vector3Length(enemies[i].velocity);
        if (speed > enemies[i].speed) {
            enemies[i].velocity = Vector3Scale(enemies[i].velocity, enemies[i].speed / speed);
        }
        
        // Calculate new position
        Vector3 newPosition = Vector3Add(enemies[i].position, enemies[i].velocity);
        
        // Check collision with player
        BoundingBox enemyBox = GetBoundingBox(newPosition, enemies[i].size);
        BoundingBox playerBox = GetBoundingBox(playerPos, (Vector3){1.0f, 2.0f, 1.0f});

        if (CheckCollisionBoxes(enemyBox, playerBox)) {
            // Don't move if we would collide with player
            newPosition = GetCorrectedPosition(enemies[i].position, newPosition, enemies[i].size,
                                            playerPos, (Vector3){1.0f, 2.0f, 1.0f});
            
            // Reset velocity to avoid getting stuck
            enemies[i].velocity = (Vector3){ 0.0f, 0.0f, 0.0f };
        }
        
        // Check collisions with other enemies
        for (int j = 0; j < count; j++) {
            if (i != j) { // Don't check collision with self
                BoundingBox otherEnemyBox = GetBoundingBox(enemies[j].position, enemies[j].size);
                
                if (CheckCollisionBoxes(enemyBox, otherEnemyBox)) {
                    // Don't move if we would collide with another enemy
                    newPosition = GetCorrectedPosition(enemies[i].position, newPosition, enemies[i].size,
                                                    enemies[j].position, enemies[j].size);
                    
                    // Reset velocity to avoid getting stuck
                    enemies[i].velocity = (Vector3){ 0.0f, 0.0f, 0.0f };
                    
                    // Update enemyBox after position correction
                    enemyBox = GetBoundingBox(newPosition, enemies[i].size);
                }
            }
        }
        
        // Update enemy position with collision-aware position
        enemies[i].position = newPosition;
        
        // Handle shooting
        enemies[i].shootTimer += deltaTime;
        
        // Check if it's time to shoot and if player is in sight (simple distance check)
        float distanceToPlayer = Vector3Distance(enemies[i].position, playerPos);
        if (enemies[i].shootTimer >= enemies[i].shootInterval && distanceToPlayer < 15.0f) {
            // Shoot at player
            ShootProjectile(projectiles, projectileCount, enemies[i].position, playerPos);
            
            // Reset timer
            enemies[i].shootTimer = 0.0f;
            
            // Set new random interval
            enemies[i].shootInterval = GetRandomValue(2, 5);
        }
    }
} 