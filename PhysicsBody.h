#pragma once
#include <SFML/Graphics.hpp>
#include "Vec2.h"

class Entity;

class PhysicsBody {
public:
    Vec2 position, size, velocity;
    bool isStatic;
    float mass;
    bool isCircle = false; // default is box

    Entity* owner = nullptr;

    PhysicsBody(Vec2 position, Vec2 size, bool isStatic, float mass, bool isCircle = false);

    void update(float dt);
    void applyDamping(float factor);
    sf::Shape& getShape(); // returns a reference to an internally stored shape
    const sf::Shape& getShape() const;


private:
    sf::CircleShape circleShape;
    sf::RectangleShape rectShape;
};
