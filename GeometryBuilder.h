#ifndef GEOMETRYBUILDER_H
#define GEOMETRYBUILDER_H

#include <SFML/Graphics.hpp>
#include "WorldEntity.h"  // Assuming this is where WorldEntity is declared
#include "Config.h"       // For access to constants like CELL_SIZE, etc.

namespace GeometryBuilder {

    // Creates a quad (rectangle) centered at 'center' with the given 'size'
    sf::VertexArray createQuad(sf::Vector2f size, sf::Vector2f center);

    // Initializes a quad (rectangle) with a size, center, and scale
    void initQuad(sf::VertexArray& vertices, sf::Vector2f size, sf::Vector2f center, float scale);

    // Transforms a quad (by rotating and translating)
    void transform(sf::VertexArray& vertices, sf::Vector2f t, float r);

    // Moves a quad by a given translation vector
    void move(sf::VertexArray& vertices, const sf::Vector2f& t);

}

#endif // GEOMETRYBUILDER_H
