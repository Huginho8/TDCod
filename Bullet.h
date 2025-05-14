#pragma once

#include "Entity.h"
#include <unordered_set>

class Bullet : public Entity {
public:
    Bullet(Vec2 position, Vec2 velocity, float mass = 1.0f, int maxPenetrations = 1);

    void update(float dt) override;
    void onCollision(Entity* other) override;

private:
    int remainingPenetrations;
    std::unordered_set<Entity*> hitEntities;
};
