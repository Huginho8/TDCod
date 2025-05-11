#include "GameRender.h"
#include "GameWorld.h"
#include "Entity.h"

void GameRender::render(sf::RenderWindow& window, const GameWorld& world) {
    for (const auto& entity : world.entities) {
        if (entity->isAlive()) {
            draw(*entity, window);
        }
    }
}

void GameRender::draw(const Entity& entity, sf::RenderWindow& window) {
    window.draw(entity.getBody().getShape());
}
