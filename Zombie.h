#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <SFML/Graphics.hpp>

class Zombie {
public:
    Zombie(float x, float y);
    void update(float deltaTime, sf::Vector2f playerPosition);
    void draw(sf::RenderWindow& window) const; // Make draw const
    sf::FloatRect getBounds() const; // Add bounds for collision detection

private:
    sf::Sprite sprite;
    std::vector<sf::Texture> walkingTextures; // Add walking textures
    int currentFrame;
    float animationTimer;
    float animationFrameTime;
    sf::Texture texture;
    float speed;
    sf::Vector2f position;
};

#endif