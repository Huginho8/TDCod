#include "Player.h"
#include "GameWorld.h"
#include <SFML/Window/Keyboard.hpp>

Player::Player(GameWorld* world, Vec2 position)
    : Entity(EntityType::Player, position, Vec2(32, 32), false, 2.0f, true), gameWorld(world) {
    body.getShape().setFillColor(sf::Color::Red);
    getBody().isCircle = true;
}

void Player::update(float dt) {
    Entity::update(dt); 
}

void Player::handleInput(float dt, sf::RenderWindow& window) {
    shootTimer += dt;

    Vec2 input(0, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) input.y -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) input.y += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) input.x -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) input.x += 1;

    if (input.length() > 0) {
        input.normalize();
        body.velocity = input * speed;
    }
    else {
        body.velocity = Vec2(0, 0);
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && shootTimer >= shootCooldown) {
        // Get mouse position in window coordinates
        sf::Vector2i mousePosPixels = sf::Mouse::getPosition(window);

        // Convert to world coordinates
        sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePosPixels);

        // Calculate direction vector from player to mouse
        Vec2 direction = Vec2(mouseWorld.x, mouseWorld.y) - body.position;

        // Calculate angle in radians
        float angle = std::atan2(direction.y, direction.x);

        // Convert angle to unit vector
        Vec2 bulletDir(std::cos(angle), std::sin(angle));

        // Scale velocity by speed
        float bulletSpeed = 400.f;
        Vec2 bulletVelocity = bulletDir * bulletSpeed;

        // Shoot bullet
        if (gameWorld) {
            gameWorld->spawnBullet(body.position, bulletVelocity, 1.0f, 2);  // Mass = 1.0
        }

        shootTimer = 0.0f;
    }
    shootTimer += dt;

}
