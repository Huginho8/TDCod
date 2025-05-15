#include <math.h>
#include "ZombieKing.h"
#include <iostream>
#include <cmath>

ZombieKing::ZombieKing(float x, float y)
    : BaseZombie(x, y, 200.0f, 20.0f, 20.0f, 50.0f, 2.5f),
      useSecondAttack(false),
      currentAttack2Frame(0),
      attack2Timer(0.0f),
      specialAttackCooldown(5.0f),
      timeSinceLastSpecialAttack(0.0f) {
    loadTextures();
    
    // Set initial texture to walk
    if (!walkTextures.empty()) {
        sprite.setTexture(walkTextures[0]);
        sf::Vector2u textureSize = walkTextures[0].getSize();
        sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        sprite.setScale(0.5f, 0.5f); // Larger scale for boss zombie
        sprite.setPosition(position);
    }
}

void ZombieKing::update(float deltaTime, sf::Vector2f playerPosition) {
    if (dead) {
        updateAnimation(deltaTime);
        return;
    }

    // Update both regular and special attack cooldown timers
    timeSinceLastAttack += deltaTime;
    timeSinceLastSpecialAttack += deltaTime;

    // Calculate direction and distance to player
    sf::Vector2f direction = playerPosition - position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Decide whether to use special attack or regular attack
    if (distance <= attackRange && !attacking) {
        if (timeSinceLastSpecialAttack >= specialAttackCooldown && useSecondAttack) {
            attack(); // Trigger special attack
            timeSinceLastSpecialAttack = 0.0f;
            useSecondAttack = false; // Toggle to regular attack next
        } else if (timeSinceLastAttack >= attackCooldown) {
            attack(); // Trigger regular attack
            timeSinceLastAttack = 0.0f;
            useSecondAttack = true; // Toggle to special attack next if cooldown allows
        }
    } else if (!attacking) {
        // Move towards player if not attacking
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
            sprite.setRotation(angle - 90.0f);
        }
    }

    // Update animation
    updateAnimation(deltaTime);
}

void ZombieKing::attack() {
    if (!attacking && !dead) {
        attacking = true;
        currentAttackFrame = 0;
        currentAttack2Frame = 0;
        attackTimer = 0.0f;
        m_hasDealtDamageInAttack = false;
        setState(ZombieState::ATTACK);

        // Choose texture based on attack type
        if (useSecondAttack && !attack2Textures.empty()) {
            sprite.setTexture(attack2Textures[currentAttack2Frame]);
            attackDamage = 30.0f; // Higher damage for special attack
        } else if (!attackTextures.empty()) {
            sprite.setTexture(attackTextures[currentAttackFrame]);
            attackDamage = 20.0f; // Regular attack damage
        }
    }
}

void ZombieKing::loadTextures() {
    // Load walk animation textures
    for (int i = 0; i < 8; ++i) { // Changed from 9 to 8
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieKing/Walk/Walk_00";
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie king walk texture: " << filePath << std::endl;
        }
        walkTextures.push_back(texture);
    }

    // Load regular attack animation textures
    for (int i = 0; i < 10; ++i) { // Changed from 16 to 10
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieKing/Attack1/attack1_";
        if (i < 10) filePath += "00"; // Add two leading zeros for 0-9
        else filePath += "0"; // Add one leading zero for 10-15
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie king attack texture: " << filePath << std::endl;
        }
        attackTextures.push_back(texture);
    }

    // Load special attack animation textures
    for (int i = 0; i < 9; ++i) { // This loop seems correct based on the previous listing
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieKing/Attack2/Attack4_00";
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie king special attack texture: " << filePath << std::endl;
        }
        attack2Textures.push_back(texture);
    }

    // Load death animation textures
    for (int i = 0; i < 15; ++i) { // Changed from 17 to 15
        if (i == 1 || i == 5) continue; // Skip missing frames
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieKing/Death/Death_";
        if (i < 10) filePath += "00"; // Add two leading zeros for 0-9
        else filePath += "0"; // Add one leading zero for 10-14
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie king death texture: " << filePath << std::endl;
        }
        deathTextures.push_back(texture);
    }
}
