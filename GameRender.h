#ifndef GAMERENDER_HPP_INCLUDED
#define GAMERENDER_HPP_INCLUDED

#include <SFML/Graphics.hpp>
#include <vector>

enum RenderLayer {
    GROUND = 0,
    RENDER,
    LAYER_COUNT
};

class GameRender {
public:
    static GameRender& get();

    void initialize(unsigned width, unsigned height);
    void setFocus(const sf::Vector2f& focus);
    void clear();
    void display(sf::RenderTarget* target);

    size_t registerTexture(const std::string& filename, bool repeated = false);
    void addQuad(size_t textureID, const sf::VertexArray& quad, RenderLayer layer);

    void initGround(size_t textureID, const sf::VertexArray& quad);
    void renderGround();

private:
    GameRender() = default;

    sf::RenderTexture _renderTexture;
    sf::RenderTexture _groundTexture;

    sf::Vector2f _focus;
    sf::Vector2f _baseOffset;
    sf::Vector2u _renderSize;
    float _zoom = 1.5f;
    float _quality = 1.0f;
    float _ratio = 1.0f;

    std::vector<sf::Texture> _textures;
    std::vector<sf::VertexArray> _vertices[LAYER_COUNT];

    size_t _groundTextureID = 0;
    sf::VertexArray _groundQuad;

    void _translateToFocus(sf::Transform& transform);
    void _renderVertices(std::vector<sf::VertexArray>& va, sf::RenderTexture& target, sf::RenderStates& states);
};

#endif // GAMERENDER_HPP_INCLUDED