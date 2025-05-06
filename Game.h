#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Zombie.h" // Include Zombie.h
#include <vector>

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    Player player;
    sf::Sprite mapSprite; // Add map sprite
    sf::Texture mapTexture; // Add map texture
    sf::RectangleShape background;
    int points;
    int zombiesToNextLevel;
    sf::Text pointsText;
    sf::Clock clock;
    std::vector<Zombie> zombies; // Add a vector to hold zombies
    sf::View cameraView;

    void processInput();
    void update(float deltaTime);
    void render();
    void spawnZombies(int count); // Declare spawnZombies
};

#endif