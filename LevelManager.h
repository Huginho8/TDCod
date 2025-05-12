#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Player.h"
#include "Zombie.h"
#include "Doctor.h"

enum class GameState {
    TUTORIAL,
    LEVEL1,
    LEVEL2,
    LEVEL3,
    GAME_OVER,
    VICTORY
};

class LevelManager {
public:
    LevelManager();
    
    void initialize();
    void update(float deltaTime, Player& player);
    void draw(sf::RenderWindow& window);
    
    // Added methods to match Game.cpp usage
    void render(sf::RenderWindow& window);
    void renderUI(sf::RenderWindow& window, sf::Font& font);
    void nextLevel();
    void reset();
    
    GameState getCurrentState() const;
    void setGameState(GameState state);
    
    // Tutorial specific methods
    void startTutorial();
    bool isTutorialComplete() const;
    
    // Level management
    void loadLevel(int levelNumber);
    int getCurrentLevel() const;
    
    // NPC management
    void spawnDoctor(float x, float y);
    void drawDoctor(sf::RenderWindow& window);
    bool isPlayerNearDoctor(const Player& player) const;
    
    // Zombie management
    void spawnZombies(int count, sf::Vector2f playerPos);
    void updateZombies(float deltaTime, const Player& player);
    void drawZombies(sf::RenderWindow& window) const;
    std::vector<Zombie>& getZombies();
    
    // HUD elements
    void drawHUD(sf::RenderWindow& window, const Player& player);
    
    // Tutorial dialog
    void showTutorialDialog(sf::RenderWindow& window);
    void advanceDialog();
    
private:
    GameState gameState;
    int currentLevel;
    bool tutorialComplete;
    
    // Doctor NPC
    Doctor doctor;
    bool doctorSpawned;
    
    // Tutorial specifics
    std::vector<std::string> tutorialDialogs;
    int currentDialogIndex;
    sf::Text dialogText;
    sf::Font font;
    sf::RectangleShape dialogBox;
    bool showingDialog;
    
    // Zombies
    std::vector<Zombie> zombies;
    
    // Map boundaries
    sf::RectangleShape mapBounds;
    const sf::Vector2f MAP_SIZE{1000.f, 1250.f};
};

#endif // LEVELMANAGER_H
