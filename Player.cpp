#include "Player.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Player::Player() : 
    currentState(PlayerState::IDLE), 
    speed(100.0f), 
    sprintSpeed(180.0f), 
    velocity(0.0f, 0.0f), 
    health(100.0f), 
    maxHealth(100.0f), 
    stamina(100.0f), 
    maxStamina(100.0f), 
    staminaRegenRate(20.0f), 
    staminaDrainRate(30.0f), 
    staminaRegenDelay(1.5f), 
    timeSinceStaminaUse(0.0f), 
    isStaminaRegenerating(true), 
    currentFrame(0), 
    animationTimer(0.0f), 
    animationFrameTime(0.1f), 
    attacking(false), 
    currentAttackFrame(0), 
    attackTimer(0.0f), 
    attackFrameTime(0.05f), 
    dead(false), 
    currentDeathFrame(0), 
    deathTimer(0.0f), 
    deathFrameTime(0.1f), 
    rotationSpeed(180.0f), 
    scaleFactor(0.35f),
    attackDamage(10.0f) // Added default attack damage
{
    loadTextures();
    // Set initial texture to idle
    if (!idleTextures.empty()) {
        sprite.setTexture(idleTextures[0]);
        sf::Vector2u textureSize = idleTextures[0].getSize();
        sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        sprite.setScale(scaleFactor, scaleFactor);
        sprite.setPosition(100, 100);
    }
}

void Player::loadTextures() {
    // Load idle animation textures
    for (int i = 0; i < 8; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/Hero/HeroKnife/HeroIdle/Idle_Knife_0" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading player idle texture: " << filePath << std::endl;
        }
        idleTextures.push_back(texture);
    }

    // Load walking animation textures
    for (int i = 0; i < 6; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/Hero/HeroKnife/HeroWalk/Walk_knife_0" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading player walking texture: " << filePath << std::endl;
        }
        walkingTextures.push_back(texture);
    }

    // Load sprint animation textures (reusing move but will be animated faster)
    for (int i = 0; i < 6; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/Hero/HeroKnife/HeroWalk/Walk_knife_0" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading player sprint texture: " << filePath << std::endl;
        }
        sprintTextures.push_back(texture);
    }
    
    // Load attack animation textures
    for (int i = 0; i < 8; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/Hero/HeroKnife/HeroAttack/Knife_0" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading player attack texture: " << filePath << std::endl;
        }
        attackTextures.push_back(texture);
    }
    
    // Load death animation textures
    for (int i = 0; i < 6; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/Hero/HeroDeath/death_00_Man" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading player death texture: " << filePath << std::endl;
        }
        deathTextures.push_back(texture);
    }
}

void Player::sprint(bool isSprinting) {
    if (isSprinting && stamina > 0) {
        isStaminaRegenerating = false;
        timeSinceStaminaUse = 0.0f;
    }
}

void Player::takeDamage(float amount) {
    if (!dead) {
        health -= amount;
        if (health <= 0) {
            health = 0;
            kill();
        }
    }
}

void Player::kill() {
    if (!dead) {
        dead = true;
        attacking = false;
        currentDeathFrame = 0;
        deathTimer = 0.0f;
        setState(PlayerState::DEATH);
        
        if (!deathTextures.empty()) {
            sprite.setTexture(deathTextures[currentDeathFrame]);
        }
    }
}

bool Player::isAttacking() const {
    return attacking;
}

bool Player::isDead() const {
    return dead;
}

bool Player::isSprinting() const {
    return currentState == PlayerState::SPRINT && stamina > 0;
}

float Player::getCurrentStamina() const {
    return stamina;
}

float Player::getMaxStamina() const {
    return maxStamina;
}

sf::FloatRect Player::getAttackBounds() const {
    return attackBounds;
}

void Player::setState(PlayerState newState) {
    // Only change state if it's different and not dead
    if (currentState != newState && !dead) {
        currentState = newState;
        
        // Reset animation for the new state
        if (currentState != PlayerState::ATTACK) {
            currentFrame = 0;
            animationTimer = 0.0f;
        }
    }
}

void Player::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;
    
    // Animation speed is faster when sprinting
    float frameTime = (currentState == PlayerState::SPRINT) ? animationFrameTime * 0.7f : animationFrameTime;
    
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
                
                // Revert to previous state
                if (velocity.x != 0 || velocity.y != 0) {
                    if (currentState == PlayerState::SPRINT) {
                        if (!sprintTextures.empty()) {
                            sprite.setTexture(sprintTextures[currentFrame]);
                        }
                    } else {
                        if (!walkingTextures.empty()) {
                            sprite.setTexture(walkingTextures[currentFrame]);
                        }
                    }
                } else {
                    if (!idleTextures.empty()) {
                        sprite.setTexture(idleTextures[currentFrame]);
                    }
                }
            }
        }
    } else if (animationTimer >= frameTime) {
        // Regular animation cycle
        animationTimer -= frameTime;
        
        switch (currentState) {
            case PlayerState::IDLE:
                currentFrame = (currentFrame + 1) % idleTextures.size();
                if (!idleTextures.empty()) {
                    sprite.setTexture(idleTextures[currentFrame]);
                }
                break;
                
            case PlayerState::WALK:
                currentFrame = (currentFrame + 1) % walkingTextures.size();
                if (!walkingTextures.empty()) {
                    sprite.setTexture(walkingTextures[currentFrame]);
                }
                break;
                
            case PlayerState::SPRINT:
                currentFrame = (currentFrame + 1) % sprintTextures.size();
                if (!sprintTextures.empty()) {
                    sprite.setTexture(sprintTextures[currentFrame]);
                }
                break;
                
            default:
                break;
        }
    }
}

void Player::updateStamina(float deltaTime) {
    // Drain stamina when sprinting
    if (currentState == PlayerState::SPRINT) {
        stamina = std::max(0.0f, stamina - staminaDrainRate * deltaTime);
        isStaminaRegenerating = false;
        timeSinceStaminaUse = 0.0f;
    } else {
        // Update timer since last stamina use
        timeSinceStaminaUse += deltaTime;
        
        // Begin regenerating after delay
        if (timeSinceStaminaUse >= staminaRegenDelay) {
            isStaminaRegenerating = true;
        }
        
        // Regenerate stamina
        if (isStaminaRegenerating) {
            stamina = std::min(maxStamina, stamina + staminaRegenRate * deltaTime);
        }
    }
}

void Player::update(float deltaTime, sf::RenderWindow& window, sf::Vector2u mapSize) {
    if (dead) {
        // Only update death animation if player is dead
        updateAnimation(deltaTime);
        return;
    }
    
    // Set default state to idle, will be changed if necessary
    setState(PlayerState::IDLE);
    
    // Reset velocity
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    
    // Check sprint input
    bool wantToSprint = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
    
    // Only allow sprinting if we have stamina
    if (wantToSprint && stamina > 0) {
        sprint(true);
    } else {
        sprint(false);
    }
    
    // Handle movement only if not attacking
    if (!attacking) {
        // Set movement velocity based on keyboard input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            velocity.x = -1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            velocity.x = 1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            velocity.y = -1.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            velocity.y = 1.0f;
        }
        
        // Normalize diagonal movement
        if (velocity.x != 0 && velocity.y != 0) {
            float length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
            velocity.x /= length;
            velocity.y /= length;
        }
        
        // Set state based on movement
        if (velocity.x != 0 || velocity.y != 0) {
            if (isSprinting()) {
                setState(PlayerState::SPRINT);
                velocity.x *= sprintSpeed;
                velocity.y *= sprintSpeed;
            } else {
                setState(PlayerState::WALK);
                velocity.x *= speed;
                velocity.y *= speed;
            }
        }
    }
    
    // Update position
    sf::Vector2f newPosition = sprite.getPosition() + velocity * deltaTime;
    
    // Apply map boundaries (prevent player from leaving the map)
    sf::FloatRect playerBounds = sprite.getGlobalBounds();
    float playerWidth = playerBounds.width;
    float playerHeight = playerBounds.height;
    
    // Clamp to map boundaries (1000x1250 pixels)
    newPosition.x = std::max(playerWidth / 2.0f, std::min(newPosition.x, 1000.0f - playerWidth / 2.0f));
    newPosition.y = std::max(playerHeight / 2.0f, std::min(newPosition.y, 1250.0f - playerHeight / 2.0f));
    
    sprite.setPosition(newPosition);
    
    // Rotation to face mouse cursor
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    sf::Vector2f worldMousePosition = window.mapPixelToCoords(mousePosition);
    sf::Vector2f playerPosition = sprite.getPosition();
    sf::Vector2f direction = worldMousePosition - playerPosition;
    
    float angle = std::atan2(direction.y, direction.x) * 180 / 3.14159265f;
    sprite.setRotation(angle);
    
    // Update attack state
    if (attacking) {
        updateAnimation(deltaTime);
        
        // Calculate attack bounds (in front of the player)
        sf::Vector2f attackOffset = sf::Vector2f(
            std::cos(angle * 3.14159265f / 180) * 50,
            std::sin(angle * 3.14159265f / 180) * 50
        );
        
        attackBounds = sf::FloatRect(
            playerPosition.x + attackOffset.x - 30,
            playerPosition.y + attackOffset.y - 30,
            60, 60
        );
    }
    
    // Update stamina
    updateStamina(deltaTime);
    
    // Update animation
    updateAnimation(deltaTime);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
    
    // Debug: Draw attack bounds when attacking
    // if (attacking) {
    //     sf::RectangleShape attackRect;
    //     attackRect.setPosition(attackBounds.left, attackBounds.top);
    //     attackRect.setSize(sf::Vector2f(attackBounds.width, attackBounds.height));
    //     attackRect.setFillColor(sf::Color(255, 0, 0, 100));
    //     window.draw(attackRect);
    // }
}

void Player::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

sf::Vector2f Player::getPosition() const {
    return sprite.getPosition();
}

sf::Sprite& Player::getSprite() {
    return sprite;
}

void Player::attack() {
    if (!attacking && !dead) {
        attacking = true;
        currentAttackFrame = 0;
        attackTimer = 0.0f;
        setState(PlayerState::ATTACK);
        
        if (!attackTextures.empty()) {
            sprite.setTexture(attackTextures[currentAttackFrame]);
        }
    }
}

// Alias for draw to match Game.cpp usage
void Player::render(sf::RenderWindow& window) {
    draw(window);
}

// Return player hitbox for collision detection
sf::FloatRect Player::getHitbox() const {
    // Get the sprite's bounds and make the hitbox a bit smaller for better gameplay
    sf::FloatRect bounds = sprite.getGlobalBounds();
    float hitboxReduction = 10.0f; // Reduce hitbox size for better gameplay feel
    
    return sf::FloatRect(
        bounds.left + hitboxReduction,
        bounds.top + hitboxReduction,
        bounds.width - (2 * hitboxReduction),
        bounds.height - (2 * hitboxReduction)
    );
}

// Return attack hitbox
sf::FloatRect Player::getAttackHitbox() const {
    // The attack hitbox is already calculated in the update function
    // and stored in attackBounds
    return attackBounds;
}

// Apply knockback from zombie attacks
void Player::applyKnockback(const sf::Vector2f& direction, float force) {
    sf::Vector2f knockbackVelocity = direction * force;
    sf::Vector2f newPosition = sprite.getPosition() + knockbackVelocity * 0.016f; // Assume 60 FPS
    sprite.setPosition(newPosition);
}

float Player::getAttackDamage() const {
    return attackDamage;
}

void Player::reset() {
    // Reset player state
    health = maxHealth;
    stamina = maxStamina;
    dead = false;
    attacking = false;
    currentState = PlayerState::IDLE;
    sprite.setPosition(100, 100);
}