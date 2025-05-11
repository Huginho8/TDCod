#include "Player.h"
#include <SFML/Window/Keyboard.hpp>

Player::Player(Vec2 position)
    : Entity(EntityType::Player, position, Vec2(32, 32), false, 1.3f, true) {
    body.getShape().setFillColor(sf::Color::Red);
    getBody().isCircle = true;
}

void Player::update(float dt) {
    shootTimer += dt;
    handleInput(dt);
    Entity::update(dt); // Update position and physics
}

void Player::handleInput(float dt) {
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
}
