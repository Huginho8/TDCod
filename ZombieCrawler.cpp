#include "ZombieCrawler.h"
#include <iostream>

ZombieCrawler::ZombieCrawler(float x, float y)
    : BaseZombie(x, y, 20.0f, 3.0f, 40.0f, 30.0f, 1.0f) {
    loadTextures();
    
    // Set initial texture to walk
    if (!walkTextures.empty()) {
        sprite.setTexture(walkTextures[0]);
        sf::Vector2u textureSize = walkTextures[0].getSize();
        sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        sprite.setScale(0.35f, 0.35f); // Smaller than walker
        sprite.setPosition(position);
    }
}

void ZombieCrawler::loadTextures() {
    // Load walk animation textures
    for (int i = 0; i < 9; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieCrawler/Walk/Walk_00";
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie crawler walk texture: " << filePath << std::endl;
        }
        walkTextures.push_back(texture);
    }

    // Load attack animation textures
    for (int i = 0; i < 9; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieCrawler/Attack/Attack1_00";
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie crawler attack texture: " << filePath << std::endl;
        }
        attackTextures.push_back(texture);
    }

    // Load death animation textures
    for (int i = 0; i < 10; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieCrawler/Death/Death_00";
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie crawler death texture: " << filePath << std::endl;
        }
        deathTextures.push_back(texture);
    }
}
