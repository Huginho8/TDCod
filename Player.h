#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <vector>

// Player states for animation and behavior control
enum class PlayerState {
    IDLE,
    WALK,
    // SPRINT, // Sprinting will just modify speed, not be a separate animation state for now
    ATTACK,
    DEATH
};

class Player {
public:
    Player();
    
    // Core gameplay methods
    void update(float deltaTime, sf::RenderWindow& window, sf::Vector2u mapSize, sf::Vector2f worldMousePosition);
    void draw(sf::RenderWindow& window);
    void render(sf::RenderWindow& window); // Alias for draw
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

    // Health getters
    float getCurrentHealth() const;
    float getMaxHealth() const;
    
    // Stamina getters
    float getCurrentStamina() const;
    float getMaxStamina() const;
    
    // Collision related methods
    sf::FloatRect getAttackBounds() const;
    sf::Sprite& getSprite();
    
    // Can be used to knock player back when hit by enemies
    void applyKnockback(const sf::Vector2f& direction, float force);
    
    // Get attack damage for enemy hit calculations
    float getAttackDamage() const;
    
    // Get player hitbox for collision detection
    sf::FloatRect getHitbox() const;
    sf::FloatRect getAttackHitbox() const; // Declaration for attack hitbox
    
    // Reset player to initial state
    void reset();

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