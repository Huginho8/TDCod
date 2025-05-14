#pragma once
#include "Entity.h"

class GameWorld;

class Player : public Entity {
public:
    Player(GameWorld* world, Vec2 position);

    void update(float dt) override;
    void handleInput(float dt, sf::RenderWindow& window);

private:
    float speed = 100.f;
    float shootCooldown = 0.3f;
    float shootTimer = 0.f;

    GameWorld* gameWorld;
};