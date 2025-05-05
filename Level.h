#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics.hpp>
#include "Config.h"
#include "GameRender.h"
#include "Vec2.h" // Assuming Vec2 is defined in this header

class Level {
public:
    Level();
    Level(float width, float height);

    bool isInBounds(const Vec2& coord) const;
    void render();  // Render the level (background or ground)

private:
    sf::Vector2f _dimension;
    sf::VertexArray _vertexArray;  // For texture-based rendering
    size_t _groundTextureID;       // Texture ID for the ground (or background)
};

#endif // LEVEL_H
