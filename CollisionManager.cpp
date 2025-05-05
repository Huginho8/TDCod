#include "CollisionManager.h"
#include <iostream>
#include <SFML/Graphics.hpp>

#include "WorldEntity.h"
#include <System/GameWorld.hpp>

#include <thread>

GridCell::GridCell()
{
    _maxIndex = 0;
}

void GridCell::add(PhysicsBody* body)
{
    _bodies[_maxIndex] = body;
    _bodiesID[_maxIndex] = body->getIndex();
    ++_maxIndex;
}

void GridCell::reset()
{
    _maxIndex = 0;
}

CollisionManager::CollisionManager()
{
    m_timeStep = 1;
    m_bodySize = 1;
    m_mapSize = Vec2();

    m_iterationCount = 0;
    m_currentIteration = 0;
}

CollisionManager::CollisionManager(float timeStep, float bodyRadius, Vec2 mapSize, GameWorld* world) :
    _world(world)
{
    m_timeStep = timeStep;
    m_bodySize = bodyRadius * 2;
    m_mapSize = mapSize;

    m_iterationCount = 2;
    m_currentIteration = 0;
}

void CollisionManager::addBody(PhysicsBody* body)
{
    if (!body->getRadius())
        body->setRadius(m_bodySize * 0.45f);
}

long CollisionManager::convertPosToHash(int x, int y) const
{
    return x + y * 1000000000L;
}

void CollisionManager::addBodyToGrid(PhysicsBody* body)
{
    int bodyX = static_cast<int32_t>(body->getPosition().x);
    int bodyY = static_cast<int32_t>(body->getPosition().y);

    const int caseSize = static_cast<int32_t>(m_bodySize);
    const int gridX = static_cast<int32_t>(bodyX / caseSize);
    const int gridY = static_cast<int32_t>(bodyY / caseSize);
    const int midGrid = caseSize / 2;

    m_grid[convertPosToHash(gridX, gridY)].add(body);
    if (bodyX % caseSize > midGrid)
    {
        m_grid[convertPosToHash(gridX + 1, gridY)].add(body);
        if (bodyY % caseSize > midGrid)
        {
            m_grid[convertPosToHash(gridX, gridY + 1)].add(body);
            m_grid[convertPosToHash(gridX + 1, gridY + 1)].add(body);
        }
        else
        {
            m_grid[convertPosToHash(gridX, gridY - 1)].add(body);
            m_grid[convertPosToHash(gridX + 1, gridY - 1)].add(body);
        }
    }
    else
    {
        m_grid[convertPosToHash(gridX - 1, gridY)].add(body);
        if (bodyY % caseSize > midGrid)
        {
            m_grid[convertPosToHash(gridX, gridY + 1)].add(body);
            m_grid[convertPosToHash(gridX - 1, gridY + 1)].add(body);
        }
        else
        {
            m_grid[convertPosToHash(gridX, gridY - 1)].add(body);
            m_grid[convertPosToHash(gridX - 1, gridY - 1)].add(body);
        }
    }
}

void CollisionManager::solveBoundCollisions(PhysicsBody* body)
{
    const float maxX = m_mapSize.x;
    const float maxY = m_mapSize.y;
    const float bodyX = body->getPosition().x;
    const float bodyY = body->getPosition().y;

    const float radius = m_bodySize * 0.5f;

    if (bodyY + radius + 1 > maxY)
    {
        body->setY(maxY - radius);
    }
    if (bodyX + radius + 1 > maxX)
    {
        body->setX(maxX - radius);
    }
    if (bodyY - radius - 1 < 0)
    {
        const float delta = -bodyY + radius;
        body->move2D(Vec2(0, delta));
    }
    if (bodyX - radius < 0)
    {
        body->setX(radius);
    }
}

void CollisionManager::solveGridCollisions(GridCell& cell)
{
    int n_bodies = cell._maxIndex;
    PhysicsBody* bodies[1000];  // Adjusted to hold PhysicsBody pointers

    for (int i(0); i < n_bodies; ++i)
    {
        PhysicsBody* currentBody = bodies[i];

        const Vec2 currentPos(currentBody->getPosition());
        float currentMass = currentBody->getMass();
        float currentRadius = currentBody->getRadius();
        float currentX = currentPos.x;
        float currentY = currentPos.y;

        for (int k(i + 1); k < n_bodies; k++)
        {
            PhysicsBody* collider = bodies[k];
            float colliderRadius = collider->getRadius();

            float minDist = currentRadius + colliderRadius;
            float vx, vy, dist2;

            const Vec2 colliderPos(collider->getPosition());
            vx = currentX - colliderPos.x;
            vy = currentY - colliderPos.y;

            dist2 = vx * vx + vy * vy;

            if (dist2 && dist2 < minDist * minDist)
            {
                float dist = sqrt(dist2);
                float deltaDist = minDist - dist;
                float responseVelocity = 0.5f * deltaDist / dist;

                vx *= responseVelocity;
                vy *= responseVelocity;

                float colliderMass = collider->getMass();
                float totalMassCoeff = 1.0f / (currentMass + colliderMass);
                float massCoef1 = colliderMass * totalMassCoeff;
                float massCoef2 = currentMass * totalMassCoeff;

                currentBody->move2D(vx * massCoef1, vy * massCoef1);
                collider->move2D(-vx * massCoef2, -vy * massCoef2);
            }
        }
    }
}

void CollisionManager::solveCollisions()
{
    for (auto& elem : m_grid)
        solveGridCollisions(elem.second);

    PhysicsBody* b = nullptr;
    while (PhysicsBody::getNext(b))
    {
        solveBoundCollisions(b);
    }
}

void CollisionManager::update()
{
    m_currentIteration++;

    m_nCollisionChecked = 0;
    m_newHash = 0;

    for (auto& elem : m_grid) elem.second.reset();
    PhysicsBody* b = nullptr;
    while (PhysicsBody::getNext(b))
    {
        addBodyToGrid(b);
    }

    for (uint32_t i(0); i < m_iterationCount; ++i)
    {
        solveCollisions();
    }
}

PhysicsBody* CollisionManager::getBodyByID(BodyID id)
{
    return PhysicsBody::getObjectAt(static_cast<uint32_t>(id));
}
