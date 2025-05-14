#pragma once

#include <vector>
#include <memory>
#include "Vec2.h"
#include "PhysicsWorld.h"
#include "Entity.h"
#include "Player.h"
#include "Zombie.h"
#include "Bullet.h"

class GameRender;

class GameWorld {
public:
    PhysicsWorld physics;
    std::vector<std::unique_ptr<Entity>> entities;

    Player* player = nullptr; // Raw pointer to the player (owned by unique_ptr)

    void init();
    void update(float dt, sf::RenderWindow& window);
    void render(sf::RenderWindow& window);
    void draw(const Entity& entity, sf::RenderWindow& window);
    void spawnBullet(const Vec2& position, const Vec2& velocity, float mass, int maxPenetrations);
       
};
