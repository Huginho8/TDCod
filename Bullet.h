#pragma once

#include "Entity.h"
#include <unordered_set>

class Bullet : public Entity {
public:
    Bullet(Vec2 position, Vec2 velocity, float mass = 1.0f, int maxPenetrations = 1);

    void update(float dt) override;
    void onCollision(Entity* other) override;
    void render(sf::RenderWindow& window) override;

private:
    int remainingPenetrations;
    std::unordered_set<Entity*> hitEntities;

    sf::Sprite sprite;
    static sf::Texture texture; // Shared texture
    static bool textureLoaded;
};