#include "Game.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>

Game::Game()
    : window(sf::VideoMode(800, 600), "Echoes of Valkyrie"),
      player(Vec2(400, 300)),
      points(0),
      zombiesToNextLevel(15),
      pointsToNextLevel(150),
      physics(),
      isPaused(false),
      showDebugInfo(false)
{
    window.setFramerateLimit(60);

    // Initialize background
    background.setFillColor(sf::Color(50, 50, 50));
    background.setSize(sf::Vector2f(1200, 1250));

    // Load map textures
    if (!mapTexture1.loadFromFile("TDCod/Assets/Map.png")) {
        std::cerr << "Error loading map1 texture!" << std::endl;
    }
    mapSprite1.setTexture(mapTexture1);

    if (!mapTexture2.loadFromFile("TDCod/Assets/Map/Map2/Map2.png")) {
        std::cerr << "Error loading map2 texture!" << std::endl;
    }
    mapSprite2.setTexture(mapTexture2);

    if (!mapTexture3.loadFromFile("TDCod/Assets/Map/Map3/Map3.png")) {
        std::cerr << "Error loading map3 texture!" << std::endl;
    }
    mapSprite3.setTexture(mapTexture3);

    // Load font
    if (!font.loadFromFile("TDCod/Assets/Call of Ops Duty.otf")) {
        std::cerr << "Error loading font 'TDCod/Assets/Call of Ops Duty.otf'! Trying fallback." << std::endl;
    }

    // Initialize points text
    pointsText.setFont(font);
    pointsText.setCharacterSize(24);
    pointsText.setFillColor(sf::Color::White);
    pointsText.setPosition(10, 10);

    // Initialize debug text
    debugText.setFont(font);
    debugText.setCharacterSize(18);
    debugText.setFillColor(sf::Color::Yellow);
    debugText.setPosition(10, 40);

    // Initialize pause text
    pauseText.setFont(font);
    pauseText.setCharacterSize(48);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setString("Paused\nP: Resume\nESC: Exit");
    pauseText.setPosition((window.getSize().x - pauseText.getGlobalBounds().width) / 2,
                         window.getSize().y / 2 - 50);

    // Initialize game view
    gameView.setSize(800, 600);
    gameView.setCenter(400, 300);

    // Load audio
    if (!backgroundMusic.openFromFile("TDCod/Assets/Audio/atmosphere.mp3")) {
        std::cerr << "Error loading background music! Path: TDCod/Assets/Audio/atmosphere.mp3" << std::endl;
    } else {
        backgroundMusic.setVolume(15);
    }

    if (!zombieBiteBuffer.loadFromFile("TDCod/Assets/Audio/zombiebite.mp3")) {
        std::cerr << "Error loading zombie bite sound!" << std::endl;
    } else {
        zombieBiteSound.setBuffer(zombieBiteBuffer);
    }

    if (!knifeSwingBuffer.loadFromFile("TDCod/Assets/Audio/knife_swing.mp3")) {
        std::cerr << "Error loading knife swing sound!" << std::endl;
    } else {
        knifeSwingSound.setBuffer(knifeSwingBuffer);
    }

    if (!batSwingBuffer.loadFromFile("TDCod/Assets/Audio/bat_swing.mp3")) {
        std::cerr << "Error loading bat swing sound!" << std::endl;
    } else {
        batSwingSound.setBuffer(batSwingBuffer);
    }

    // Initialize physics and level manager
    physics.addBody(&player.getBody(), false);
    levelManager.setPhysicsWorld(&physics);
    levelManager.initialize();
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processInput();
        if (!isPaused) {
            update(deltaTime);
        }
        render();
    }
}

void Game::processInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                if (isPaused) {
                    window.close();
                } else if (levelManager.getCurrentState() != GameState::GAME_OVER && levelManager.getCurrentState() != GameState::VICTORY) {
                    isPaused = true;
                } else {
                    window.close();
                }
            }
            else if (event.key.code == sf::Keyboard::P && levelManager.getCurrentState() != GameState::GAME_OVER && levelManager.getCurrentState() != GameState::VICTORY) {
                isPaused = !isPaused;
            }
            else if (event.key.code == sf::Keyboard::R && (levelManager.getCurrentState() == GameState::GAME_OVER || levelManager.getCurrentState() == GameState::VICTORY)) {
                reset();
            }
            else if (event.key.code == sf::Keyboard::F1) {
                showDebugInfo = !showDebugInfo;
            }
            else if (event.key.code == sf::Keyboard::Space && levelManager.getCurrentState() == GameState::TUTORIAL && levelManager.isPlayerNearDoctor(player) && !isPaused) {
                levelManager.advanceDialog();
            }
        }
        else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && !isPaused) {
            if (levelManager.getCurrentState() != GameState::GAME_OVER && levelManager.getCurrentState() != GameState::VICTORY) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                sf::Vector2f worldMousePosition = window.mapPixelToCoords(mousePosition, gameView);
                if (player.getCurrentWeapon() == WeaponType::PISTOL || player.getCurrentWeapon() == WeaponType::RIFLE || player.getCurrentWeapon() == WeaponType::FLAMETHROWER) {
                    if (player.timeSinceLastShot >= player.fireCooldown) {
                        player.shoot(worldMousePosition, physics);
                    }
                }
                else {
                    player.attack();
                    if (player.getCurrentWeapon() == WeaponType::KNIFE && knifeSwingSound.getStatus() != sf::Sound::Playing) {
                        knifeSwingSound.play();
                    }
                    else if (player.getCurrentWeapon() == WeaponType::BAT && batSwingSound.getStatus() != sf::Sound::Playing) {
                        batSwingSound.play();
                    }
                }
            }
        }
    }
}

void Game::update(float deltaTime) {
    if (levelManager.getCurrentState() == GameState::GAME_OVER || levelManager.getCurrentState() == GameState::VICTORY) {
        if (backgroundMusic.getStatus() == sf::Music::Playing) {
            backgroundMusic.stop();
        }
        return;
    }

    // Play background music
    if (backgroundMusic.getStatus() != sf::Music::Playing) {
        backgroundMusic.setLoop(true);
        backgroundMusic.play();
    }

    // Update player
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    sf::Vector2f worldMousePosition = window.mapPixelToCoords(mousePosition, gameView);
    int currentLevel = levelManager.getCurrentLevel();
    sf::Vector2u mapSize = getMapSize(currentLevel);
    player.update(deltaTime, window, mapSize, worldMousePosition);

    // Handle sprinting
    bool isSprinting = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
    player.sprint(isSprinting);

    // Update level manager and physics
    levelManager.update(deltaTime, player);
    physics.update(deltaTime);

    // Check collisions
    checkZombiePlayerCollisions();
    checkPlayerBoundaries();

    // Update camera
    sf::Vector2f playerPosition = player.getPosition();
    gameView.setCenter(playerPosition);

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
    if (cameraRight > mapSize.x) {
        gameView.setCenter(mapSize.x - gameView.getSize().x / 2, gameView.getCenter().y);
    }
    if (cameraBottom > mapSize.y) {
        gameView.setCenter(mapSize.x, mapSize.y - gameView.getSize().y / 2);
    }

    // Handle level transitions
    if (levelManager.isLevelTransitionPending() || (zombiesToNextLevel <= 0 && points >= pointsToNextLevel)) {
        levelManager.clearLevelTransitionPending();
        levelManager.nextLevel();
        zombiesToNextLevel = 15;
        pointsToNextLevel += 50; // Increase points requirement for next level
    }
}

void Game::render() {
    window.clear();
    window.setView(gameView);

    // Draw map and entities
    if (!levelManager.isLevelTransitioning()) {
        int currentLevel = levelManager.getCurrentLevel();
        window.draw(getMapSprite(currentLevel));
        levelManager.draw(window);
        player.draw(window);
    }

    // Draw UI
    window.setView(window.getDefaultView());
    pointsText.setString("Points: " + std::to_string(points));
    window.draw(pointsText);
    levelManager.renderUI(window, font);
    levelManager.drawHUD(window, player);

    // Draw debug info
    if (showDebugInfo) {
        debugText.setString(
            "Health: " + std::to_string(static_cast<int>(player.getHealth())) +
            "\nZombies: " + std::to_string(levelManager.getZombies().size()) +
            "\nLevel: " + std::to_string(levelManager.getCurrentLevel()) +
            "\nPoints to Next: " + std::to_string(pointsToNextLevel - points)
        );
        window.draw(debugText);
    }

    // Draw pause menu
    if (isPaused && levelManager.getCurrentState() != GameState::GAME_OVER && levelManager.getCurrentState() != GameState::VICTORY) {
        window.draw(pauseText);
    }

    // Draw game over or victory screen
    if (levelManager.getCurrentState() == GameState::GAME_OVER) {
        sf::Text gameOverText;
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(48);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setString("Game Over");
        gameOverText.setPosition((window.getSize().x - gameOverText.getGlobalBounds().width) / 2,
                                 window.getSize().y / 2 - 50);
        window.draw(gameOverText);

        sf::Text restartText;
        restartText.setFont(font);
        restartText.setCharacterSize(24);
        restartText.setFillColor(sf::Color::White);
        restartText.setString("R: Restart\nESC: Exit");
        restartText.setPosition((window.getSize().x - restartText.getGlobalBounds().width) / 2,
                                window.getSize().y / 2 + 50);
        window.draw(restartText);
    }
    else if (levelManager.getCurrentState() == GameState::VICTORY) {
        sf::Text victoryText;
        victoryText.setFont(font);
        victoryText.setCharacterSize(48);
        victoryText.setFillColor(sf::Color::Green);
        victoryText.setString("Victory!");
        victoryText.setPosition((window.getSize().x - victoryText.getGlobalBounds().width) / 2,
                                window.getSize().y / 2 - 50);
        window.draw(victoryText);

        sf::Text restartText;
        restartText.setFont(font);
        restartText.setCharacterSize(24);
        restartText.setFillColor(sf::Color::White);
        restartText.setString("R: Restart\nESC: Exit");
        restartText.setPosition((window.getSize().x - restartText.getGlobalBounds().width) / 2,
                                window.getSize().y / 2 + 50);
        window.draw(restartText);
    }

    window.display();
}

sf::Sprite& Game::getMapSprite(int level) {
    switch (level) {
        case 0:
        case 1:
        case 4:
        case 5:
            return mapSprite1;
        case 2:
            return mapSprite2;
        case 3:
            return mapSprite3;
        default:
            return mapSprite1;
    }
}

sf::Vector2u Game::getMapSize(int level) {
    switch (level) {
        case 0:
        case 1:
        case 4:
        case 5:
            return sf::Vector2u(1000, 1250);
        case 2:
            return sf::Vector2u(1200, 1000);
        case 3:
            return sf::Vector2u(1200, 800);
        default:
            return sf::Vector2u(1000, 1250);
    }
}

void Game::checkZombiePlayerCollisions() {
    std::vector<std::unique_ptr<BaseZombie>>& zombies = levelManager.getZombies();

    for (auto it = zombies.begin(); it != zombies.end(); ) {
        if (!(*it)->isAlive()) {
            points += 10;
            zombiesToNextLevel--;
            it = zombies.erase(it);
            continue;
        }

        sf::FloatRect playerHitbox = player.getHitbox();
        sf::FloatRect zombieHitbox = (*it)->getHitbox();

        bool pzCollision = isCollision(playerHitbox, zombieHitbox);
        if (pzCollision && (*it)->isAttacking()) {
            float damageToApply = (*it)->tryDealDamage();
            if (damageToApply > 0) {
                player.takeDamage(damageToApply);
                if (zombieBiteSound.getStatus() != sf::Sound::Playing) {
                    zombieBiteSound.play();
                }

                sf::Vector2f direction = player.getPosition() - (*it)->getPosition();
                float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);
                if (magnitude > 0) {
                    direction.x /= magnitude;
                    direction.y /= magnitude;
                    player.applyKnockback(direction, 50.0f);
                }
            }
        }

        if (player.isAttacking() && (player.getCurrentWeapon() == WeaponType::KNIFE || player.getCurrentWeapon() == WeaponType::BAT)) {
            sf::FloatRect playerAttackBox = player.getAttackHitbox();
            bool paZCollision = isCollision(playerAttackBox, zombieHitbox);
            if (paZCollision) {
                (*it)->takeDamage(player.getAttackDamage());
                if (!(*it)->isAlive()) {
                    points += 10;
                    zombiesToNextLevel--;
                }
            }
        }

        ++it;
    }
}

void Game::checkPlayerBoundaries() {
    sf::Vector2f playerPos = player.getPosition();
    int currentLevel = levelManager.getCurrentLevel();
    sf::Vector2u mapSize = getMapSize(currentLevel);
    float playerRadius = 20.0f;

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
    pointsToNextLevel = 150;
    levelManager.reset();
    player.reset();
    gameView.setCenter(400, 300);
    isPaused = false;
    showDebugInfo = false;
    backgroundMusic.stop();
}

sf::RenderWindow& Game::getWindow() {
    return window;
}