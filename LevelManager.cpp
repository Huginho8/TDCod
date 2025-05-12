#include "LevelManager.h"
#include <iostream>
#include <random>
#include <cmath>

LevelManager::LevelManager() 
    : gameState(GameState::TUTORIAL), 
      currentLevel(0), 
      tutorialComplete(false),
      doctorSpawned(false),
      currentDialogIndex(0),
      showingDialog(false) {
    
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
    if (!font.loadFromFile("TDCod/Assets/Fonts/arial.ttf")) {
        std::cerr << "Error loading font for dialog!" << std::endl;
        // Fallback to system font if available
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading fallback font as well!" << std::endl;
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
    // Update based on current game state
    switch (gameState) {
        case GameState::TUTORIAL:
            // Check if player is near doctor to trigger dialog
            if (isPlayerNearDoctor(player) && !showingDialog) {
                showingDialog = true;
            }
            
            // Update zombies
            updateZombies(deltaTime, player);
            
            // Check if tutorial is complete (all zombies defeated)
            if (zombies.empty() && currentDialogIndex >= tutorialDialogs.size()) {
                tutorialComplete = true;
                loadLevel(1); // Proceed to level 1
            }
            break;
            
        case GameState::LEVEL1:
        case GameState::LEVEL2:
        case GameState::LEVEL3:
            // Update zombies
            updateZombies(deltaTime, player);
            
            // Check if level is complete (all zombies defeated)
            if (zombies.empty()) {
                int nextLevel = getCurrentLevel() + 1;
                if (nextLevel > 3) {
                    setGameState(GameState::VICTORY);
                } else {
                    loadLevel(nextLevel);
                }
            }
            break;
            
        case GameState::GAME_OVER:
        case GameState::VICTORY:
            // Nothing to update in these states
            break;
    }
    
    // Update doctor NPC
    if (doctorSpawned) {
        doctor.update(deltaTime, player.getPosition());
    }
}

void LevelManager::draw(sf::RenderWindow& window) {
    // Draw map boundaries (for debugging)
    // window.draw(mapBounds);
    
    // Draw zombies
    drawZombies(window);
    
    // Draw doctor if spawned
    if (doctorSpawned) {
        drawDoctor(window);
    }
    
    // Draw dialog if showing
    if (showingDialog && currentDialogIndex < tutorialDialogs.size()) {
        showTutorialDialog(window);
    }
}

// Alias for draw method to match Game.cpp usage
void LevelManager::render(sf::RenderWindow& window) {
    draw(window);
}

// Render UI elements
void LevelManager::renderUI(sf::RenderWindow& window, sf::Font& font) {
    // Draw dialog if showing
    if (showingDialog && currentDialogIndex < tutorialDialogs.size()) {
        showTutorialDialog(window);
    }
}

// Progress to next level
void LevelManager::nextLevel() {
    int nextLevelNumber = currentLevel + 1;
    if (nextLevelNumber > 3) {
        setGameState(GameState::VICTORY);
    } else {
        loadLevel(nextLevelNumber);
    }
}

// Reset level manager
void LevelManager::reset() {
    currentLevel = 0;
    zombies.clear();
    doctorSpawned = false;
    tutorialComplete = false;
    currentDialogIndex = 0;
    showingDialog = false;
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
    tutorialComplete = false;
    currentDialogIndex = 0;
    showingDialog = true;
    
    // Spawn doctor in tutorial
    spawnDoctor(400, 300);
    
    // Spawn a few zombies for the tutorial
    spawnZombies(3, doctor.getPosition());
}

bool LevelManager::isTutorialComplete() const {
    return tutorialComplete;
}

void LevelManager::loadLevel(int levelNumber) {
    currentLevel = levelNumber;
    
    switch (levelNumber) {
        case 1:
            setGameState(GameState::LEVEL1);
            spawnZombies(10, sf::Vector2f(500, 500));
            break;
        case 2:
            setGameState(GameState::LEVEL2);
            spawnZombies(15, sf::Vector2f(500, 500));
            break;
        case 3:
            setGameState(GameState::LEVEL3);
            spawnZombies(20, sf::Vector2f(500, 500));
            break;
        default:
            std::cerr << "Invalid level number: " << levelNumber << std::endl;
            break;
    }
}

int LevelManager::getCurrentLevel() const {
    return currentLevel;
}

void LevelManager::spawnDoctor(float x, float y) {
    doctor = Doctor(x, y);
    doctorSpawned = true;
}

void LevelManager::drawDoctor(sf::RenderWindow& window) {
    doctor.draw(window);
}

bool LevelManager::isPlayerNearDoctor(const Player& player) const {
    if (!doctorSpawned) return false;
    
    sf::Vector2f playerPos = player.getPosition();
    sf::Vector2f doctorPos = doctor.getPosition();
    
    // Calculate distance
    float dx = playerPos.x - doctorPos.x;
    float dy = playerPos.y - doctorPos.y;
    float distance = std::sqrt(dx*dx + dy*dy);
    
    // Return true if player is within 100 pixels of doctor
    return distance < 100.0f;
}

void LevelManager::spawnZombies(int count, sf::Vector2f playerPos) {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Define distribution for x and y, ensuring zombies spawn away from player
    std::uniform_real_distribution<> distribX(50, MAP_SIZE.x - 50);
    std::uniform_real_distribution<> distribY(50, MAP_SIZE.y - 50);
    
    for (int i = 0; i < count; ++i) {
        float x, y;
        float minDistance = 200.0f; // Minimum distance from player
        
        // Keep generating positions until we find one far enough from player
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
        
        zombies.emplace_back(x, y);
    }
}

void LevelManager::updateZombies(float deltaTime, const Player& player) {
    // Update all zombies
    for (auto& zombie : zombies) {
        zombie.update(deltaTime, player.getPosition());
    }
    
    // Check for collision with player attacks
    auto it = zombies.begin();
    while (it != zombies.end()) {
        if (player.isAttacking() && player.getAttackBounds().intersects(it->getBounds())) {
            // Hit detected, remove zombie
            it = zombies.erase(it);
        } else {
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
    // Draw stamina bar
    sf::RectangleShape staminaBar;
    staminaBar.setSize(sf::Vector2f(200 * (player.getCurrentStamina() / player.getMaxStamina()), 20));
    staminaBar.setFillColor(sf::Color::Green);
    staminaBar.setPosition(10, 10);
    
    sf::RectangleShape staminaBarBackground;
    staminaBarBackground.setSize(sf::Vector2f(200, 20));
    staminaBarBackground.setFillColor(sf::Color(100, 100, 100, 150));
    staminaBarBackground.setPosition(10, 10);
    
    window.draw(staminaBarBackground);
    window.draw(staminaBar);
    
    // Draw level info
    sf::Text levelText;
    levelText.setFont(font);
    levelText.setCharacterSize(18);
    levelText.setFillColor(sf::Color::White);
    
    if (gameState == GameState::TUTORIAL) {
        levelText.setString("Tutorial");
    } else {
        levelText.setString("Level " + std::to_string(currentLevel));
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
    
    // Set dialog text
    dialogText.setString(tutorialDialogs[currentDialogIndex]);
    
    // Position dialog box at bottom center of screen
    sf::Vector2u windowSize = window.getSize();
    dialogBox.setPosition((windowSize.x - dialogBox.getSize().x) / 2, windowSize.y - dialogBox.getSize().y - 20);
    
    // Position text inside dialog box
    dialogText.setPosition(
        dialogBox.getPosition().x + 20,
        dialogBox.getPosition().y + 20
    );
    
    // Draw dialog
    window.draw(dialogBox);
    window.draw(dialogText);
    
    // Prompt for next
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
