#include "LevelManager.h"
#include <iostream>
#include <random>
#include <cmath>

LevelManager::LevelManager()
    : gameState(GameState::TUTORIAL),
      currentLevel(0),
      previousLevel(0),
      currentRound(0),
      tutorialComplete(false),
      tutorialZombiesSpawned(false),
      currentDialogIndex(0),
      showingDialog(false),
      roundTransitionTimer(0.0f),
      inRoundTransition(false) {
    
    // Initialize tutorial dialogs
    tutorialDialogs = {
        "Welcome to Echoes of Valkyrie! I'm Dr. Mendel, the lead researcher.",
        "The infection is spreading rapidly. We need your help!",
        "Use WASD keys to move around. Press SHIFT to sprint, but watch your stamina.",
        "Left click to attack zombies with your knife.",
        "Defeat zombies to earn points and progress to the next level.",
        "Good luck! The fate of humanity depends on you.",
        "Complete this tutorial by exploring the area and eliminating all zombies."
    };
    
    // Load font
    if (!font.loadFromFile("TDCod/Assets/Call of Ops Duty.otf")) {
        std::cerr << "Error loading font for dialog! Trying fallback." << std::endl;
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading fallback font 'arial.ttf' as well!" << std::endl;
        }
    }
    
    // Setup dialog box
    dialogBox.setSize(sf::Vector2f(600, 150));
    dialogBox.setFillColor(sf::Color(0, 0, 0, 200));
    dialogBox.setOutlineColor(sf::Color::White);
    dialogBox.setOutlineThickness(2);
    
    // Setup dialog text
    dialogText.setFont(font);
    dialogText.setCharacterSize(18);
    dialogText.setFillColor(sf::Color::White);
    
    // Setup map boundaries
    mapBounds.setSize(MAP_SIZE);
    mapBounds.setFillColor(sf::Color::Transparent);
    mapBounds.setOutlineColor(sf::Color::Red);
    mapBounds.setOutlineThickness(2);
    mapBounds.setPosition(0, 0);
}

void LevelManager::initialize() {
    setGameState(GameState::TUTORIAL);
    startTutorial();
}

void LevelManager::update(float deltaTime, Player& player) {
    previousLevel = currentLevel;
    
    // Handle round transition delay
    if (inRoundTransition) {
        roundTransitionTimer += deltaTime;
        if (roundTransitionTimer >= 2.0f) { // 2-second pause between rounds
            inRoundTransition = false;
            roundTransitionTimer = 0.0f;
            spawnZombies(getZombieCountForLevel(currentLevel), player.getPosition());
        }
        return;
    }
    
    switch (gameState) {
        case GameState::TUTORIAL:
            if (isPlayerNearDoctor(player) && !showingDialog) {
                showingDialog = true;
            }
            
            if (tutorialZombiesSpawned) {
                updateZombies(deltaTime, player);
            }
            
            if (currentDialogIndex >= tutorialDialogs.size() && !tutorialZombiesSpawned) {
                if (doctor) {
                    spawnZombies(3, doctor->getPosition());
                }
                tutorialZombiesSpawned = true;
            }

            if (zombies.empty() && tutorialZombiesSpawned && currentDialogIndex >= tutorialDialogs.size()) {
                tutorialComplete = true;
                loadLevel(1);
            }
            break;
            
        case GameState::LEVEL1:
        case GameState::LEVEL2:
        case GameState::LEVEL3:
        case GameState::LEVEL4:
        case GameState::LEVEL5:
            updateZombies(deltaTime, player);
            
            if (zombies.empty() && !inRoundTransition) {
                if (gameState == GameState::LEVEL5) {
                    setGameState(GameState::VICTORY);
                } else if (currentRound < 1) {
                    currentRound++;
                    inRoundTransition = true; // Start transition delay
                } else {
                    int nextLevel = currentLevel + 1;
                    if (nextLevel > 5) {
                        setGameState(GameState::VICTORY);
                    } else {
                        loadLevel(nextLevel);
                    }
                }
            }
            break;
            
        case GameState::GAME_OVER:
        case GameState::VICTORY:
            break;
    }
    
    if (doctor) {
        doctor->update(deltaTime, player.getPosition());
    }
}

void LevelManager::draw(sf::RenderWindow& window) {
    drawZombies(window);
    
    if (doctor) {
        doctor->draw(window);
    }
}

void LevelManager::render(sf::RenderWindow& window) {
    draw(window);
}

void LevelManager::renderUI(sf::RenderWindow& window, sf::Font& font) {
    if (showingDialog && currentDialogIndex < tutorialDialogs.size()) {
        dialogText.setFont(this->font);
        showTutorialDialog(window);
    }
    
    // Display round transition message
    if (inRoundTransition && gameState != GameState::TUTORIAL) {
        sf::Text transitionText;
        transitionText.setFont(this->font);
        transitionText.setCharacterSize(24);
        transitionText.setFillColor(sf::Color::Yellow);
        transitionText.setString("Round " + std::to_string(currentRound + 1) + " Starting Soon...");
        sf::Vector2u windowSize = window.getSize();
        transitionText.setPosition((windowSize.x - transitionText.getGlobalBounds().width) / 2, windowSize.y / 2);
        window.draw(transitionText);
    }
}

void LevelManager::nextLevel() {
    int nextLevelNumber = currentLevel + 1;
    if (nextLevelNumber > 5) {
        setGameState(GameState::VICTORY);
    } else {
        loadLevel(nextLevelNumber);
    }
}

void LevelManager::reset() {
    currentLevel = 0;
    currentRound = 0;
    zombies.clear();
    doctor.reset();
    tutorialComplete = false;
    tutorialZombiesSpawned = false;
    currentDialogIndex = 0;
    showingDialog = false;
    roundTransitionTimer = 0.0f;
    inRoundTransition = false;
    setGameState(GameState::TUTORIAL);
}

GameState LevelManager::getCurrentState() const {
    return gameState;
}

void LevelManager::setGameState(GameState state) {
    gameState = state;
}

void LevelManager::startTutorial() {
    currentLevel = 0;
    currentRound = 0;
    tutorialComplete = false;
    currentDialogIndex = 0;
    showingDialog = true;
    spawnDoctor(400, 300);
}

bool LevelManager::isTutorialComplete() const {
    return tutorialComplete;
}

int LevelManager::getCurrentLevel() const {
    return currentLevel;
}

int LevelManager::getCurrentRound() const {
    return currentRound;
}

int LevelManager::getPreviousLevel() const {
    return previousLevel;
}

void LevelManager::spawnDoctor(float x, float y) {
    doctor.emplace(x, y);
}

void LevelManager::drawDoctor(sf::RenderWindow& window) {
    if (doctor) {
        doctor->draw(window);
    }
}

bool LevelManager::isPlayerNearDoctor(const Player& player) const {
    if (!doctor) return false;
    
    sf::Vector2f playerPos = player.getPosition();
    sf::Vector2f doctorPos = doctor->getPosition();
    
    float dx = playerPos.x - doctorPos.x;
    float dy = playerPos.y - doctorPos.y;
    float distance = std::sqrt(dx*dx + dy*dy);
    
    return distance < 100.0f;
}

void LevelManager::spawnZombies(int count, sf::Vector2f playerPos) {
    zombies.clear();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distribX(50, MAP_SIZE.x - 50);
    std::uniform_real_distribution<> distribY(50, MAP_SIZE.y - 50);
    std::uniform_int_distribution<> typeDist(0, currentLevel - 1);
    
    for (int i = 0; i < count; ++i) {
        float x, y;
        float minDistance = 200.0f;
        
        do {
            x = distribX(gen);
            y = distribY(gen);
            
            float dx = x - playerPos.x;
            float dy = y - playerPos.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            
            if (distance >= minDistance) {
                break;
            }
        } while (true);
        
        if (gameState == GameState::TUTORIAL) {
            zombies.emplace_back(ZombieWalker(x, y));
        } else if (gameState == GameState::LEVEL5) {
            if (i == 0) {
                zombies.emplace_back(ZombieKing(x, y));
            } else {
                int type = typeDist(gen);
                switch (type % 4) {
                    case 0: zombies.emplace_back(ZombieWalker(x, y)); break;
                    case 1: zombies.emplace_back(ZombieCrawler(x, y)); break;
                    case 2: zombies.emplace_back(ZombieTank(x, y)); break;
                    case 3: zombies.emplace_back(ZombieZoom(x, y)); break;
                }
            }
        } else {
            int type = typeDist(gen);
            switch (type % currentLevel) {
                case 0: zombies.emplace_back(ZombieWalker(x, y)); break;
                case 1: if (currentLevel >= 2) zombies.emplace_back(ZombieCrawler(x, y)); else zombies.emplace_back(ZombieWalker(x, y)); break;
                case 2: if (currentLevel >= 3) zombies.emplace_back(ZombieTank(x, y)); else zombies.emplace_back(ZombieWalker(x, y)); break;
                case 3: if (currentLevel >= 4) zombies.emplace_back(ZombieZoom(x, y)); else zombies.emplace_back(ZombieWalker(x, y)); break;
            }
        }
    }
}

void LevelManager::updateZombies(float deltaTime, const Player& player) {
    for (auto& zombie : zombies) {
        zombie.update(deltaTime, player.getPosition());
    }
    
    auto it = zombies.begin();
    while (it != zombies.end()) {
        bool erased = false;
        if (player.isAttacking() && player.getAttackBounds().intersects(it->getHitbox())) {
            it->takeDamage(player.getAttackDamage());
            if (it->isDead()) {
                it = zombies.erase(it);
                erased = true;
            }
        }
        if (!erased) {
            ++it;
        }
    }
}

void LevelManager::drawZombies(sf::RenderWindow& window) const {
    for (const auto& zombie : zombies) {
        zombie.draw(window);
    }
}

std::vector<Zombie>& LevelManager::getZombies() {
    return zombies;
}

void LevelManager::drawHUD(sf::RenderWindow& window, const Player& player) {
    sf::Vector2u windowSize = window.getSize();
    float barWidth = 200;
    float barHeight = 20;
    float padding = 10;

    // Draw Health Bar (Top-Right)
    sf::RectangleShape healthBarBackground;
    healthBarBackground.setSize(sf::Vector2f(barWidth, barHeight));
    healthBarBackground.setFillColor(sf::Color(100, 100, 100, 150));
    healthBarBackground.setPosition(windowSize.x - barWidth - padding, padding);
    
    sf::RectangleShape healthBar;
    float healthPercent = player.getCurrentHealth() / player.getMaxHealth();
    if (healthPercent < 0) healthPercent = 0;
    healthBar.setSize(sf::Vector2f(barWidth * healthPercent, barHeight));
    healthBar.setFillColor(sf::Color::Red);
    healthBar.setPosition(windowSize.x - barWidth - padding, padding);

    window.draw(healthBarBackground);
    window.draw(healthBar);

    // Draw Stamina Bar (Bottom-Middle)
    sf::RectangleShape staminaBarBackground;
    staminaBarBackground.setSize(sf::Vector2f(barWidth, barHeight));
    staminaBarBackground.setFillColor(sf::Color(100, 100, 100, 150));
    staminaBarBackground.setPosition((windowSize.x - barWidth) / 2.0f, windowSize.y - barHeight - padding);
    
    sf::RectangleShape staminaBar;
    float staminaPercent = player.getCurrentStamina() / player.getMaxStamina();
    if (staminaPercent < 0) staminaPercent = 0;
    staminaBar.setSize(sf::Vector2f(barWidth * staminaPercent, barHeight));
    staminaBar.setFillColor(sf::Color::Green);
    staminaBar.setPosition((windowSize.x - barWidth) / 2.0f, windowSize.y - barHeight - padding);
    
    window.draw(staminaBarBackground);
    window.draw(staminaBar);
    
    // Draw level and round info (Top-Left)
    sf::Text levelText;
    levelText.setFont(font);
    levelText.setCharacterSize(18);
    levelText.setFillColor(sf::Color::White);
    
    if (gameState == GameState::TUTORIAL) {
        levelText.setString("Tutorial");
    } else if (gameState == GameState::LEVEL5) {
        levelText.setString("Boss Level");
    } else {
        levelText.setString("Level " + std::to_string(currentLevel) + " - Round " + std::to_string(currentRound + 1));
    }
    
    levelText.setPosition(10, 40);
    window.draw(levelText);
    
    // Draw zombies remaining
    sf::Text zombiesText;
    zombiesText.setFont(font);
    zombiesText.setCharacterSize(18);
    zombiesText.setFillColor(sf::Color::White);
    zombiesText.setString("Zombies: " + std::to_string(zombies.size()));
    zombiesText.setPosition(10, 70);
    window.draw(zombiesText);
}

void LevelManager::showTutorialDialog(sf::RenderWindow& window) {
    if (currentDialogIndex >= tutorialDialogs.size()) {
        showingDialog = false;
        return;
    }
    
    dialogText.setString(tutorialDialogs[currentDialogIndex]);
    
    sf::Vector2u windowSize = window.getSize();
    dialogBox.setPosition((windowSize.x - dialogBox.getSize().x) / 2, windowSize.y - dialogBox.getSize().y - 20);
    
    dialogText.setPosition(
        dialogBox.getPosition().x + 20,
        dialogBox.getPosition().y + 20
    );
    
    window.draw(dialogBox);
    window.draw(dialogText);
    
    sf::Text promptText;
    promptText.setFont(font);
    promptText.setCharacterSize(14);
    promptText.setFillColor(sf::Color::Yellow);
    promptText.setString("Press SPACE to continue...");
    promptText.setPosition(
        dialogBox.getPosition().x + dialogBox.getSize().x - 200,
        dialogBox.getPosition().y + dialogBox.getSize().y - 30
    );
    window.draw(promptText);
}

void LevelManager::advanceDialog() {
    if (showingDialog) {
        currentDialogIndex++;
        if (currentDialogIndex >= tutorialDialogs.size()) {
            showingDialog = false;
        }
    }
}

int LevelManager::getZombieCountForLevel(int level) {
    switch (level) {
        case 1: return 10;
        case 2: return 12;
        case 3: return 15;
        case 4: return 18;
        case 5: return 10; // 1 ZombieKing + 9 others
        default: return 10;
    }
}
