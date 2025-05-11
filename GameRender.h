#pragma once
#include <SFML/Graphics.hpp>
#include "GameWorld.h"

class GameRender {
public:
    void render(sf::RenderWindow& window, const GameWorld& world);
    void draw(const Entity& entity, sf::RenderWindow& window); 
};
