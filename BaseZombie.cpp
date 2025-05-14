#include "BaseZombie.h"
#include <iostream>
#include <cmath>

BaseZombie::BaseZombie(float x, float y, float health, float attackDamage, float speed, float attackRange, float attackCooldown)
    : position(x, y),
      currentState(ZombieState::WALK),
      speed(speed),
      currentFrame(0),
      animationTimer(0.0f),
      animationFrameTime(0.1f),
      attacking(false),
      currentAttackFrame(0),
      attackTimer(0.0f),
      attackFrameTime(0.1f),
      attackCooldown(attackCooldown),
      timeSinceLastAttack(0.0f),
      attackDamage(attackDamage),
      attackRange(attackRange),
      dead(false),
      currentDeathFrame(0),
      deathTimer(0.0f),
      deathFrameTime(0.15f),
      health(health),
      maxHealth(health),
      m_hasDealtDamageInAttack(false) {
}

void BaseZombie::update(float deltaTime, sf::Vector2f playerPosition) {
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
            sprite.setRotation(angle - 90.0f);
        }
    }

    // Update animation
    updateAnimation(deltaTime);
}

void BaseZombie::draw(sf::RenderWindow& window) const {
    window.draw(sprite);

    // Debug hitbox visualization - uncomment if needed for debugging
    /*
    sf::RectangleShape hitboxShape;
    sf::FloatRect hitbox = getHitbox();
    hitboxShape.setPosition(hitbox.left, hitbox.top);
    hitboxShape.setSize(sf::Vector2f(hitbox.width, hitbox.height));
    hitboxShape.setFillColor(sf::Color(255, 0, 255, 70));
    hitboxShape.setOutlineColor(sf::Color::Magenta);
    hitboxShape.setOutlineThickness(1);
    window.draw(hitboxShape);
    */
}

sf::FloatRect BaseZombie::getBounds() const {
    return sprite.getGlobalBounds();
}

void BaseZombie::attack() {
    if (!attacking && !dead) {
        attacking = true;
        currentAttackFrame = 0;
        attackTimer = 0.0f;
        m_hasDealtDamageInAttack = false;
        setState(ZombieState::ATTACK);

        if (!attackTextures.empty()) {
            sprite.setTexture(attackTextures[currentAttackFrame]);
        }
    }
}

void BaseZombie::takeDamage(float amount) {
    if (!dead) {
        health -= amount;
        if (health <= 0) {
            health = 0;
            kill();
        }
    }
}

void BaseZombie::kill() {
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

bool BaseZombie::isAttacking() const {
    return attacking;
}

bool BaseZombie::isDead() const {
    return dead;
}

bool BaseZombie::isAlive() const {
    return !dead;
}

float BaseZombie::getAttackDamage() const {
    return attackDamage;
}

sf::Vector2f BaseZombie::getPosition() const {
    return position;
}

sf::FloatRect BaseZombie::getHitbox() const {
    // Define desired hitbox size in scaled pixels
    const float desiredScaledWidth = 60.0f;
    const float desiredScaledHeight = 60.0f;
    const float zombieScaleFactor = 0.4f;

    // Calculate the equivalent local (unscaled) dimensions
    float localBoxWidth = desiredScaledWidth / zombieScaleFactor;
    float localBoxHeight = desiredScaledHeight / zombieScaleFactor;

    // Create a local hitbox centered around the sprite's origin
    sf::FloatRect localBoxDefinition(
        -localBoxWidth / 2.0f,
        -localBoxHeight / 2.0f,
        localBoxWidth,
        localBoxHeight
    );

    // Get the AABB of this transformed local box
    sf::FloatRect aabbOfTransformedLocalBox = sprite.getTransform().transformRect(localBoxDefinition);

    // Create an AABB hitbox centered at the sprite's position
    sf::Vector2f zombieCurrentPosition = sprite.getPosition();
    sf::FloatRect finalHitbox(
        zombieCurrentPosition.x - aabbOfTransformedLocalBox.width / 2.0f,
        zombieCurrentPosition.y - aabbOfTransformedLocalBox.height / 2.0f,
        aabbOfTransformedLocalBox.width,
        aabbOfTransformedLocalBox.height
    );

    return finalHitbox;
}

void BaseZombie::setState(ZombieState newState) {
    if (currentState != newState && !dead) {
        currentState = newState;

        // Reset animation for the new state
        if (currentState != ZombieState::ATTACK) {
            currentFrame = 0;
            animationTimer = 0.0f;
        }
    }
}

void BaseZombie::updateAnimation(float deltaTime) {
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
                m_hasDealtDamageInAttack = false;

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

bool BaseZombie::hasDealtDamageInAttack() const {
    return m_hasDealtDamageInAttack;
}

float BaseZombie::tryDealDamage() {
    if (!m_hasDealtDamageInAttack) {
        m_hasDealtDamageInAttack = true;
        return attackDamage;
    }
    return 0.0f;
}

void BaseZombie::loadTextureSet(std::vector<sf::Texture>& textures, const std::string& basePath, 
                                const std::string& prefix, int count, bool isBoss) {
    textures.clear();
    
    for (int i = 0; i < count; ++i) {
        sf::Texture texture;
        std::string filePath = basePath + "/" + prefix + "_00";
        
        if (i < 10) {
            filePath += std::to_string(i) + ".png";
        } else {
            filePath += std::to_string(i) + ".png";
        }
        
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie texture: " << filePath << std::endl;
        }
        textures.push_back(texture);
    }
}
