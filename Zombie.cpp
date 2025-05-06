#include "Zombie.h"
#include <iostream>
#include <cmath>

Zombie::Zombie(float x, float y) : speed(50.0f), position(x, y), currentFrame(0), animationTimer(0.0f), animationFrameTime(0.1f) {
    // Load walking animation textures
    for (int i = 0; i < 32; ++i) { // Assuming 32 frames for run animation
        sf::Texture texture;
        std::string filePath = "Assets/zombie_01/run/run00";
        if (i < 10) {
            filePath += "0";
        }
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie texture: " << filePath << std::endl;
        }
        walkingTextures.push_back(texture);
    }
    sprite.setTexture(walkingTextures[0]);
    sprite.setPosition(position);
    // Set origin to the center for rotation (optional, but good practice for rotation)
    sf::Vector2u textureSize = walkingTextures[0].getSize();
    sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
}

void Zombie::update(float deltaTime, sf::Vector2f playerPosition) {
    // Calculate direction towards player
    sf::Vector2f direction = playerPosition - position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 0) {
        direction /= distance; // Normalize
        position.x += direction.x * speed * deltaTime;
        position.y += direction.y * speed * deltaTime;
        sprite.setPosition(position);

        // Rotate zombie to face player
        float angle = atan2(direction.y, direction.x) * 180 / 3.14159265f;
        sprite.setRotation(angle); // Removed + 90 offset
    }

    // Animation
    animationTimer += deltaTime;
    if (animationTimer >= animationFrameTime) {
        animationTimer -= animationFrameTime;
        currentFrame = (currentFrame + 1) % walkingTextures.size();
        sprite.setTexture(walkingTextures[currentFrame]);
    }
}

void Zombie::draw(sf::RenderWindow& window) const { // Marked as const
    window.draw(sprite);
}

sf::FloatRect Zombie::getBounds() const {
    return sprite.getGlobalBounds();
}