#include "Bullet.h"

Bullet::Bullet(Vec2 position, Vec2 velocity, float mass, int maxPenetrations)
    : Entity(EntityType::Bullet, position, Vec2(8.f, 8.f), false, mass, true),
    remainingPenetrations(maxPenetrations)
{
    getBody().isCircle = true;
    body.isTrigger = true;
    body.velocity = velocity;
    body.getCircleShape().setRadius(4.f);
    body.getCircleShape().setOrigin(4.f, 4.f);
    body.getCircleShape().setFillColor(sf::Color::Yellow);
}

void Bullet::update(float dt) {
    Entity::update(dt); // Add physics updates if needed
}

void Bullet::onCollision(Entity* other) {
    if (!other || other == this || !other->isAlive()) return;

    EntityType targetType = other->getType();

    if (targetType == EntityType::Enemy) {
        // Avoid multiple hits on the same entity
        if (hitEntities.count(other)) return;

        hitEntities.insert(other);

        // Calculate the mass ratio between the bullet and the enemy
        float bulletMass = body.mass;
        float enemyMass = other->getBody().mass;

        // Apply pushback scaled by mass ratio
        float pushbackFactor = bulletMass / (bulletMass + enemyMass);  // Small pushback for heavy enemies, bigger for light enemies
        other->getBody().externalImpulse += body.velocity * 0.5f * pushbackFactor;

        remainingPenetrations--;
    }
    else if (targetType == EntityType::Wall) {
        remainingPenetrations = 0; // Wall stops bullet
    }

    if (remainingPenetrations <= 0) {
        destroy();
    }
}
