#include "Game.h" 
#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>
#include <vector>

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
    if (!font.loadFromFile("TDCod/Assets/Fonts/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        // Try system font as fallback
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading fallback font as well!" << std::endl;
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
}

void Game::run() {
    while (window.isOpen()) {
        processInput();
        float deltaTime = clock.restart().asSeconds();
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
    // Update player with window and map size
    player.update(deltaTime, window, mapTexture.getSize());
    
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
    
    // Draw level UI
    levelManager.renderUI(window, font);
    
    // Display everything
    window.display();
}

void Game::checkZombiePlayerCollisions() {
    std::vector<Zombie>& zombies = levelManager.getZombies();
    
    for (auto& zombie : zombies) {
        if (!zombie.isAlive()) continue;
        
        // Check for player-zombie collision
        if (isCollision(player.getHitbox(), zombie.getHitbox())) {
            player.takeDamage(zombie.getDamage());
            
            // Knockback player away from zombie
            sf::Vector2f direction = player.getPosition() - zombie.getPosition();
            float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);
            if (magnitude > 0) {
                direction.x /= magnitude;
                direction.y /= magnitude;
                player.applyKnockback(direction, 50.0f);
            }
        }
        
        // Check for player attack-zombie collision
        if (player.isAttacking()) {
            if (isCollision(player.getAttackHitbox(), zombie.getHitbox())) {
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