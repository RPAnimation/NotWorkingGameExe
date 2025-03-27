#include "common.h"

// Create a BoundingBox from position and size
BoundingBox GetBoundingBox(Vector3 position, Vector3 size) {
    BoundingBox box;
    
    // Calculate minimum point (bottom-left-front corner)
    box.min.x = position.x - size.x/2;
    box.min.y = position.y;
    box.min.z = position.z - size.z/2;
    
    // Calculate maximum point (top-right-back corner)
    box.max.x = position.x + size.x/2;
    box.max.y = position.y + size.y;
    box.max.z = position.z + size.z/2;
    
    return box;
}

// Get a corrected position to prevent collision
Vector3 GetCorrectedPosition(Vector3 currentPos, Vector3 newPos, Vector3 entitySize, 
                            Vector3 otherPos, Vector3 otherSize) {
    // Try to move only in X direction
    Vector3 tempPos = currentPos;
    tempPos.x = newPos.x;
    
    BoundingBox entityBox = GetBoundingBox(tempPos, entitySize);
    BoundingBox otherBox = GetBoundingBox(otherPos, otherSize);
    
    if (!CheckCollisionBoxes(entityBox, otherBox)) {
        // X movement is valid
        currentPos.x = newPos.x;
    }
    
    // Try to move only in Z direction
    tempPos = currentPos;
    tempPos.z = newPos.z;
    
    entityBox = GetBoundingBox(tempPos, entitySize);
    
    if (!CheckCollisionBoxes(entityBox, otherBox)) {
        // Z movement is valid
        currentPos.z = newPos.z;
    }
    
    return currentPos;
} 