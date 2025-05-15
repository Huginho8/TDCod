#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#include "LevelManager.h"

class Game {
public:
    Game();
    void run();
    sf::RenderWindow& getWindow();
    
    bool isCollision(const sf::FloatRect& rect1, const sf::FloatRect& rect2);
    void addPoints(int amount);
    int getPoints() const;
    void reset();
    
    // Added to select map sprite based on level
    sf::Sprite& getMapSprite(int level);

private:
    sf::RenderWindow window;
    Player player;
    LevelManager levelManager;
    
    sf::Sprite mapSprite1; // Map for Tutorial, Level 1, Level 4, Level 5
    sf::Sprite mapSprite2; // Map for Level 2
    sf::Sprite mapSprite3; // Map for Level 3
    sf::Texture mapTexture1;
    sf::Texture mapTexture2;
    sf::Texture mapTexture3;
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
    
    void checkZombiePlayerCollisions();
    void checkPlayerBoundaries();
    
    void drawHUD();

    sf::Music cutsceneMusic;
    sf::Music backgroundMusic;
    sf::Music waveStartSound;
    sf::SoundBuffer zombieBiteBuffer;
    sf::Sound zombieBiteSound;
};

#endif // GAME_H