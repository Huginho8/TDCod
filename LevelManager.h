#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <optional>
#include "Player.h"
#include "Zombie.h"
#include "Doctor.h"
#include "ZombieWalker.h"
#include "ZombieCrawler.h"
#include "ZombieTank.h"
#include "ZombieZoom.h"
#include "ZombieKing.h"

enum class GameState {
    TUTORIAL,
    LEVEL1,
    LEVEL2,
    LEVEL3,
    LEVEL4,
    LEVEL5,
    GAME_OVER,
    VICTORY
};

class LevelManager {
public:
    LevelManager();
    
    void initialize();
    void update(float deltaTime, Player& player);
    void draw(sf::RenderWindow& window);
    
    void render(sf::RenderWindow& window);
    void renderUI(sf::RenderWindow& window, sf::Font& font);
    void nextLevel();
    void reset();
    
    int getPreviousLevel() const;
    GameState getCurrentState() const;
    void setGameState(GameState state);
    
    void startTutorial();
    bool isTutorialComplete() const;
    
    void loadLevel(int levelNumber);
    int getCurrentLevel() const;
    int getCurrentRound() const;
    
    void spawnDoctor(float x, float y);
    void drawDoctor(sf::RenderWindow& window);
    bool isPlayerNearDoctor(const Player& player) const;
    
    void spawnZombies(int count, sf::Vector2f playerPos);
    void updateZombies(float deltaTime, const Player& player);
    void drawZombies(sf::RenderWindow& window) const;
    std::vector<Zombie>& getZombies();
    
    void drawHUD(sf::RenderWindow& window, const Player& player);
    
    void showTutorialDialog(sf::RenderWindow& window);
    void advanceDialog();
    
private:
    GameState gameState;
    int currentLevel;
    int previousLevel;
    int currentRound;
    bool tutorialComplete;
    
    std::optional<Doctor> doctor;
    bool tutorialZombiesSpawned;
    std::vector<std::string> tutorialDialogs;
    int currentDialogIndex;
    sf::Text dialogText;
    sf::Font font;
    sf::RectangleShape dialogBox;
    bool showingDialog;
    
    std::vector<Zombie> zombies;
    
    sf::RectangleShape mapBounds;
    const sf::Vector2f MAP_SIZE{1000.f, 1250.f};
    
    // Round transition
    float roundTransitionTimer;
    bool inRoundTransition;
    
    int getZombieCountForLevel(int level);
};

#endif // LEVELMANAGER_H
