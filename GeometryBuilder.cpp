#include "GeometryBuilder.h"

sf::VertexArray GeometryBuilder::createQuad(sf::Vector2f size, sf::Vector2f center)
{
    sf::VertexArray vertices(sf::Quads, 4);

    float offsetX = center.x;
    float offsetY = center.y;

    float hx = size.x * 0.5f;
    float hy = size.y * 0.5f;

    vertices[0].position = sf::Vector2f(-hx - offsetX, -hy - offsetY);
    vertices[1].position = sf::Vector2f(hx - offsetX, -hy - offsetY);
    vertices[2].position = sf::Vector2f(hx - offsetX, hy - offsetY);
    vertices[3].position = sf::Vector2f(-hx - offsetX, hy - offsetY);

    return vertices;
}

void GeometryBuilder::initQuad(sf::VertexArray& vertices, sf::Vector2f size, sf::Vector2f center, float scale)
{
    float offsetX = center.x * scale;
    float offsetY = center.y * scale;

    float hx = size.x * scale;
    float hy = size.y * scale;

    vertices[0].position = sf::Vector2f(-offsetX, -offsetY);
    vertices[1].position = sf::Vector2f(hx - offsetX, -offsetY);
    vertices[2].position = sf::Vector2f(hx - offsetX, hy - offsetY);
    vertices[3].position = sf::Vector2f(-offsetX, hy - offsetY);
}

void GeometryBuilder::transform(sf::VertexArray& vertices, sf::Vector2f t, float r)
{
    float cosa = cos(r);
    float sina = sin(r);

    size_t nVertices = vertices.getVertexCount();
    for (size_t i = nVertices; i--; )
    {
        sf::Vertex& v = vertices[i];
        float x = v.position.x;
        float y = v.position.y;

        v.position.x = x * cosa - y * sina + t.x;
        v.position.y = x * sina + y * cosa + t.y;
    }
}

void GeometryBuilder::move(sf::VertexArray& vertices, const sf::Vector2f& t)
{
    size_t nVertices = vertices.getVertexCount();
    for (size_t i = nVertices; i--; )
    {
        vertices[i].position.x += t.x;
        vertices[i].position.y += t.y;
    }
}
