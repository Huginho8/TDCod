#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#include "LevelManager.h"
#include "PhysicsWorld.h"

class Game {
public:
    Game();
    void run();
    void addPoints(int amount);
    int getPoints() const;
    void reset();
    sf::RenderWindow& getWindow();

private:
    void processInput();
    void update(float deltaTime);
    void render();
    sf::Sprite& getMapSprite(int level);
    sf::Vector2u getMapSize(int level);
    void checkZombiePlayerCollisions();
    void checkPlayerBoundaries();
    bool isCollision(const sf::FloatRect& rect1, const sf::FloatRect& rect2);

    sf::RenderWindow window;
    Player player;
    int points;
    int zombiesToNextLevel;
    int pointsToNextLevel; // Added for points-based progression
    bool isPaused; // Added for pause menu
    bool showDebugInfo; // Added for debug info
    PhysicsWorld physics;
    LevelManager levelManager;

    sf::RectangleShape background;
    sf::Texture mapTexture1, mapTexture2, mapTexture3;
    sf::Sprite mapSprite1, mapSprite2, mapSprite3;
    sf::Font font;
    sf::Text pointsText;
    sf::Text debugText; // Added for debug info
    sf::Text pauseText; // Added for pause menu
    sf::View gameView;

    sf::Music backgroundMusic;
    sf::SoundBuffer zombieBiteBuffer;
    sf::Sound zombieBiteSound;
    sf::SoundBuffer knifeSwingBuffer; // Added for knife attack sound
    sf::Sound knifeSwingSound; // Added for knife attack sound
    sf::SoundBuffer batSwingBuffer; // Added for bat attack sound
    sf::Sound batSwingSound; // Added for bat attack sound
};

#endif