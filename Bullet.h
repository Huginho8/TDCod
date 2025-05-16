#ifndef BULLET_H
#define BULLET_H

#include "Entity.h"
#include <unordered_set>

class Bullet : public Entity {
public:
    Bullet(Vec2 position, Vec2 velocity, float damage, float mass = 1.0f, int maxPenetrations = 1);

    void update(float dt) override;
    void onCollision(Entity* other) override;

    float getDamage() const { return damage; }

private:
    int remainingPenetrations;
    float damage;
    std::unordered_set<Entity*> hitEntities;
};

#endif