#include "Game.h"
#include <iostream>
#include <algorithm>
#include "Zombie.h"
#include <random> // Include for random number generation
#include <cmath> // Include cmath for atan2
#include <vector> // Include for std::vector

Game::Game() : window(sf::VideoMode(1600, 900), "Echoes of Valkyrie"), points(0), zombiesToNextLevel(15) {
    window.setFramerateLimit(60);
    background.setFillColor(sf::Color::Blue);
    background.setSize(sf::Vector2f(2000, 2000)); // Set background size to map texture size

    // Load map texture
    if (!mapTexture.loadFromFile("Assets/TempMap.png")) {
        std::cerr << "Error loading map texture!" << std::endl;
    }
    mapSprite.setTexture(mapTexture);
    mapSprite.setOrigin(mapTexture.getSize().x / 2.0f, mapTexture.getSize().y / 2.0f);
    mapSprite.setScale(2.0f, 2.0f); // Scale the map

    // Initialize camera view
    cameraView.setSize(window.getSize().x, window.getSize().y);
    cameraView.setCenter(800, 450); // Default center

    // Font for displaying points
    // sf::Font font;
    // if (!font.loadFromFile("/System/Library/Fonts/Arial.ttf")) { // Replace with your font file
    //     std::cerr << "Error loading font!" << std::endl;
    // }
    // pointsText.setFont(font);
    // pointsText.setCharacterSize(24);
    // pointsText.setFillColor(sf::Color::White);
    // pointsText.setPosition(10, 10);

    // Spawn initial zombies
    spawnZombies(5);
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
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::C) { // Keep crouching logic
                 // Placeholder for crouching logic
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
    player.update(deltaTime, window, mapTexture.getSize()); // Pass window and mapSize

    // Center the view on the player
    sf::Vector2f playerPosition = player.getPosition();
    cameraView.setCenter(playerPosition);



    // Prevent player from going out of bounds - Map boundaries
    sf::FloatRect playerBounds = player.getSprite().getGlobalBounds();
    //if (playerBounds.left < 0) {
    //    player.setPosition(0, playerPosition.y);
    //}
    //if (playerBounds.left + playerBounds.width > mapSize.x) {
    //    player.setPosition(mapSize.x - playerBounds.width, playerPosition.y);
    //}
    //if (playerBounds.top < 0) {
    //    player.setPosition(playerPosition.x, 0);
    //}
    //if (playerBounds.top + playerBounds.height > mapSize.y) {
    //    player.setPosition(playerPosition.x, mapSize.y - playerBounds.height);
    //}

    // Update zombies
    for (auto& zombie : zombies) {
        zombie.update(deltaTime, player.getPosition());
    }

    // Placeholder for zombie collision and point increment
    // In a real game, this would be handled by collision detection
    // and zombie defeat logic.
    if (points >= zombiesToNextLevel) {
        std::cout << "Level Up!" << std::endl;
        zombiesToNextLevel += 10; // Increase zombies needed for next level
        points = 0;
    }

    // Update points text
    // pointsText.setString("Points: " + std::to_string(points));
}

void Game::render() {
    window.clear();
    window.setView(cameraView);

    window.draw(background);
    window.draw(mapSprite); // Draw the map
    player.draw(window);
    for (const auto& zombie : zombies) {
        zombie.draw(window);
    }
    // window.draw(pointsText); // Draw the points text
    window.display();
}

void Game::spawnZombies(int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distribX(0, 800); // Screen width
    std::uniform_real_distribution<> distribY(0, 600); // Screen height

    for (int i = 0; i < count; ++i) {
        float x = distribX(gen);
        float y = distribY(gen);
        zombies.emplace_back(x, y);
    }
}