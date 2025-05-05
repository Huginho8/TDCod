#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "PhysicsBody.h"
#include "Types.h"
#include "Vec2.h"

#include <unordered_map>
#include <array>

class GameWorld;

// Alias for pointer to PhysicsBody
typedef PhysicsBody* PhysicsBody_ptr;

// Represents a spatial grid cell for collision optimization
struct GridCell {
    std::array<PhysicsBody_ptr, 1000> _bodies;
    std::array<BodyID, 1000> _bodiesID;
    int _maxIndex;

    GridCell();
    void add(PhysicsBody* body);
    void reset();
};

// Manages spatial partitioning and collision resolution
class CollisionManager {
public:
    CollisionManager();
    CollisionManager(float timeStep, float bodyRadius, Vec2 mapSize, GameWorld* world);

    // Main update function to be called every tick
    void update();

    // Adds a body to the system
    void addBody(PhysicsBody* body);

    // Returns radius of bodies managed
    float getBodyRadius() const { return m_bodySize * 0.5f; }

    // Returns world bounds
    Vec2 getMapSize() const { return m_mapSize; }

    // Access to grid cell at a position (not currently used in update)
    GridCell* getBodyAt(Vec2 coord);

    // Retrieve body by ID (static)
    static PhysicsBody* getBodyByID(BodyID id);

private:
    void addBodyToGrid(PhysicsBody_ptr body);
    void solveGridCollisions(GridCell& cell);
    void solveCollisions();
    void solveBoundCollisions(PhysicsBody_ptr body);

    long convertPosToHash(int x, int y) const;

private:
    float m_timeStep;
    float m_bodySize;
    Vec2  m_mapSize;

    uint32_t m_nCollisionChecked;
    uint32_t m_iterationCount;

    long m_currentIteration;
    int  m_newHash;

    std::unordered_map<long, GridCell> m_grid;

    GameWorld* _world;
};

#endif // COLLISIONMANAGER_H