#include "Bullet.h"
#include "Zombie.h" // Added to include BaseZombie definition

Bullet::Bullet(Vec2 position, Vec2 velocity, float damage, float mass, int maxPenetrations)
    : Entity(EntityType::Bullet, position, Vec2(8.f, 8.f), false, mass, true),
    remainingPenetrations(maxPenetrations),
    damage(damage)
{
    getBody().isCircle = true;
    body.isTrigger = true;
    body.velocity = velocity;
    body.getCircleShape().setRadius(4.f);
    body.getCircleShape().setOrigin(4.f, 4.f);
    body.getCircleShape().setFillColor(sf::Color::Yellow);
}

void Bullet::update(float dt) {
    Entity::update(dt);
}

void Bullet::onCollision(Entity* other) {
    if (!other || other == this || !other->isAlive()) return;

    EntityType targetType = other->getType();

    if (targetType == EntityType::Enemy) {
        if (hitEntities.count(other)) return;

        hitEntities.insert(other);
        BaseZombie* zombie = dynamic_cast<BaseZombie*>(other);
        if (zombie) {
            zombie->takeDamage(damage);
        }

        float bulletMass = body.mass;
        float enemyMass = other->getBody().mass;
        float pushbackFactor = bulletMass / (bulletMass + enemyMass);
        other->getBody().externalImpulse += body.velocity * 0.5f * pushbackFactor;

        remainingPenetrations--;
    }
    else if (targetType == EntityType::Wall) {
        remainingPenetrations = 0;
    }

    if (remainingPenetrations <= 0) {
        destroy();
    }
}