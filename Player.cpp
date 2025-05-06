#include "Player.h"
#include <iostream>
#include <cmath> // Include cmath for atan2
#include <algorithm> // Include for std::min and std::max

Player::Player() : speed(100.0f), velocity(0.0f, 0.0f), currentFrame(0), animationTimer(0.0f), animationFrameTime(0.1f), isAttacking(false), currentAttackFrame(0), attackTimer(0.0f), attackFrameTime(0.05f), rotationSpeed(180.0f), scaleFactor(0.35f) { // Reduced scaleFactor
    // Load walking animation textures
    for (int i = 0; i < 20; ++i) {
        sf::Texture texture;
        std::string filePath = "Assets/Top_Down_Survivor/knife/move/survivor-move_knife_" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading player texture: " << filePath << std::endl;
        }
        walkingTextures.push_back(texture);
    }

    // Load attack animation textures
    for (int i = 0; i < 15; ++i) { // Assuming 15 frames for attack animation
        sf::Texture texture;
        std::string filePath = "Assets/Top_Down_Survivor/knife/meleeattack/survivor-meleeattack_knife_" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading player attack texture: " << filePath << std::endl;
        }
        attackTextures.push_back(texture);
    }


    sprite.setTexture(walkingTextures[0]);
    sprite.setPosition(100, 100);
    // Set origin to the center for rotation
    sf::Vector2u textureSize = walkingTextures[0].getSize();
    sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    sprite.setScale(scaleFactor, scaleFactor); // Apply scale factor
}

void Player::update(float deltaTime, sf::RenderWindow& window, sf::Vector2u mapSize) { // Pass window and mapSize
    if (isAttacking) {
        attackTimer += deltaTime;
        if (attackTimer >= attackFrameTime) {
            attackTimer -= attackFrameTime;
            currentAttackFrame++;
            if (currentAttackFrame < attackTextures.size()) {
                sprite.setTexture(attackTextures[currentAttackFrame]);
            } else {
                isAttacking = false;
                currentAttackFrame = 0;
                // After attack, revert to idle or walking frame
                if (velocity.x == 0 && velocity.y == 0) {
                     sprite.setTexture(walkingTextures[0]); // Assuming first frame of walking is idle
                } else {
                     sprite.setTexture(walkingTextures[currentFrame]);
                }
            }
        }
    } else {
        // Movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            velocity.x = -speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            velocity.x = speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            velocity.y = -speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            velocity.y = speed;
        }

        // Normalize diagonal movement
        if (velocity.x != 0 && velocity.y != 0) {
            velocity.x *= 0.707f; // 1/sqrt(2)
            velocity.y *= 0.707f;
        }

        float newX = sprite.getPosition().x + velocity.x * deltaTime;
        float newY = sprite.getPosition().y + velocity.y * deltaTime;

        // Clamp the position to the map boundaries
        //newX = std::max(0.f, std::min(newX, static_cast<float>(mapSize.x - sprite.getGlobalBounds().width)));
        //newY = std::max(0.f, std::min(newY, static_cast<float>(mapSize.y - sprite.getGlobalBounds().height)));

        sprite.setPosition(newX, newY);

        // Rotation - Face Mouse
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f worldMousePosition = window.mapPixelToCoords(mousePosition);
        sf::Vector2f playerPosition = sprite.getPosition();
        sf::Vector2f direction = worldMousePosition - playerPosition;

        float angle = atan2(direction.y, direction.x) * 180 / 3.14159265f;
        sprite.setRotation(angle); // Corrected offset

        velocity.x = 0;
        velocity.y = 0;

        // Animation
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            animationTimer += deltaTime;
            if (animationTimer >= animationFrameTime) {
                animationTimer -= animationFrameTime;
                currentFrame = (currentFrame + 1) % walkingTextures.size();
                sprite.setTexture(walkingTextures[currentFrame]);
            }
        } else {
            // If not moving, reset to the first frame
            currentFrame = 0;
            sprite.setTexture(walkingTextures[currentFrame]);
        }
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Player::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

sf::Vector2f Player::getPosition() const {
    return sprite.getPosition();
}

void Player::attack() {
    if (!isAttacking) {
        isAttacking = true;
        currentAttackFrame = 0;
        attackTimer = 0.0f;
        if (!attackTextures.empty()) {
            sprite.setTexture(attackTextures[currentAttackFrame]);
        }
    }
}
sf::Sprite& Player::getSprite() {
    return sprite;
}
