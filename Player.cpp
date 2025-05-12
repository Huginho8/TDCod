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
        // Adjusting Y origin: if circle is "in front", origin Y might be too large.
        // Assuming "front" is along the sprite's visual top if it faces down.
        // Let's try making the Y origin much smaller (higher on the texture).
        sprite.setOrigin(textureSize.x / 2.0f, textureSize.y * 0.25f);
        sprite.setScale(scaleFactor, scaleFactor);
        sprite.setPosition(100, 100);
    }

    // Load sounds
    if (!knifeAttackBuffer.loadFromFile("TDCod/Assets/Audio/knife.mp3")) {
        std::cerr << "Error loading knife attack sound!" << std::endl;
    } else {
        knifeAttackSound.setBuffer(knifeAttackBuffer);
        knifeAttackSound.setVolume(50); // Lower knife attack sound volume
    }

    if (!walkingBuffer.loadFromFile("TDCod/Assets/Audio/walking.mp3")) {
        std::cerr << "Error loading walking sound!" << std::endl;
    } else {
        walkingSound.setBuffer(walkingBuffer);
        walkingSound.setLoop(true); // Loop the walking sound
        walkingSound.setVolume(90); // Increase walking sound volume again
    }
}

void Player::loadTextures() {
    // Load idle animation textures
    for (int i = 0; i < 8; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/Hero/HeroKnife/HeroIdle/Idle_Knife_00" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading player idle texture: " << filePath << std::endl;
        }
        idleTextures.push_back(texture);
    }

    // Load walking animation textures
    for (int i = 0; i < 6; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/Hero/HeroKnife/HeroWalk/Walk_knife_00" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading player walking texture: " << filePath << std::endl;
        }
        walkingTextures.push_back(texture);
    }

    // Load sprint animation textures (reusing move but will be animated faster)
    for (int i = 0; i < 6; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/Hero/HeroKnife/HeroWalk/Walk_knife_00" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading player sprint texture: " << filePath << std::endl;
        }
        sprintTextures.push_back(texture);
    }
    
    // Load attack animation textures
    for (int i = 0; i < 8; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/Hero/HeroKnife/HeroAttack/Knife_00" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading player attack texture: " << filePath << std::endl;
        }
        attackTextures.push_back(texture);
    }
    
    // Load death animation textures
    for (int i = 0; i < 6; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/Hero/HeroDeath/death_000" + std::to_string(i) + "_Man.png";
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
        timeSinceLastDamage = 0.0f; // Reset timer on taking damage
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
    // Sprinting is now determined by Shift key and stamina, not a state.
    // This method might still be useful for external checks or if we reintroduce sprint state.
    // For now, let's say you are "trying to sprint" if Shift is pressed.
    // Actual speed modification will happen in update().
    bool wantsToSprint = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
    return wantsToSprint && stamina > 0;
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
    
    // Animation speed is faster when sprinting (Shift pressed and moving)
    // We'll use a flag or direct check in update() for speed, animation will just be WALK or IDLE.
    // No separate SPRINT animation for now.
    float frameTime = animationFrameTime; // Default frame time
    
    // If we wanted faster walk animation when "sprinting":
    // bool isMovingFast = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) && (velocity.x != 0 || velocity.y != 0) && stamina > 0;
    // if (isMovingFast) frameTime *= 0.7f;


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
                // Attack finished, determine next state based on movement
                if (velocity.x != 0.0f || velocity.y != 0.0f) {
                    setState(PlayerState::WALK);
                } else {
                    setState(PlayerState::IDLE);
                }
                // The main animation loop will pick up the correct frame for the new state
                // because setState would have reset currentFrame if the state changed.
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
                if (!walkingTextures.empty()) {
                    currentFrame = (currentFrame + 1) % walkingTextures.size();
                    sprite.setTexture(walkingTextures[currentFrame]);
                } else {
                }
                break;
            // SPRINT case removed
            default:
                break;
        }
    }
}

void Player::updateStamina(float deltaTime) {
    // All stamina logic (drain and regeneration) has been moved into Player::update()
    // This function is currently not strictly needed but kept to avoid breaking calls if any.
    // To re-enable separate regen logic here, uncomment and adjust Player::update().

    // Example of standalone regen logic (if Player::update didn't handle it):
    /*
    if (! (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) || stamina <= 0 ) { // Not trying to sprint or out of stamina
        timeSinceStaminaUse += deltaTime;
        if (timeSinceStaminaUse >= staminaRegenDelay) {
            isStaminaRegenerating = true;
        }
        if (isStaminaRegenerating) {
            stamina = std::min(maxStamina, stamina + staminaRegenRate * deltaTime);
        }
    }
    */
}

void Player::update(float deltaTime, sf::RenderWindow& window, sf::Vector2u mapSize, sf::Vector2f worldMousePosition) {
    if (dead) {
        // Only update death animation if player is dead
        updateAnimation(deltaTime);
        // Stop walking sound if player dies
        if (walkingSound.getStatus() == sf::Sound::Playing) {
            walkingSound.stop();
        }
        return;
    }
    
    // Update time since last damage for health regeneration
    timeSinceLastDamage += deltaTime;

    // Health regeneration logic
    if (timeSinceLastDamage >= HEALTH_REGEN_DELAY && health < maxHealth) {
        health = std::min(maxHealth, health + healthRegenRate * deltaTime);
    }

    // Set default state to idle, will be changed if necessary
    setState(PlayerState::IDLE);
    
    // Reset velocity
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    
    // Check sprint input
    bool wantsToSprint = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
    bool actuallySprinting = false;
    
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
    
    // Set state based on movement (only change if not attacking to maintain attack animation)
    if (!attacking) {
        if (velocity.x != 0 || velocity.y != 0) {
            setState(PlayerState::WALK); // Always WALK state if moving
        } else {
             setState(PlayerState::IDLE); // Ensure idle if not moving
        }
    }

    // Apply speed based on sprinting and stamina
    if (velocity.x != 0 || velocity.y != 0) {
        if (wantsToSprint && stamina > 0) {
            velocity.x *= sprintSpeed;
            velocity.y *= sprintSpeed;
            actuallySprinting = true; // For stamina drain
            // Adjust walking sound pitch for running
            walkingSound.setPitch(1.5f); // Speed up the sound
        } else {
            velocity.x *= speed;
            velocity.y *= speed;
            // Reset walking sound pitch for walking
            walkingSound.setPitch(1.0f);
        }
        // Play walking sound if not already playing and not attacking
        if (walkingSound.getStatus() != sf::Sound::Playing && !attacking) {
            walkingSound.play();
        }
    } else {
         // Stop walking sound if not moving
         if (walkingSound.getStatus() == sf::Sound::Playing) {
             walkingSound.stop();
         }
    }
    
    // Update Stamina Drain / Regen Call
    if (actuallySprinting) {
        stamina = std::max(0.0f, stamina - staminaDrainRate * deltaTime);
        isStaminaRegenerating = false;
        timeSinceStaminaUse = 0.0f;
    } else {
        // This part of stamina logic was originally in updateStamina()
        timeSinceStaminaUse += deltaTime;
        if (timeSinceStaminaUse >= staminaRegenDelay) {
            isStaminaRegenerating = true;
        }
        if (isStaminaRegenerating) {
            stamina = std::min(maxStamina, stamina + staminaRegenRate * deltaTime);
        }
    }
    // The call to updateStamina() below will now primarily handle regeneration
    // or can be removed if all logic is here. For now, let's keep it simple.
    // updateStamina(deltaTime); // Original call - review if needed after merging logic

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
    // Rotation to face mouse cursor using the provided world mouse position
    sf::Vector2f playerPosition = sprite.getPosition();
    sf::Vector2f direction = worldMousePosition - playerPosition;
    float distanceToMouse = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    float minDistanceForRotation = 1.0f; // Minimum distance to update rotation

    float angleToMouseDegrees; // Angle towards the mouse, or current facing if mouse is too close

    if (distanceToMouse > minDistanceForRotation) {
        angleToMouseDegrees = std::atan2(direction.y, direction.x) * 180 / 3.14159265f;
        sprite.setRotation(angleToMouseDegrees - 90.0f);
    } else {
        // Mouse is too close, use current sprite rotation to derive the facing angle
        // Add 90 because sprite.setRotation had 90 subtracted.
        angleToMouseDegrees = sprite.getRotation() + 90.0f;
    }
    // 'angleToMouseDegrees' is now the effective angle the player is "aiming" or was last aiming.
    
    // Update attack state
    if (attacking) {
        // updateAnimation(deltaTime); // This call is redundant, will be called once at the end
        
        // Calculate attack bounds (in front of the player) using angleToMouseDegrees
        sf::Vector2f attackOffset = sf::Vector2f(
            std::cos(angleToMouseDegrees * 3.14159265f / 180) * 30.0f,
            std::sin(angleToMouseDegrees * 3.14159265f / 180) * 30.0f
        );
        
        // Keep the attack box size as 60x60 for now, centered on the new offset
        float attackBoxSize = 60.0f;
        attackBounds = sf::FloatRect(
            playerPosition.x + attackOffset.x - (attackBoxSize / 2.0f),
            playerPosition.y + attackOffset.y - (attackBoxSize / 2.0f),
            attackBoxSize, attackBoxSize
        );
    }
    
    // Stamina logic is now fully within Player::update's main body.
    // No separate call to updateStamina() needed here anymore.
    
    // Update animation
    updateAnimation(deltaTime);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
    
    // Debug: Draw a circle at sprite's reported position (Now removed)
    // sf::CircleShape posCircle(5.f);
    // posCircle.setPosition(sprite.getPosition());
    // posCircle.setOrigin(5.f, 5.f);
    // posCircle.setFillColor(sf::Color::Yellow);
    // window.draw(posCircle);

    // Debug: Draw main hitbox (AABB of transformed local box)
    sf::RectangleShape mainHitboxShape;
    sf::FloatRect mainHitbox = getHitbox(); // This is an AABB
    // To center the AABB shape on the sprite's origin (yellow circle)
    // Correctly position the debug shape using the global coordinates from getHitbox()
    mainHitboxShape.setPosition(mainHitbox.left, mainHitbox.top);
    mainHitboxShape.setSize(sf::Vector2f(mainHitbox.width, mainHitbox.height));
    mainHitboxShape.setFillColor(sf::Color(0, 255, 0, 70)); // Green, semi-transparent
    mainHitboxShape.setOutlineColor(sf::Color::Green);
    mainHitboxShape.setOutlineThickness(1);
    window.draw(mainHitboxShape);

    // Debug: Draw attack bounds when attacking
    if (attacking) {
        sf::RectangleShape attackRect;
        attackRect.setPosition(attackBounds.left, attackBounds.top);
        attackRect.setSize(sf::Vector2f(attackBounds.width, attackBounds.height));
        attackRect.setFillColor(sf::Color(255, 0, 0, 70)); // Red, semi-transparent
        attackRect.setOutlineColor(sf::Color::Red);
        attackRect.setOutlineThickness(1);
        window.draw(attackRect);
    }
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

        // Play knife attack sound
        // Stop the sound before playing to prevent delay on rapid swings
        knifeAttackSound.stop();
        knifeAttackSound.play();
    }
}

// Alias for draw to match Game.cpp usage
void Player::render(sf::RenderWindow& window) {
    draw(window);
}

// Return player hitbox for collision detection
sf::FloatRect Player::getHitbox() const {
    // Define desired hitbox size in *scaled* pixels (these dimensions define a local box)
    const float desiredScaledWidth = 60.0f;
    const float desiredScaledHeight = 60.0f;

    // Calculate the equivalent local (unscaled) dimensions for the local box
    float localBoxWidth = desiredScaledWidth / scaleFactor;
    float localBoxHeight = desiredScaledHeight / scaleFactor;

    // Create a local hitbox centered around the sprite's origin (0,0 in local origin-adjusted space)
    sf::FloatRect localBoxDefinition(
        -localBoxWidth / 2.0f,
        -localBoxHeight / 2.0f,
        localBoxWidth,
        localBoxHeight
    );
    
    // Get the AABB of this transformed local box. Its dimensions will be used.
    sf::FloatRect aabbOfTransformedLocalBox = sprite.getTransform().transformRect(localBoxDefinition);

    // The logical hitbox will be centered at the sprite's current position,
    // using the width and height of the AABB of the transformed local box.
    // This creates an AABB hitbox that doesn't rotate with the sprite visually.
    sf::Vector2f playerCurrentPosition = sprite.getPosition();
    sf::FloatRect finalHitbox(
        playerCurrentPosition.x - aabbOfTransformedLocalBox.width / 2.0f,
        playerCurrentPosition.y - aabbOfTransformedLocalBox.height / 2.0f,
        aabbOfTransformedLocalBox.width,
        aabbOfTransformedLocalBox.height
    );
    
    // Optional: Log the final hitbox details
    return finalHitbox;
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
float Player::getCurrentHealth() const {
    return health;
}

float Player::getMaxHealth() const {
    return maxHealth;
}