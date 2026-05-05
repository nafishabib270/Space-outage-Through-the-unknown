#include "model/systems/CollisionSystem.h"

CollisionSystem::CollisionSystem(SessionStats& stats) : stats_(stats) {}

Rectangle CollisionSystem::shrink(Rectangle r, float m) {
    return { r.x + m, r.y + m, r.width - m * 2, r.height - m * 2 };
}

std::vector<HitEvent> CollisionSystem::testBulletsVsEntities(
    std::vector<Bullet>&      bullets,
    std::vector<ICollidable*> entities)
{
    std::vector<HitEvent> events;

    for (auto& bullet : bullets) {
        if (!bullet.isActive()) continue;
        for (auto* entity : entities) {
            if (!entity->isAlive()) continue;
            if (!CheckCollisionRecs(entity->getBounds(), bullet.getBounds())) continue;

            // Record hit event so View can draw the flash
            bool wasAlive = entity->isAlive();
            entity->takeDamage(static_cast<float>(stats_.getHitPower()));
            bullet.deactivate();

            bool killed = wasAlive && !entity->isAlive();
            if (killed) stats_.registerKill();

            events.push_back({ entity->getBounds(), killed });
            break;
        }
    }
    return events;
}

int CollisionSystem::testEntitiesVsPlayer(
    std::vector<ICollidable*> entities,
    Rectangle                 playerRect,
    int                       damagePerHit)
{
    if (IsKeyDown(KEY_L)) return 0; // god-mode debug key
    Rectangle ph = shrink(playerRect, 20.0f);
    int total = 0;
    for (auto* e : entities) {
        if (!e->isAlive()) continue;
        if (CheckCollisionRecs(shrink(e->getBounds(), 20.0f), ph)) {
            total += damagePerHit;
            e->kill();
        }
    }
    return total;
}
