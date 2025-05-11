#include "PhysicsBody.h"

PhysicsBody::PhysicsBody(Vec2 position, Vec2 size, bool isStatic, float mass, bool isCircle)
    : position(position), size(size), isStatic(isStatic), mass(mass), isCircle(isCircle) {
    // Initialize both shapes, one will be used depending on isCircle
    rectShape.setSize(sf::Vector2f(size.x, size.y));
    rectShape.setOrigin(size.x / 2, size.y / 2);
    rectShape.setFillColor(isStatic ? sf::Color::White : sf::Color::Red);

    float radius = std::max(size.x, size.y) / 2.f;
    circleShape.setRadius(radius);
    circleShape.setOrigin(radius, radius);
    circleShape.setFillColor(isStatic ? sf::Color::White : sf::Color::Blue);
}

void PhysicsBody::update(float dt) {
    if (!isStatic) {
        position += velocity * dt;
    }

    // Always update shape positions (even for static if moved manually)
    rectShape.setPosition(position.x, position.y);
    circleShape.setPosition(position.x, position.y);
}


void PhysicsBody::applyDamping(float factor) {
    if (!isStatic) {
        velocity.x *= (1.f - factor);
        velocity.y *= (1.f - factor);
    }
}

sf::Shape& PhysicsBody::getShape() {
    return isCircle ? static_cast<sf::Shape&>(circleShape)
        : static_cast<sf::Shape&>(rectShape);
}

const sf::Shape& PhysicsBody::getShape() const {
    return isCircle ? static_cast<const sf::Shape&>(circleShape)
        : static_cast<const sf::Shape&>(rectShape);
}
