#pragma once
#include "Entity.h"

class Player : public Entity {
public:
    Player(Vec2 position);

    void update(float dt) override;
    void handleInput(float dt);

private:
    float speed = 200.f;
    float shootCooldown = 0.3f;
    float shootTimer = 0.f;
};