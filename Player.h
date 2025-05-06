#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>

class Player {
public:
    Player();
    void update(float deltaTime, sf::RenderWindow& window, sf::Vector2u mapSize); // Pass window and mapSize
    void draw(sf::RenderWindow& window);
    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;
    sf::Sprite& getSprite();
    void attack(); // Add attack function

private:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;
    //float speed = 100.0f;
    sf::Vector2f velocity;
    std::vector<sf::Texture> walkingTextures;
    std::vector<sf::Texture> attackTextures; // Add attack textures
    int currentFrame;
    float animationTimer;
    float animationFrameTime;
    bool isAttacking; // Add attacking state
    int currentAttackFrame; // Add current attack frame
    float attackTimer; // Add attack timer
    float attackFrameTime; // Add attack frame time
    float rotationSpeed; // Add rotation speed
    float scaleFactor; // Add scale factor
};

#endif