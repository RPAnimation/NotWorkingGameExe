#ifndef CHARACTER_H
#define CHARACTER_H

#include "common.h"

// Character structure
typedef struct {
    Vector3 position;      // 3D position
    Vector3 size;          // Size of the character
    float rotation;        // Rotation angle
    float speed;           // Movement speed
    Color color;           // Color of the character
} Character;

#endif // CHARACTER_H 