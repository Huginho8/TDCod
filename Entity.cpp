#include "Entity.h"

Entity::Entity(EntityType type, Vec2 position, Vec2 size, bool isStatic, float mass, bool isCircle)
    : type(type), body(position, size, isStatic, mass, isCircle)
{
    body.owner = this;
}

void Entity::update(float dt) {
    body.update(dt);
    body.getShape().setPosition(body.position.x, body.position.y);
}

void Entity::render(sf::RenderWindow& window) {
    window.draw(body.getShape());
}

void Entity::onCollision(Entity* other) {
    // Example: bullets destroy on hit, enemies react to bullets
    if (type == EntityType::Bullet && other->getType() == EntityType::Enemy) {
        other->getBody().velocity += body.velocity * 0.5f; // push enemy
        destroy(); // bullet vanishes
    }
}

PhysicsBody& Entity::getBody() {
    return body;
}

const PhysicsBody& Entity::getBody() const {
    return body;
}

EntityType Entity::getType() const {
    return type;
}

bool Entity::isAlive() const {
    return alive;
}

void Entity::destroy() {
    alive = false;
}
