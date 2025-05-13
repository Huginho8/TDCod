#include "LevelManager.h"
#include <iostream>
#include <random>
#include <cmath>

LevelManager::LevelManager()
    : gameState(GameState::TUTORIAL),
      currentLevel(0),
      previousLevel(0), // Initialize previousLevel
      tutorialComplete(false),
      // doctorSpawned is no longer needed with std::optional
      tutorialZombiesSpawned(false), // Initialize new flag
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
    if (!font.loadFromFile("TDCod/Assets/Call of Ops Duty.otf")) {
        std::cerr << "Error loading font for dialog! Trying fallback." << std::endl;
        // Fallback to a generic system font name if the primary one fails
        if (!font.loadFromFile("arial.ttf")) { // This is a common system font, but might not exist
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
    previousLevel = currentLevel; // Store the current level before updating
    // Update based on current game state
    switch (gameState) {
        case GameState::TUTORIAL:
            // Check if player is near doctor to trigger dialog
            if (isPlayerNearDoctor(player) && !showingDialog) {
                showingDialog = true;
            }
            
            // Update zombies only if they have been spawned
            if (tutorialZombiesSpawned) {
                updateZombies(deltaTime, player);
            }
            
            // Check if dialogue is finished to spawn tutorial zombies
            if (currentDialogIndex >= tutorialDialogs.size() && !tutorialZombiesSpawned) {
                if (doctor) { // Ensure doctor exists before getting position
                    spawnZombies(3, doctor->getPosition());
                }
                tutorialZombiesSpawned = true;
            }

            // Check if tutorial is complete (dialogue finished AND all zombies defeated)
            if (zombies.empty() && tutorialZombiesSpawned && currentDialogIndex >= tutorialDialogs.size()) {
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
    if (doctor) {
        doctor->update(deltaTime, player.getPosition());
    }
}

void LevelManager::draw(sf::RenderWindow& window) {
    // Draw map boundaries (for debugging)
    // window.draw(mapBounds);
    
    // Draw zombies
    drawZombies(window);
    
    // Draw doctor if spawned
    if (doctor) {
        doctor->draw(window);
    }
    
    // Draw dialog if showing - This is now handled by renderUI
    // if (showingDialog && currentDialogIndex < tutorialDialogs.size()) {
    //     showTutorialDialog(window);
    // }
}

// Alias for draw method to match Game.cpp usage
void LevelManager::render(sf::RenderWindow& window) {
    draw(window);
}

// Render UI elements
void LevelManager::renderUI(sf::RenderWindow& window, sf::Font& fontToUse) { // Renamed param to avoid conflict with member
    // Draw dialog if showing
    if (showingDialog && currentDialogIndex < tutorialDialogs.size()) {
        // Ensure dialogText uses the font passed to renderUI or the member font
        dialogText.setFont(this->font); // Prefer member font for consistency if loaded
        // promptText is local to showTutorialDialog and will use this->font there.
        showTutorialDialog(window);
    }
    // It seems Game.cpp passes its own font to renderUI, but LevelManager also has a font member.
    // For HUD elements, we'll use the LevelManager's member font.
    // The Player object needs to be accessible here to draw its HUD.
    // This suggests drawHUD might be better called from Game::render directly,
    // or Player reference needs to be available in renderUI.
    // For now, assuming Player object is accessible via a getter in Game or passed differently.
    // This will likely require further changes in Game.cpp to pass the player object.
    // Temporarily, we can't draw player-specific HUD here without player reference.
    // Let's assume drawHUD will be called from Game::render where player is available.
}

// drawHUD needs to be called from Game::render or similar where Player is available.
// For now, I will modify it as requested, but the call site needs adjustment.

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
    doctor.reset(); // Clear the optional doctor
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
    
    // Zombies will now be spawned in update() after dialogue
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

int LevelManager::getPreviousLevel() const {
    return previousLevel;
}

void LevelManager::spawnDoctor(float x, float y) {
    doctor.emplace(x, y); // Construct Doctor in-place
}

void LevelManager::drawDoctor(sf::RenderWindow& window) {
    // This method is called within an 'if (doctor)' block in draw(),
    // so we can assume doctor has a value here.
    doctor->draw(window);
}

bool LevelManager::isPlayerNearDoctor(const Player& player) const {
    if (!doctor) return false; // Check if doctor exists
    
    sf::Vector2f playerPos = player.getPosition();
    sf::Vector2f doctorPos = doctor->getPosition(); // Access via ->
    
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
        bool erased = false;
        if (player.isAttacking() && player.getAttackBounds().intersects(it->getHitbox())) { // Use getHitbox()
            it->takeDamage(player.getAttackDamage()); // Apply damage
            if (it->isDead()) {
                it = zombies.erase(it); // Erase if dead
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
    healthBarBackground.setFillColor(sf::Color(100, 100, 100, 150)); // Dark grey background
    healthBarBackground.setPosition(windowSize.x - barWidth - padding, padding);
    
    sf::RectangleShape healthBar;
    float healthPercent = player.getCurrentHealth() / player.getMaxHealth();
    if (healthPercent < 0) healthPercent = 0; // Ensure not negative
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
    if (staminaPercent < 0) staminaPercent = 0; // Ensure not negative
    staminaBar.setSize(sf::Vector2f(barWidth * staminaPercent, barHeight));
    staminaBar.setFillColor(sf::Color::Green);
    staminaBar.setPosition((windowSize.x - barWidth) / 2.0f, windowSize.y - barHeight - padding);
    
    window.draw(staminaBarBackground);
    window.draw(staminaBar);
    
    // Draw level info (Top-Left)
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
