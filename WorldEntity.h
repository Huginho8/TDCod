#ifndef WORLDENTITY_HPP_INCLUDED
#define WORLDENTITY_HPP_INCLUDED

#include "PhysicsBody.h"  // Include the new PhysicsBody class
#include "EntityTypes.h"
#include <cstdlib>

#define SCALE 1.0f

#define ENTITY_NULL 0
typedef WorldEntity* WorldEntityPtr;
typedef uint64_t EntityID;

class GameWorld;

/// Represents every object in the GameWorld
class WorldEntity
{
public:
    WorldEntity();
    WorldEntity(float x, float y, float angle);
    WorldEntity(const WorldEntity& entity);

    virtual ~WorldEntity();

    bool          isDying()      const;
    float         getAngle()     const;
    EntityID      getID()        const;
    PhysicsBody& getBody();  // Change to PhysicsBody
    const Vec2& getCoord()     const;
    const Vec2& getBodyCoord() const;
    EntityTypes   getType()      const;
    virtual bool  isDone()       const = 0;

    void setID(EntityID id);
    void move(float vx, float vy);
    void setDying();
    void setEnemy();
    void resetTime();
    void addLife(float life);
    void setLife(float life);

    virtual void kill(GameWorld* world);
    virtual void initPhysics(GameWorld* world) = 0;
    virtual void hit(WorldEntity* entity, GameWorld* gameWorld) {};
    virtual void update(GameWorld& world) = 0;
    virtual void render() = 0;

    static uint32_t registerEntity();

protected:
    /// Engine data
    uint32_t _id;
    EntityID m_globalID;

    /// Physical data
    Vec2     m_coord;
    float    _angle;
    float    _time;
    float    _life;
    bool     _isDying;

    EntityTypes _type;

    PhysicsBody m_body;  // Change to use PhysicsBody instead of U_2DBody

    const PhysicsBody* m_thisBody() const;  // Change return type
    PhysicsBody* m_thisBody();  // Change return type
    PhysicsBody* m_initBody(GameWorld* world);

private:
    static uint32_t _entityCount;
    static uint32_t _entityTypeCount;

};

#endif // WORLDENTITY_HPP_INCLUDED

