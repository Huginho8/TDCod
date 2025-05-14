#include "GameWorld.h"
#include "GameRender.h"

void GameWorld::init() {
    float thickness = 50;
    float width = 800, height = 600;

    // Box walls: Top, Bottom, Left, Right
    std::vector<Vec2> wallPositions = {
        {width / 2, thickness / 2},           // Top
        {width / 2, height - thickness / 2},  // Bottom
        {thickness / 2, height / 2},          // Left
        {width - thickness / 2, height / 2}   // Right

    };

    std::vector<Vec2> wallSizes = {
        {width, thickness},
        {width, thickness},
        {thickness, height},
        {thickness, height}
    };

    for (size_t i = 0; i < wallPositions.size(); ++i) {
        auto wall = std::make_unique<Entity>(EntityType::Wall, wallPositions[i], wallSizes[i], true, 0, false);
        physics.addBody(&wall->getBody(), true);
        entities.push_back(std::move(wall));
    }

    // Create player
    auto p = std::make_unique<Player>(this, Vec2(400, 300));
    player = p.get(); // Save raw pointer for target assignment
    physics.addBody(&p->getBody(), false);
    entities.push_back(std::move(p));

    // Create 10 zombies
    for (int i = 0; i < 10; ++i) {
        auto z = std::make_unique<Zombie>(Vec2(100 + i * 50, 100), 30.0f);
        z->setTarget(player);
        physics.addBody(&z->getBody(), false);
        entities.push_back(std::move(z));
    }
}

void GameWorld::spawnBullet(const Vec2& position, const Vec2& velocity, float mass, int maxPenetrations) {
    std::unique_ptr<Bullet> bullet = std::make_unique<Bullet>(position, velocity, mass, maxPenetrations);
    physics.addBody(&bullet->getBody(), false); // Add to physics engine
    entities.push_back(std::move(bullet));
}

void GameWorld::update(float dt, sf::RenderWindow& window) {
    // Handle input only for the player
    if (player) {
        player->handleInput(dt, window);
    }

    // Update all entities
    for (auto& e : entities) {
        if (e->isAlive()) {
            e->update(dt);
        }
    }

    physics.update(dt);
}


void GameWorld::render(sf::RenderWindow& window) {
    for (auto& e : entities) {
        if (e->isAlive()) {
            draw(*e, window);
        }
    }
}

void GameWorld::draw(const Entity& entity, sf::RenderWindow& window) {
    // Example: use getType() to specialize drawing
    switch (entity.getType()) {
    case EntityType::Bullet:
    case EntityType::Enemy:
    case EntityType::Player:
    case EntityType::Wall:
    default:
        window.draw(entity.getBody().getShape());
        break;
    }
}
