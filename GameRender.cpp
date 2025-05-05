#include "GameRender.h"
#include <iostream>

GameRender& GameRender::get() {
    static GameRender instance;
    return instance;
}

void GameRender::initialize(unsigned width, unsigned height) {
    _renderSize = sf::Vector2u(width, height);
    _ratio = _zoom / _quality;

    float bx = width / (_quality * _zoom) * 0.5f;
    float by = height / (_quality * _zoom) * 0.5f;
    _baseOffset = sf::Vector2f(bx, by);

    _renderTexture.create(width, height);
    _groundTexture.create(width, height);

    for (int i = 0; i < LAYER_COUNT; ++i)
        _vertices[i].clear();
}

void GameRender::setFocus(const sf::Vector2f& focus) {
    _focus = focus;
}

void GameRender::clear() {
    for (int i = 0; i < LAYER_COUNT; ++i)
        for (auto& va : _vertices[i])
            va.clear();

    _renderTexture.clear(sf::Color::Black);
    renderGround();
}

void GameRender::display(sf::RenderTarget* target) {
    _renderTexture.display();
    sf::Sprite sprite(_renderTexture.getTexture());
    sprite.setScale(_ratio, _ratio);
    target->draw(sprite);
}

size_t GameRender::registerTexture(const std::string& filename, bool repeated) {
    _textures.emplace_back();
    _textures.back().setRepeated(repeated);
    if (!_textures.back().loadFromFile(filename)) {
        std::cout << "Error loading texture: " << filename << std::endl;
    }

    for (int i = 0; i < LAYER_COUNT; ++i)
        _vertices[i].emplace_back(sf::Quads);

    return _textures.size() - 1;
}

void GameRender::addQuad(size_t textureID, const sf::VertexArray& quad, RenderLayer layer) {
    auto& va = _vertices[layer][textureID];
    for (int i = 0; i < 4; ++i)
        va.append(quad[i]);
}

void GameRender::initGround(size_t textureID, const sf::VertexArray& quad) {
    _groundTextureID = textureID;
    _groundQuad = quad;
    _groundTexture.clear(sf::Color::Black);
}

void GameRender::renderGround() {
    sf::RenderStates states;
    _renderVertices(_vertices[GROUND], _groundTexture, states);
    _groundTexture.display();

    sf::Sprite sprite(_groundTexture.getTexture());
    sprite.setScale(_quality, _quality);

    float tx = (_focus.x - _baseOffset.x) * _quality;
    float ty = (_focus.y - _baseOffset.y) * _quality;
    sprite.setPosition(-tx, -ty);

    _renderTexture.draw(sprite);
}

void GameRender::_translateToFocus(sf::Transform& transform) {
    transform.translate(_baseOffset.x - _focus.x, _baseOffset.y - _focus.y);
}

void GameRender::_renderVertices(std::vector<sf::VertexArray>& vList, sf::RenderTexture& target, sf::RenderStates& states) {
    for (size_t i = 0; i < vList.size(); ++i) {
        if (!vList[i].getVertexCount()) continue;
        states.texture = &_textures[i];
        target.draw(vList[i], states);
    }
}