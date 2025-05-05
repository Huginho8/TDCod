#include "Level.h"
#include "GameRender.h"

Level::Level() :
    _dimension(0.0, 0.0),
    _groundTextureID(-1) // Initialize without a background texture
{
}

Level::Level(float width, float height) :
    _dimension(width, height),
    _vertexArray(sf::Quads, 4),
    _groundTextureID(-1) // Initialize without a background texture
{
    // Register a texture for the background (optional)
    _groundTextureID = GameRender::registerTexture("data/ground3.jpg", true);

    // Define vertices for rendering a quad (background or ground)
    size_t scaleDown = 4;
    _vertexArray[0].texCoords = sf::Vector2f(0, 0);
    _vertexArray[1].texCoords = sf::Vector2f(scaleDown * _dimension.x, 0);
    _vertexArray[2].texCoords = sf::Vector2f(scaleDown * _dimension.x, scaleDown * _dimension.y);
    _vertexArray[3].texCoords = sf::Vector2f(0, scaleDown * _dimension.y);

    _vertexArray[0].position = sf::Vector2f(0, 0);
    _vertexArray[1].position = sf::Vector2f(_dimension.x, 0);
    _vertexArray[2].position = sf::Vector2f(_dimension.x, _dimension.y);
    _vertexArray[3].position = sf::Vector2f(0, _dimension.y);

    // Initialize ground (or background) with texture or solid color
    GameRender::initGround(_groundTextureID, _vertexArray);
}

bool Level::isInBounds(const Vec2& coord) const
{
    return coord.x > 0 && coord.x < _dimension.x && coord.y > 0 && coord.y < _dimension.y;
}

void Level::render()
{
    GameRender& instance = *GameRender::getInstance();

    // Check if ground texture ID is valid (if not, render solid color as background)
    if (_groundTextureID == -1) {
        // Render a solid color background (green as placeholder)
        sf::RectangleShape background(sf::Vector2f(_dimension.x, _dimension.y));
        background.setFillColor(sf::Color(80, 120, 80));  // Green color as placeholder
        instance.getRenderTexture().draw(background);
    }
    else {
        // Render the ground texture (using texture-based rendering)
        GameRender::addQuad(_groundTextureID, _vertexArray, RenderLayer::GROUND);
    }
}
