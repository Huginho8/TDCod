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
    auto p = std::make_unique<Player>(Vec2(400, 300));
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

void GameWorld::update(float dt) {
    for (auto& e : entities) {
        if (e->isAlive()) {
            e->update(dt);
        }
    }
    physics.update(dt);
}

void GameWorld::render(GameRender& renderer, sf::RenderWindow& window) {
    for (auto& e : entities) {
        if (e->isAlive()) {
            renderer.draw(*e, window);
        }
    }
}
