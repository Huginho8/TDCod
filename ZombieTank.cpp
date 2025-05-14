#include "ZombieTank.h"
#include <iostream>

ZombieTank::ZombieTank(float x, float y)
    : BaseZombie(x, y, 80.0f, 12.0f, 30.0f, 45.0f, 3.0f) {
    loadTextures();
    
    // Set initial texture to walk
    if (!walkTextures.empty()) {
        sprite.setTexture(walkTextures[0]);
        sf::Vector2u textureSize = walkTextures[0].getSize();
        sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        sprite.setScale(0.45f, 0.45f); // Slightly larger than walker
        sprite.setPosition(position);
    }
}

void ZombieTank::loadTextures() {
    // Load walk animation textures
    for (int i = 0; i < 9; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieTank/Walk/walk_00";
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie tank walk texture: " << filePath << std::endl;
        }
        walkTextures.push_back(texture);
    }

    // Load attack animation textures
    for (int i = 0; i < 9; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieTank/Attack/attack_00";
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie tank attack texture: " << filePath << std::endl;
        }
        attackTextures.push_back(texture);
    }

    // Load death animation textures
    for (int i = 0; i < 6; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieTank/Death/death_00";
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie tank death texture: " << filePath << std::endl;
        }
        deathTextures.push_back(texture);
    }
}
