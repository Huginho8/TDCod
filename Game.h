#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // Include for audio
#include "Player.h"
#include "LevelManager.h"

class Game {
public:
    Game();
    void run();
    sf::RenderWindow& getWindow(); // Added getter for the window
    
    // Added declarations for methods used in Game.cpp
    bool isCollision(const sf::FloatRect& rect1, const sf::FloatRect& rect2);
    void addPoints(int amount);
    int getPoints() const;
    void reset();
    
private:
    sf::RenderWindow window;
    Player player;
    LevelManager levelManager;
    
    sf::Sprite mapSprite;
    sf::Texture mapTexture;
    sf::RectangleShape background;
    
    int points;
    int zombiesToNextLevel;
    sf::Text pointsText;
    sf::Font font;
    sf::Clock clock;
    sf::View gameView;
    
    void processInput();
    void update(float deltaTime);
    void render();
    
    // Collision detection
    void checkZombiePlayerCollisions();
    void checkPlayerBoundaries();
    
    // HUD elements
    void drawHUD();

   // Sound effects
   sf::Music cutsceneMusic;
   sf::Music backgroundMusic;
   sf::Music waveStartSound;
   sf::SoundBuffer zombieBiteBuffer;
   sf::Sound zombieBiteSound;
};

#endif // GAME_H