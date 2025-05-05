#include "PhysicsBody.h"
#include "WorldEntity.h"
#include <cmath>
#include <iostream>

PhysicsBody::PhysicsBody()
{
    init(Vec2(), 0, false);
}

PhysicsBody::PhysicsBody(Vec2 position, float mass, bool included)
{
    init(position, mass, included);
    m_radius = 0.0;
}

void PhysicsBody::init(Vec2 position, float mass, bool included)
{
    m_position = position;
    m_lastPosition = position;
    m_mass = mass;
    m_acceleration = Vec2();
    m_static = false;
    m_included = included;
    _pressure = Vec2();
    m_gridChange = true;
}

void PhysicsBody::move2D(const Vec2& vec)
{
    if (!m_static)
        m_position.move2D(vec);
}

void PhysicsBody::moveLastPos2D(const Vec2& vec)
{
    m_lastPosition.move2D(vec);
}

void PhysicsBody::accelerate2D(const Vec2& vec)
{
    m_acceleration.move2D(vec);
}

void PhysicsBody::accelerate2D(float ax, float ay)
{
    m_acceleration.x += ax;
    m_acceleration.y += ay;
}

void PhysicsBody::updatePosition(float timeStep)
{
    if (!m_static)
    {
        Vec2 newPosition;
        newPosition.x = m_position.x + (m_position.x - m_lastPosition.x) + timeStep * m_acceleration.x;
        newPosition.y = m_position.y + (m_position.y - m_lastPosition.y) + timeStep * m_acceleration.y;

        m_lastPosition = m_position;
        m_position = newPosition;
    }
    else
    {
        m_position = m_lastPosition;
    }

    // Reset acceleration after position update
    m_acceleration = {};
}

Vec2 PhysicsBody::getVelocity() const
{
    return Vec2(m_position.x - m_lastPosition.x, m_position.y - m_lastPosition.y);
}

void PhysicsBody::setEntity(WorldEntity* entity)
{
    _entity = entity;
}

WorldEntity* PhysicsBody::getEntity()
{
    return _entity;
}

void PhysicsBody::setPosition(const Vec2& pos)
{
    m_position = pos;
}

void PhysicsBody::setX(float x)
{
    m_position.x = x;
}

void PhysicsBody::setY(float y)
{
    m_position.y = y;
}

void PhysicsBody::stop()
{
    m_lastPosition = m_position;
}

void PhysicsBody::setStatic(bool isStatic)
{
    m_static = isStatic;
}

void PhysicsBody::setPressure(Vec2 pressure)
{
    _pressure = pressure;
}

void PhysicsBody::setMass(float mass)
{
    m_mass = mass;
}

void PhysicsBody::setRadius(float radius)
{
    m_radius = radius;
}

void PhysicsBody::addPressure(Vec2 pressure)
{
    _pressure.x += pressure.x;
    _pressure.y += pressure.y;
}

const Vec2& PhysicsBody::getPosition() const
{
    return m_position;
}

bool PhysicsBody::isStatic() const
{
    return m_static;
}

bool PhysicsBody::isIncluded() const
{
    return m_included;
}

float PhysicsBody::getMass() const
{
    return m_mass;
}

float PhysicsBody::getRadius() const
{
    return m_radius;
}

float PhysicsBody::getAngle(PhysicsBody* body)
{
    Vec2 v = m_position - body->getPosition();

    float dist = v.getNorm();
    float angle = acos(v.x / dist);

    return v.y > 0 ? angle : -angle;
}

void PhysicsBody::move2D(float vx, float vy)
{
    m_position.x += vx;
    m_position.y += vy;
}

void PhysicsBody::setPosition(float x, float y)
{
    m_position.x = x;
    m_position.y = y;
}