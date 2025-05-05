#include "WorldEntity.h"
#include "System/GameWorld.hpp"
#include <iostream>

uint32_t WorldEntity::_entityCount = 0;
uint32_t WorldEntity::_entityTypeCount = 0;

WorldEntity::WorldEntity() :
    _id(WorldEntity::_entityCount++),
    m_coord(0, 0),
    _angle(0.0)
{
    _isDying = false;
}

WorldEntity::WorldEntity(float x, float y, float angle) :
    _id(WorldEntity::_entityCount++),
    m_coord(x, y),
    _angle(angle)
{
    _isDying = false;
}

WorldEntity::WorldEntity(const WorldEntity& entity)
{
    _id = WorldEntity::_entityCount++;

    _angle = entity._angle;
    _time = entity._time;
    _life = entity._life;
    _isDying = entity._isDying;
    _type = entity._type;
}

WorldEntity::~WorldEntity()
{

}

void WorldEntity::move(float vx, float vy)
{
    PhysicsBody* body = m_thisBody();
    body->stop();
    body->accelerate2D(vx, vy);
}

PhysicsBody* WorldEntity::m_initBody(GameWorld* world)
{
    m_bodyID = static_cast<uint32_t>(world->addBody());
    PhysicsBody* body = m_thisBody();

    body->setEntity(this);
    body->setPosition(m_coord);
    body->stop();

    return body;
}

void WorldEntity::setID(EntityID id)
{
    m_globalID = id;
}

const PhysicsBody* WorldEntity::m_thisBody() const
{
    return &m_body;  // Return the member PhysicsBody
}

PhysicsBody* WorldEntity::m_thisBody()
{
    return &m_body;  // Return the member PhysicsBody
}

const Vec2& WorldEntity::getBodyCoord() const
{
    return m_thisBody()->getPosition();
}

void WorldEntity::kill(GameWorld* world)
{
    // deleter
}

uint32_t WorldEntity::registerEntity()
{
    return _entityTypeCount++;
}

void WorldEntity::setDying()
{
    _isDying = true;
}

bool WorldEntity::isDying() const
{
    return _isDying;
}

PhysicsBody& WorldEntity::getBody()
{
    return m_body;  // Return the member PhysicsBody
}

const Vec2& WorldEntity::getCoord() const
{
    return m_coord;
}

EntityID WorldEntity::getID() const
{
    return m_globalID;
}

float WorldEntity::getAngle() const
{
    return _angle;
}

EntityTypes WorldEntity::getType() const
{
    return _type;
}

void WorldEntity::addLife(float life)
{
    _life += life;
}

void WorldEntity::setLife(float life)
{
    _life = life;
}

void WorldEntity::resetTime()
{
    _time = 0;
}
