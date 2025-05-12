#include "Game.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>

#include <SFML/Audio.hpp>
Game::Game() : window(sf::VideoMode(800, 600), "Echoes of Valkyrie"), points(0), zombiesToNextLevel(15) {
    window.setFramerateLimit(60);
    
    // Set background
    background.setFillColor(sf::Color(50, 50, 50));
    background.setSize(sf::Vector2f(1000, 1250)); // Match map size
    
    // Load map texture
    if (!mapTexture.loadFromFile("TDCod/Assets/Map/Map1/Map.png")) {
        std::cerr << "Error loading map texture!" << std::endl;
    }
    mapSprite.setTexture(mapTexture);
    
    // Load font
    if (!font.loadFromFile("Cutscene/Assets/Call of Ops Duty.otf")) {
        std::cerr << "Error loading font 'Cutscene/Assets/Call of Ops Duty.otf'! Trying fallback." << std::endl;
        // Fallback to a generic system font name if the primary one fails
        if (!font.loadFromFile("arial.ttf")) { // This is a common system font, but might not exist
            std::cerr << "Error loading fallback font 'arial.ttf' as well!" << std::endl;
        }
    }
    
    // Setup points text
    pointsText.setFont(font);
    pointsText.setCharacterSize(24);
    pointsText.setFillColor(sf::Color::White);
    pointsText.setPosition(10, 10);
    
    // Initialize game view
    gameView.setSize(800, 600);
    gameView.setCenter(400, 300);
    
    // Initialize level manager
    levelManager.initialize();
    // Load sounds
    if (!backgroundMusic.openFromFile("TDCod/Assets/Audio/atmosphere.mp3")) {
        std::cerr << "Error loading background music! Path: TDCod/Assets/Audio/atmosphere.mp3" << std::endl;
        // Optionally set a flag here to indicate loading failure
    } else {
        backgroundMusic.setVolume(15); // Lower the volume further
    }

    if (!zombieBiteBuffer.loadFromFile("TDCod/Assets/Audio/zombiebite.mp3")) {
        std::cerr << "Error loading zombie bite sound!" << std::endl;
    } else {
        zombieBiteSound.setBuffer(zombieBiteBuffer);
    }
}

void Game::run() {
    while (window.isOpen()) {
        processInput();
        float deltaTime = clock.restart().asSeconds();

        // Manage background music based on game state
        GameState currentState = levelManager.getCurrentState();
        if (currentState != GameState::GAME_OVER && currentState != GameState::VICTORY) {
            if (backgroundMusic.getStatus() != sf::Music::Playing) {
                backgroundMusic.setLoop(true);
                backgroundMusic.play();
            }
        } else {
            // Stop background music in GAME_OVER or VICTORY states
            if (backgroundMusic.getStatus() == sf::Music::Playing) backgroundMusic.stop();
        }

        update(deltaTime);
        render();
    }
}

void Game::processInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
            
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape)
                window.close();
            if (event.key.code == sf::Keyboard::Space) {
                levelManager.advanceDialog(); // Advance tutorial dialog
            }
        }
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                player.attack(); // Call the player's attack function
            }
        }
    }
}

void Game::update(float deltaTime) {
    // Get mouse position in world coordinates
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    sf::Vector2f worldMousePosition = window.mapPixelToCoords(mousePosition, gameView); // Use gameView for mapping

    // Update player with window, map size, and world mouse position
    player.update(deltaTime, window, mapTexture.getSize(), worldMousePosition);
    
    // Update level manager
    levelManager.update(deltaTime, player);
    
    
    // Check for collisions between player and zombies
    checkZombiePlayerCollisions();
    
    // Ensure player stays within map boundaries
    checkPlayerBoundaries();
    
    // Camera follows player
    sf::Vector2f playerPosition = player.getPosition();
    gameView.setCenter(playerPosition);
    
    // Clamp camera view to map bounds
    float cameraLeft = gameView.getCenter().x - gameView.getSize().x / 2;
    float cameraTop = gameView.getCenter().y - gameView.getSize().y / 2;
    float cameraRight = gameView.getCenter().x + gameView.getSize().x / 2;
    float cameraBottom = gameView.getCenter().y + gameView.getSize().y / 2;
    
    if (cameraLeft < 0) {
        gameView.setCenter(gameView.getSize().x / 2, gameView.getCenter().y);
    }
    if (cameraTop < 0) {
        gameView.setCenter(gameView.getCenter().x, gameView.getSize().y / 2);
    }
    if (cameraRight > 1000) {
        gameView.setCenter(1000 - gameView.getSize().x / 2, gameView.getCenter().y);
    }
    if (cameraBottom > 1250) {
        gameView.setCenter(gameView.getCenter().x, 1250 - gameView.getSize().y / 2);
    }
}

void Game::render() {
    window.clear();
    
    // Set the game view for world rendering
    window.setView(gameView);
    
    // Draw map
    window.draw(mapSprite);
    
    // Draw zombies through level manager
    levelManager.render(window);
    
    // Draw player
    player.render(window);
    
    // Switch to default view for UI elements
    window.setView(window.getDefaultView());
    
    // Update points text
    pointsText.setString("Points: " + std::to_string(points));
    window.draw(pointsText);
    
    // Draw level UI (dialogs)
    levelManager.renderUI(window, font);
    
    // Draw HUD (health, stamina, etc.)
    levelManager.drawHUD(window, player);
    
    // Display everything
    window.display();
}

void Game::checkZombiePlayerCollisions() {
    std::vector<Zombie>& zombies = levelManager.getZombies();
    
    for (auto& zombie : zombies) {
        if (!zombie.isAlive()) continue;
        
        // Check for player-zombie collision
        sf::FloatRect playerHitbox = player.getHitbox();
        sf::FloatRect zombieHitbox = zombie.getHitbox();

        bool pzCollision = isCollision(playerHitbox, zombieHitbox);
        // Check for player-zombie collision AND if the zombie is currently attacking
        if (pzCollision && zombie.isAttacking()) {
            // Use tryDealDamage to ensure damage is only applied once per attack animation
            float damageToApply = zombie.tryDealDamage();
            if (damageToApply > 0) {
                player.takeDamage(damageToApply);
                
                // Play zombie bite sound
                if (zombieBiteSound.getStatus() != sf::Sound::Playing) {
                    zombieBiteSound.play();
                }

                // Knockback player away from zombie
                sf::Vector2f direction = player.getPosition() - zombie.getPosition();
                float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);
                if (magnitude > 0) {
                    direction.x /= magnitude;
                    direction.y /= magnitude;
                    player.applyKnockback(direction, 50.0f);
                }
            }
        }
        
        // Check for player attack-zombie collision
        if (player.isAttacking()) {
            sf::FloatRect playerAttackBox = player.getAttackHitbox();
            // Zombie hitbox already fetched as zombieHitbox
            bool paZCollision = isCollision(playerAttackBox, zombieHitbox);
            if (paZCollision) {
                zombie.takeDamage(player.getAttackDamage());
                
                // If zombie defeated
                if (!zombie.isAlive()) {
                    points += 10;
                    zombiesToNextLevel--;
                    
                    // Check for level completion
                    if (zombiesToNextLevel <= 0) {
                        levelManager.nextLevel();
                        zombiesToNextLevel = 15 + levelManager.getCurrentLevel() * 5; // Increase zombies per level
                    }
                }
            }
        }
    }
}

void Game::checkPlayerBoundaries() {
    sf::Vector2f playerPos = player.getPosition();
    sf::Vector2u mapSize = mapTexture.getSize();
    float playerRadius = 20.0f; // Approximate player hitbox radius
    
    // Constrain player to map boundaries
    if (playerPos.x - playerRadius < 0) {
        player.setPosition(playerRadius, playerPos.y);
    }
    if (playerPos.y - playerRadius < 0) {
        player.setPosition(playerPos.x, playerRadius);
    }
    if (playerPos.x + playerRadius > mapSize.x) {
        player.setPosition(mapSize.x - playerRadius, playerPos.y);
    }
    if (playerPos.y + playerRadius > mapSize.y) {
        player.setPosition(playerPos.x, mapSize.y - playerRadius);
    }
}

bool Game::isCollision(const sf::FloatRect& rect1, const sf::FloatRect& rect2) {
    return rect1.intersects(rect2);
}

void Game::addPoints(int amount) {
    points += amount;
}

int Game::getPoints() const {
    return points;
}

void Game::reset() {
    points = 0;
    zombiesToNextLevel = 15;
    levelManager.reset();
    player.reset();
}
sf::RenderWindow& Game::getWindow() {
    return window;
}