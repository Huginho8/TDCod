#include "Zombie.h"

Zombie::Zombie(Vec2 position, float speed)
    : Entity(EntityType::Enemy, position, Vec2(32, 32), false, 3.0f, true), target(nullptr), speed(speed) {
    body.getShape().setFillColor(sf::Color::Green);
    getBody().isCircle = true;
}

void Zombie::setTarget(Entity* t) {
    target = t;
}

void Zombie::update(float dt) {
    if (target) {
        // Move towards the target (player)
        Vec2 direction = target->getBody().position - body.position;
        if (direction.length() > 0.01f) {
            direction.normalize();
            body.velocity = direction * speed;
        }
    }
    else {
        body.velocity = Vec2(0, 0); // Stop if no target is set
    }

    Entity::update(dt); // Update position and physics
}

void Zombie::onCollision(Entity* other) {
}
