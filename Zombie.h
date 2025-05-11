#pragma once
#include "Entity.h"

class Zombie : public Entity {
public:
    Zombie(Vec2 position, float speed);

    void update(float dt) override;
    void onCollision(Entity* other) override;

    void setTarget(Entity* target);

private:
    float speed;
    Entity* target;
};
