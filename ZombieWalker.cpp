#include "ZombieWalker.h"
#include <iostream>

ZombieWalker::ZombieWalker(float x, float y)
    : BaseZombie(x, y, 30.0f, 5.0f, 50.0f, 40.0f, 2.0f) {
    loadTextures();
    
    // Set initial texture to walk
    if (!walkTextures.empty()) {
        sprite.setTexture(walkTextures[0]);
        sf::Vector2u textureSize = walkTextures[0].getSize();
        sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        sprite.setScale(0.4f, 0.4f);
        sprite.setPosition(position);
    }
}

void ZombieWalker::loadTextures() {
    // Load walk animation textures
    for (int i = 0; i < 9; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieWalker/Walk/walk_00";
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie walk texture: " << filePath << std::endl;
        }
        walkTextures.push_back(texture);
    }

    // Load attack animation textures
    for (int i = 0; i < 9; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieWalker/Attack/Attack_00";
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie attack texture: " << filePath << std::endl;
        }
        attackTextures.push_back(texture);
    }

    // Load death animation textures
    for (int i = 0; i < 6; ++i) {
        sf::Texture texture;
        std::string filePath = "TDCod/Assets/ZombieWalker/Death/Death_00";
        filePath += std::to_string(i) + ".png";
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading zombie death texture: " << filePath << std::endl;
        }
        deathTextures.push_back(texture);
    }
}
