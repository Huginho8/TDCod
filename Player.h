#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <vector>

// Player states for animation and behavior control
enum class PlayerState {
    IDLE,
    WALK,
    SPRINT, // Activated by holding Shift
    ATTACK,
    DEATH
};

class Player {
public:
    Player();
    
    // Core gameplay methods
    void update(float deltaTime, sf::RenderWindow& window, sf::Vector2u mapSize);
    void draw(sf::RenderWindow& window);
    void render(sf::RenderWindow& window) { draw(window); } // Alias for draw
    void attack();
    void takeDamage(float amount);
    void kill();
    
    // Movement and positioning
    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;
    
    // Sprinting functionality - requires Shift key to be pressed
    void sprint(bool isSprinting);
    bool isSprinting() const;
    
    // State queries
    bool isAttacking() const;
    bool isDead() const;
    
    // Stamina getters
    float getCurrentStamina() const;
    float getMaxStamina() const;
    
    // Collision related methods
    sf::FloatRect getAttackBounds() const;
    sf::Sprite& getSprite();
    
    // Can be used to knock player back when hit by enemies
    void applyKnockback(const sf::Vector2f& direction, float force) {
        // Temporary implementation for knockback
        sf::Vector2f newPosition = sprite.getPosition() + direction * force;
        sprite.setPosition(newPosition);
    }
    
    // Get attack damage for enemy hit calculations
    float getAttackDamage() const { return attackDamage; }
    
    // Get player hitbox for collision detection
    sf::FloatRect getHitbox() const {
        return sprite.getGlobalBounds();
    }
    
    // Reset player to initial state
    void reset() {
        health = maxHealth;
        stamina = maxStamina;
        dead = false;
        attacking = false;
        currentState = PlayerState::IDLE;
        setPosition(100, 100);
        
        if (!idleTextures.empty()) {
            sprite.setTexture(idleTextures[0]);
        }
    }

private:
    // Animation handling
    void loadTextures();
    void updateAnimation(float deltaTime);
    void setState(PlayerState newState);
    void updateStamina(float deltaTime);
    
    // Core attributes
    sf::Sprite sprite;
    sf::Vector2f velocity;
    PlayerState currentState;
    
    // Movement properties
    float speed;
    float sprintSpeed;  // Applied when Shift is held and stamina > 0
    float rotationSpeed;
    float scaleFactor;
    
    // Health properties
    float health;
    float maxHealth;
    
    // Stamina system - used for sprinting (Shift key)
    float stamina;
    float maxStamina;
    float staminaRegenRate;
    float staminaDrainRate;
    float staminaRegenDelay;
    float timeSinceStaminaUse;
    bool isStaminaRegenerating;
    
    // Animation properties
    int currentFrame;
    float animationTimer;
    float animationFrameTime;
    
    // Attack properties
    bool attacking;
    int currentAttackFrame;
    float attackTimer;
    float attackFrameTime;
    sf::FloatRect attackBounds;
    float attackDamage = 20.0f;  // Damage dealt to enemies
    
    // Death properties
    bool dead;
    int currentDeathFrame;
    float deathTimer;
    float deathFrameTime;
    
    // Texture collections for animations
    std::vector<sf::Texture> idleTextures;
    std::vector<sf::Texture> walkingTextures;
    std::vector<sf::Texture> sprintTextures;  // Used when holding Shift
    std::vector<sf::Texture> attackTextures;
    std::vector<sf::Texture> deathTextures;
};

#endif // PLAYER_H