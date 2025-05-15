#include "LevelManager.h"
#include <memory> // Required for std::unique_ptr
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
      inRoundTransition(false),
      levelTransitionTimer(0.0f),
      levelTransitionDuration(3.0f), // Increased duration to 3 seconds
      levelTransitioning(false),
      transitionState(TransitionState::NONE),
      totalZombiesInRound(0), // Initialize new members
      zombiesSpawnedInRound(0),
      zombiesKilledInRound(0),
      zombieSpawnTimer(0.0f),
      zombieSpawnInterval(1.0f),
      roundStarted(false) { // Initialize roundStarted
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
    
    // Load level start sound
    if (!levelStartBuffer.loadFromFile("TDCod/Assets/Audio/wavestart.mp3")) {
        std::cerr << "Error loading level start sound!" << std::endl;
    }
    levelStartSound.setBuffer(levelStartBuffer);
    
    // Setup transition rectangle
    transitionRect.setSize(sf::Vector2f(1000, 1250)); // Match map size
    transitionRect.setFillColor(sf::Color::Black);
    transitionRect.setPosition(0, 0);
    
    // Setup level start text
    levelStartText.setFont(font);
    levelStartText.setCharacterSize(48);
    levelStartText.setFillColor(sf::Color::White);
    levelStartText.setPosition(300, 500); // Adjust position as needed
}

void LevelManager::initialize() {
    setGameState(GameState::TUTORIAL);
    startTutorial();
}

void LevelManager::update(float deltaTime, Player& player) {
    previousLevel = currentLevel;

    // Handle level transition
    if (levelTransitioning) {
        levelTransitionTimer += deltaTime;

        if (transitionState == TransitionState::FADE_IN) {
            float alpha = 255 * (levelTransitionTimer / levelTransitionDuration);
            transitionRect.setFillColor(sf::Color(0, 0, 0, static_cast<int>(alpha)));
            // Text appears during fade-in
            if (levelTransitionTimer >= levelTransitionDuration) {
                transitionState = TransitionState::SHOW_TEXT;
                levelTransitionTimer = 0.0f;
                // Sound is played at the start of the transition in loadLevel
            }
        } else if (transitionState == TransitionState::SHOW_TEXT) {
             // Show text for a fixed duration
            if (levelTransitionTimer >= 1.0f) { // Show text for 1 second
                transitionState = TransitionState::FADE_OUT;
                levelTransitionTimer = 0.0f;
            }
        } else if (transitionState == TransitionState::FADE_OUT) {
            float alpha = 255 - (255 * (levelTransitionTimer / levelTransitionDuration));
            transitionRect.setFillColor(sf::Color(0, 0, 0, static_cast<int>(alpha)));
            if (levelTransitionTimer >= levelTransitionDuration) {
                levelTransitioning = false;
                transitionState = TransitionState::NONE;
                levelTransitionTimer = 0.0f;
                // Level fully starts after transition finishes
                // Spawn zombies for Round 0 of the new level
                spawnZombies(getZombieCountForLevel(currentLevel, currentRound), player.getPosition());
            }
        }
        return; // Skip other updates during transition
    }

    // Handle round transition delay
    if (inRoundTransition) {
        roundTransitionTimer += deltaTime;
        if (roundTransitionTimer >= 2.0f) { // 2-second pause between rounds
            inRoundTransition = false;
            roundTransitionTimer = 0.0f;
            spawnZombies(getZombieCountForLevel(currentLevel, currentRound), player.getPosition()); // Prepare zombies for spawning
        }
        return;
    }

    // Zombie Spawning Logic
    if (zombiesSpawnedInRound < totalZombiesInRound && !zombiesToSpawn.empty()) {
        zombieSpawnTimer += deltaTime;
        if (zombieSpawnTimer >= zombieSpawnInterval) {
            // Move zombie from zombiesToSpawn to active zombies
            zombies.push_back(std::move(zombiesToSpawn.front()));
            zombiesToSpawn.erase(zombiesToSpawn.begin());
            zombiesSpawnedInRound++;
            zombieSpawnTimer = 0.0f; // Reset timer
        }
    }

    switch (gameState) {
        case GameState::TUTORIAL:
            if (isPlayerNearDoctor(player) && !showingDialog) {
                showingDialog = true;
            }

            // In tutorial, zombies are spawned directly, not through interval spawning
            if (tutorialZombiesSpawned) {
                 updateZombies(deltaTime, player);
            }

            if (currentDialogIndex >= tutorialDialogs.size() && !tutorialZombiesSpawned) {
                if (doctor) {
                    // In tutorial, spawn zombies directly for simplicity
                    zombies.push_back(std::make_unique<ZombieWalker>(doctor->getPosition().x + 100, doctor->getPosition().y));
                    zombies.push_back(std::make_unique<ZombieWalker>(doctor->getPosition().x - 100, doctor->getPosition().y));
                    zombies.push_back(std::make_unique<ZombieWalker>(doctor->getPosition().x, doctor->getPosition().y + 100));
                    totalZombiesInRound = 3; // Set total for tutorial
                    zombiesSpawnedInRound = 3; // All spawned at once in tutorial
                }
                tutorialZombiesSpawned = true;
            }

            // Tutorial completion check based on killed zombies
            if (zombiesKilledInRound == totalZombiesInRound && tutorialZombiesSpawned) {
                tutorialComplete = true;
                pendingLevelTransition = true; // Set flag instead of loading level and changing state
                showingDialog = false; // Explicitly hide dialog after tutorial completion
            }
            break;

        case GameState::LEVEL1:
        case GameState::LEVEL2:
        case GameState::LEVEL3:
        case GameState::LEVEL4:
        case GameState::LEVEL5:
            updateZombies(deltaTime, player);

            // Round/Level Progression Logic
            // Round/Level Progression Logic
            // Round/Level Progression Logic
            if (roundStarted && zombiesKilledInRound == totalZombiesInRound && zombiesSpawnedInRound == totalZombiesInRound && !inRoundTransition && !levelTransitioning && transitionState == TransitionState::NONE) {
                // Check if there are more rounds in the current level
                if (currentRound < getTotalRoundsForLevel(currentLevel) - 1) {
                    currentRound++;
                    inRoundTransition = true; // Start transition delay
                    roundStarted = false; // Reset roundStarted for the next round
                } else {
                    // All rounds for the current level are complete, move to the next level or boss fight
                    if (currentLevel >= 1 && currentLevel <= 4) { // Levels 1, 2, 3, 4
                         int nextLevel = currentLevel + 1;
                         if (nextLevel > 5) {
                             setGameState(GameState::VICTORY);
                         } else {
                             loadLevel(nextLevel); // loadLevel prepares next round/level
                             roundStarted = false; // Reset roundStarted for the next level
                         }
                    } else if (gameState == GameState::LEVEL5) { // Level 5 (Boss Level before Boss Fight)
                         setGameState(GameState::BOSS_FIGHT); // Transition to boss fight state
                         roundStarted = false; // Reset roundStarted for the boss fight
                    }
                }
            }
            break;

        case GameState::BOSS_FIGHT:
            // Boss spawning is handled by the general spawning logic now
            updateZombies(deltaTime, player);
            // Boss defeated condition
            if (zombiesKilledInRound == totalZombiesInRound && zombiesSpawnedInRound == totalZombiesInRound) {
                 setGameState(GameState::VICTORY);
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
    
    // Draw level transition
    if (levelTransitioning) {
        window.draw(transitionRect);
    }
    
    // Draw level transition text during FADE_IN and SHOW_TEXT states
    // Draw level transition text during FADE_IN and SHOW_TEXT states
    // Draw level transition text during FADE_IN and SHOW_TEXT states
    if (levelTransitioning && (transitionState == TransitionState::FADE_IN || transitionState == TransitionState::SHOW_TEXT)) {
        std::string levelTextString;
        // Display the next level number based on the previous level
        // Display the next level number based on the current level after loadLevel
        if (currentLevel >= 1 && currentLevel <= 5) {
             levelTextString = "Level " + std::to_string(currentLevel) + " Starting";
        } else {
            levelTextString = "Starting Level"; // Fallback text
        }
        levelStartText.setString(levelTextString);
        sf::Vector2u windowSize = window.getSize();
        levelStartText.setPosition((windowSize.x - levelStartText.getGlobalBounds().width) / 2, windowSize.y / 2 - 50);
        window.draw(levelStartText);
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
    zombiesToSpawn.clear(); // Clear zombies waiting to spawn
    doctor.reset();
    tutorialComplete = false;
    tutorialZombiesSpawned = false;
    currentDialogIndex = 0;
    showingDialog = false;
    roundTransitionTimer = 0.0f;
    inRoundTransition = false;
    levelTransitionTimer = 0.0f;
    levelTransitioning = false;
    transitionState = TransitionState::NONE;
    totalZombiesInRound = 0; // Reset new members
    zombiesSpawnedInRound = 0;
    zombiesKilledInRound = 0;
    zombieSpawnTimer = 0.0f;
    zombieSpawnInterval = 1.0f;
    roundStarted = false; // Reset roundStarted
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

void LevelManager::loadLevel(int levelNumber) {
    previousLevel = currentLevel; // Store previous level before changing
    currentLevel = levelNumber;
    currentRound = 0;
    zombies.clear();
    zombiesToSpawn.clear(); // Clear any zombies waiting to spawn from the previous level
    totalZombiesInRound = 0; // Reset zombie counts for the new level
    zombiesSpawnedInRound = 0;
    zombiesKilledInRound = 0;
    roundStarted = false; // Ensure roundStarted is false at the start of a new level

    // Only trigger transition for specific level changes after Round 2 (except Tutorial to 1 and 4 to 5)
    bool triggerTransition = false;
    // Only trigger transition for specific level changes after Round 2 (Levels 1-3 to next level) and Level 4 to 5
    if ((previousLevel >= 1 && previousLevel <= 3) && currentLevel == previousLevel + 1) { // Levels 1-3 to next level
        // Check if previous level's Round 2 was completed
        // This logic needs to be handled in the update loop when round 2 is cleared
        // For now, we'll assume the call to loadLevel means round 2 was cleared
        triggerTransition = true;
    } else if (previousLevel == 4 && currentLevel == 5) { // Level 4 to Level 5 (Boss)
         triggerTransition = true;
    }


    if (triggerTransition) {
        levelTransitioning = true;
        levelTransitionTimer = 0.0f;
        transitionState = TransitionState::FADE_IN;
        levelStartSound.play(); // Play sound at the start of the transition
        // Zombies will be spawned after the transition finishes
    } else {
        // If no transition, spawn zombies immediately
        spawnZombies(getZombieCountForLevel(currentLevel, currentRound), sf::Vector2f(400, 300));
    }
}

// Function to get the total number of rounds for a given level
int LevelManager::getTotalRoundsForLevel(int level) const {
    switch (level) {
        case 1: return 2; // Level 1 has 2 rounds (0 and 1)
        case 2: return 2; // Level 2 has 2 rounds (0 and 1)
        case 3: return 2; // Level 3 has 2 rounds (0 and 1)
        case 4: return 2; // Level 4 has 2 rounds (0 and 1)
        case 5: return 1; // Level 5 (Boss) has 1 round (0)
        default: return 1; // Default to 1 round for other cases
    }
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
    zombiesToSpawn.clear(); // Clear previous zombies waiting to spawn
    totalZombiesInRound = count;
    zombiesSpawnedInRound = 0;
    zombiesKilledInRound = 0;
    zombieSpawnTimer = 0.0f; // Reset spawn timer
    zombieSpawnInterval = 1.0f; // Set a default spawn interval (can be adjusted per level/round)

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distribX(50, MAP_SIZE.x - 50);
    std::uniform_real_distribution<> distribY(50, MAP_SIZE.y - 50);
    std::uniform_int_distribution<> typeDist(0, currentLevel > 0 ? currentLevel - 1 : 0);

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
            zombiesToSpawn.push_back(std::make_unique<ZombieWalker>(x, y));
        } else if (gameState == GameState::BOSS_FIGHT) {
             // Only one boss zombie
            if (zombiesToSpawn.empty()) {
                 zombiesToSpawn.push_back(std::make_unique<ZombieKing>(x, y));
            }
        } else {
            int type = typeDist(gen);
            switch (type % (currentLevel > 0 ? currentLevel : 1)) {
                case 0: zombiesToSpawn.push_back(std::make_unique<ZombieWalker>(x, y)); break;
                case 1: if (currentLevel >= 2) zombiesToSpawn.push_back(std::make_unique<ZombieCrawler>(x, y)); else zombiesToSpawn.push_back(std::make_unique<ZombieWalker>(x, y)); break;
                case 2: if (currentLevel >= 3) zombiesToSpawn.push_back(std::make_unique<ZombieTank>(x, y)); else zombiesToSpawn.push_back(std::make_unique<ZombieWalker>(x, y)); break;
                case 3: if (currentLevel >= 4) zombiesToSpawn.push_back(std::make_unique<ZombieZoom>(x, y)); else zombiesToSpawn.push_back(std::make_unique<ZombieWalker>(x, y)); break;
                default: zombiesToSpawn.push_back(std::make_unique<ZombieWalker>(x, y)); break; // Default case to ensure a zombie is always added
            }
        }
    }
    roundStarted = true; // Indicate that the round has started and zombies are prepared
}

void LevelManager::updateZombies(float deltaTime, const Player& player) {
    for (auto& zombie : zombies) {
        zombie->update(deltaTime, player.getPosition());
    }

    auto it = zombies.begin();
    while (it != zombies.end()) {
        bool erased = false;
        if (player.isAttacking() && player.getAttackBounds().intersects((*it)->getHitbox())) {
            (*it)->takeDamage(player.getAttackDamage());
            if ((*it)->isDead()) {
                it = zombies.erase(it);
                erased = true;
                zombiesKilledInRound++; // Increment killed count
            }
        }
        if (!erased) {
            ++it;
        }
    }
}

void LevelManager::drawZombies(sf::RenderWindow& window) const {
    for (const auto& zombie : zombies) {
        zombie->draw(window);
    }
}

std::vector<std::unique_ptr<BaseZombie>>& LevelManager::getZombies() {
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
    // Draw level and round info (Top-Left) and zombie count only if tutorial dialog is not showing
    // Draw level and round info (Top-Left) and zombie count only if tutorial dialog is not showing
    // Draw level and round info (Top-Left) and zombie count
    // Always show HUD elements except during level transitions where they are faded
    if (!levelTransitioning || (levelTransitioning && (transitionState == TransitionState::FADE_OUT || transitionState == TransitionState::FADE_IN))) {
        sf::Text levelText;
        levelText.setFont(font);
        levelText.setCharacterSize(18);
        levelText.setPosition(padding, padding + 30); // Position below points text (points are at 10, 10)

        sf::Text zombieCountText;
        zombieCountText.setFont(font);
        zombieCountText.setCharacterSize(18);
        zombieCountText.setFillColor(sf::Color::White); // Set default color
        zombieCountText.setPosition(padding, padding + 50); // Position below level text

        // Calculate alpha for fading during level transition
        sf::Uint8 alpha = 255;
        if (levelTransitioning) {
            if (transitionState == TransitionState::FADE_IN) {
                alpha = static_cast<sf::Uint8>(255 * (1.0f - (levelTransitionTimer / levelTransitionDuration))); // Fade out during fade in
            } else if (transitionState == TransitionState::FADE_OUT) {
                 alpha = static_cast<sf::Uint8>(255 * (levelTransitionTimer / levelTransitionDuration)); // Fade in during fade out
            } else {
                alpha = 0; // Fully transparent during SHOW_TEXT
            }
        }

        levelText.setFillColor(sf::Color(255, 255, 255, alpha));
        zombieCountText.setFillColor(sf::Color(255, 255, 255, alpha));


        if (currentLevel == 0) {
            levelText.setString("Tutorial");
        }
        else if (gameState == GameState::BOSS_FIGHT) {
            levelText.setString("Boss Level");
        }
        else {
            // Display "Level X - Round Y"
            levelText.setString("Level " + std::to_string(currentLevel) + " - Round " + std::to_string(currentRound + 1));
        }
        window.draw(levelText);

        // Draw zombie count
        zombieCountText.setString("Zombies: " + std::to_string(zombies.size() + zombiesToSpawn.size())); // Include zombies waiting to spawn in count
        window.draw(zombieCountText);
    }
}
void LevelManager::showTutorialDialog(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    dialogBox.setPosition((windowSize.x - dialogBox.getSize().x) / 2, windowSize.y - dialogBox.getSize().y - 20);
    dialogText.setPosition((dialogBox.getPosition().x + 10), dialogBox.getPosition().y + 10);

    if (currentDialogIndex < tutorialDialogs.size()) {
        dialogText.setString(tutorialDialogs[currentDialogIndex]);
    }

    window.draw(dialogBox);
    window.draw(dialogText);

    // Add "Press Space to Continue..." text
    if (currentDialogIndex < tutorialDialogs.size()) {
        sf::Text continueText;
        continueText.setFont(font);
        continueText.setCharacterSize(18);
        continueText.setFillColor(sf::Color::Yellow);
        continueText.setString("Press Space to Continue...");
        continueText.setPosition(dialogBox.getPosition().x + dialogBox.getSize().x - continueText.getGlobalBounds().width - 10, dialogBox.getPosition().y + dialogBox.getSize().y - continueText.getGlobalBounds().height - 10);
        window.draw(continueText);
    }
}

void LevelManager::advanceDialog() {
    currentDialogIndex++;
    if (currentDialogIndex < tutorialDialogs.size()) {
        showingDialog = true;
    } else {
        showingDialog = false; // Hide dialog after the last one
    }
}

int LevelManager::getZombieCountForLevel(int level, int round) {
    switch (level) {
        case 0: return 3; // Tutorial
        case 1: return 5 + round;
        case 2: return 7;
        case 3: return 7;
        case 4: return 7;
        case 5: return 15;
        default: return 1; // Default to 1 round for other cases
    }
}
