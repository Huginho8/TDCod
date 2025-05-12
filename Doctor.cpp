#include "Doctor.h"
#include <iostream>
#include <cmath>
#include <random>

Doctor::Doctor() : position(0, 0), currentState(DoctorState::IDLE), speed(70.0f), 
                  idleTimer(0.0f), moveTimer(0.0f), currentFrame(0), 
                  animationTimer(0.0f), animationFrameTime(0.1f) {
    loadTextures();
}

Doctor::Doctor(float x, float y) : position(x, y), currentState(DoctorState::IDLE), speed(70.0f), 
                                  idleTimer(0.0f), moveTimer(0.0f), currentFrame(0), 
                                  animationTimer(0.0f), animationFrameTime(0.1f) {
    loadTextures();
    
    // Set initial position
    sprite.setPosition(position);
    
    // Initialize with first idle texture
    if (!idleTextures.empty()) {
        sprite.setTexture(idleTextures[0]);
        sf::Vector2u textureSize = idleTextures[0].getSize();
        sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        sprite.setScale(0.5f, 0.5f); // Scale doctor to be appropriately sized
    }
}

void Doctor::loadTextures() {
    // Load idle textures
    for (int i = 0; i < 8; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/Doctor/DoctorIdle/Idle_knife_00" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading doctor idle texture: " << filePath << std::endl;
            // Use placeholder texture if loading fails
            texture.create(64, 64);
        }
        idleTextures.push_back(texture);
    }
    
    // Load walk textures
    for (int i = 0; i < 6; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/Doctor/DoctorWalk/Walk_knife_00" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading doctor walk texture: " << filePath << std::endl;
            // Use placeholder texture if loading fails
            texture.create(64, 64);
        }
        walkTextures.push_back(texture);
    }
}

void Doctor::update(float deltaTime, sf::Vector2f playerPosition) {
    // Calculate distance to player
    sf::Vector2f direction = playerPosition - position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    // If player is in range but not too close, move towards them
    if (distance < FOLLOW_DISTANCE && distance > MIN_DISTANCE) {
        setState(DoctorState::WALK);
        
        // Normalize direction
        if (distance > 0) {
            direction /= distance;
        }
        
        // Move towards player
        position.x += direction.x * speed * deltaTime;
        position.y += direction.y * speed * deltaTime;
        
        // Rotate to face player
        float angle = std::atan2(direction.y, direction.x) * 180 / 3.14159265f;
        sprite.setRotation(angle);
    } else {
        // Random wandering behavior when idle
        idleTimer += deltaTime;
        
        if (currentState == DoctorState::IDLE && idleTimer > 3.0f) {
            setState(DoctorState::WALK);
            idleTimer = 0.0f;
            moveTimer = 0.0f;
            
            // Generate random direction
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> distribAngle(0, 2 * 3.14159265f);
            float angle = distribAngle(gen);
            
            moveDirection.x = std::cos(angle);
            moveDirection.y = std::sin(angle);
            
            // Set rotation
            sprite.setRotation(angle * 180 / 3.14159265f);
        } else if (currentState == DoctorState::WALK) {
            moveTimer += deltaTime;
            
            if (moveTimer > 2.0f) {
                setState(DoctorState::IDLE);
                moveTimer = 0.0f;
            } else {
                // Continue moving in random direction
                position.x += moveDirection.x * speed * deltaTime;
                position.y += moveDirection.y * speed * deltaTime;
            }
        }
    }
    
    // Update sprite position
    sprite.setPosition(position);
    
    // Update animation
    updateAnimation(deltaTime);
}

void Doctor::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

sf::Vector2f Doctor::getPosition() const {
    return position;
}

sf::FloatRect Doctor::getBounds() const {
    return sprite.getGlobalBounds();
}

void Doctor::setState(DoctorState newState) {
    if (currentState != newState) {
        currentState = newState;
        currentFrame = 0;
        animationTimer = 0.0f;
        
        // Set texture based on state
        if (currentState == DoctorState::IDLE && !idleTextures.empty()) {
            sprite.setTexture(idleTextures[0]);
        } else if (currentState == DoctorState::WALK && !walkTextures.empty()) {
            sprite.setTexture(walkTextures[0]);
        }
    }
}

void Doctor::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;
    
    if (animationTimer >= animationFrameTime) {
        animationTimer -= animationFrameTime;
        
        // Update frame based on current state
        if (currentState == DoctorState::IDLE) {
            currentFrame = (currentFrame + 1) % idleTextures.size();
            if (!idleTextures.empty()) {
                sprite.setTexture(idleTextures[currentFrame]);
            }
        } else if (currentState == DoctorState::WALK) {
            currentFrame = (currentFrame + 1) % walkTextures.size();
            if (!walkTextures.empty()) {
                sprite.setTexture(walkTextures[currentFrame]);
            }
        }
    }
}
