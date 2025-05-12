#include "Zombie.h"
#include <iostream>
#include <cmath>

Zombie::Zombie(float x, float y) 
    : position(x, y),
      currentState(ZombieState::WALK),
      speed(50.0f),
      currentFrame(0),
      animationTimer(0.0f),
      animationFrameTime(0.1f),
      attacking(false),
      currentAttackFrame(0),
      attackTimer(0.0f),
      attackFrameTime(0.1f),
      attackCooldown(2.0f),
      timeSinceLastAttack(0.0f),
      attackDamage(10.0f),
      attackRange(60.0f),
      dead(false),
      currentDeathFrame(0),
      deathTimer(0.0f),
      deathFrameTime(0.15f),
      health(30.0f),
      maxHealth(30.0f) {
          
    loadTextures();
    
    // Set initial texture to walk
    if (!walkTextures.empty()) {
        sprite.setTexture(walkTextures[0]);
        sf::Vector2u textureSize = walkTextures[0].getSize();
        sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        sprite.setScale(0.4f, 0.4f); // Scale to appropriate size
        sprite.setPosition(position);
    }
}

void Zombie::loadTextures() {
    // Load walk animation textures
    for (int i = 0; i < 9; ++i) { // Assuming 32 frames for run animation
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieWalker/Walk/walk_00";
        if (i < 10) {
            filePath += "0";
        }
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie walk texture: " << filePath << std::endl;
        }
        walkTextures.push_back(texture);
    }
    
    // Load attack animation textures
    for (int i = 0; i < 9; ++i) { // Assuming 32 frames for attack animation
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieWalker/Attack/Attack_00";
        if (i < 10) {
            filePath += "0";
        }
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie attack texture: " << filePath << std::endl;
        }
        attackTextures.push_back(texture);
    }
    
    // Load death animation textures
    for (int i = 0; i < 6; ++i) { // Assuming 30 frames for death animation
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieWalker/Death/Death_00";
        if (i < 10) {
            filePath += "0";
        }
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie death texture: " << filePath << std::endl;
        }
        deathTextures.push_back(texture);
    }
}

void Zombie::update(float deltaTime, sf::Vector2f playerPosition) {
    // Don't update if dead (except for death animation)
    if (dead) {
        updateAnimation(deltaTime);
        return;
    }
    
    // Calculate direction and distance to player
    sf::Vector2f direction = playerPosition - position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    // Update attack cooldown timer
    timeSinceLastAttack += deltaTime;
    
    // Check if within attack range
    if (distance <= attackRange && timeSinceLastAttack >= attackCooldown) {
        attack();
        timeSinceLastAttack = 0.0f;
    } else if (!attacking) {
        // Move towards player if not attacking and not already on player
        if (distance > 5.0f) {
            setState(ZombieState::WALK);
            
            // Normalize direction
            if (distance > 0) {
                direction /= distance;
            }
            
            // Move towards player
            position.x += direction.x * speed * deltaTime;
            position.y += direction.y * speed * deltaTime;
            sprite.setPosition(position);
            
            // Rotate to face player
            float angle = std::atan2(direction.y, direction.x) * 180 / 3.14159265f;
            sprite.setRotation(angle);
        }
    }
    
    // Update animation
    updateAnimation(deltaTime);
}

void Zombie::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

sf::FloatRect Zombie::getBounds() const {
    return sprite.getGlobalBounds();
}

void Zombie::attack() {
    if (!attacking && !dead) {
        attacking = true;
        currentAttackFrame = 0;
        attackTimer = 0.0f;
        setState(ZombieState::ATTACK);
        
        if (!attackTextures.empty()) {
            sprite.setTexture(attackTextures[currentAttackFrame]);
        }
    }
}

void Zombie::takeDamage(float amount) {
    if (!dead) {
        health -= amount;
        if (health <= 0) {
            health = 0;
            kill();
        }
    }
}

void Zombie::kill() {
    if (!dead) {
        dead = true;
        attacking = false;
        currentDeathFrame = 0;
        deathTimer = 0.0f;
        setState(ZombieState::DEATH);
        
        if (!deathTextures.empty()) {
            sprite.setTexture(deathTextures[currentDeathFrame]);
        }
    }
}

bool Zombie::isAttacking() const {
    return attacking;
}

bool Zombie::isDead() const {
    return dead;
}

float Zombie::getAttackDamage() const {
    return attackDamage;
}

void Zombie::setState(ZombieState newState) {
    if (currentState != newState && !dead) {
        currentState = newState;
        
        // Reset animation for the new state
        if (currentState != ZombieState::ATTACK) {
            currentFrame = 0;
            animationTimer = 0.0f;
        }
    }
}

void Zombie::updateAnimation(float deltaTime) {
    if (dead) {
        // Death animation
        deathTimer += deltaTime;
        if (deathTimer >= deathFrameTime) {
            deathTimer -= deathFrameTime;
            if (currentDeathFrame < deathTextures.size() - 1) {
                currentDeathFrame++;
                if (!deathTextures.empty()) {
                    sprite.setTexture(deathTextures[currentDeathFrame]);
                }
            }
        }
        return;
    }
    
    if (attacking) {
        // Attack animation
        attackTimer += deltaTime;
        if (attackTimer >= attackFrameTime) {
            attackTimer -= attackFrameTime;
            currentAttackFrame++;
            
            if (currentAttackFrame < attackTextures.size()) {
                sprite.setTexture(attackTextures[currentAttackFrame]);
            } else {
                // Attack animation finished
                attacking = false;
                
                // Revert to walk state
                setState(ZombieState::WALK);
                if (!walkTextures.empty()) {
                    sprite.setTexture(walkTextures[currentFrame]);
                }
            }
        }
    } else {
        // Walk animation
        animationTimer += deltaTime;
        if (animationTimer >= animationFrameTime) {
            animationTimer -= animationFrameTime;
            currentFrame = (currentFrame + 1) % walkTextures.size();
            if (!walkTextures.empty()) {
                sprite.setTexture(walkTextures[currentFrame]);
            }
        }
    }
}