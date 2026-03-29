#include "CollisionSystem.h"

CollisionSystem::CollisionSystem(SessionStats& stats, Texture2D hitEffect)
    : stats_(stats), hitEffect_(hitEffect)
{}

Rectangle CollisionSystem::shrink(Rectangle r, float margin) {
    return { r.x + margin, r.y + margin,
             r.width  - margin * 2.0f,
             r.height - margin * 2.0f };
}

void CollisionSystem::testBulletsVsEntities(
    std::vector<Bullet>&      bullets,
    std::vector<ICollidable*> entities,
    std::vector<Sound>&       hitSounds,
    int&                      soundIndex)
{
    Rectangle hitSrc{ 0, 0,
        static_cast<float>(hitEffect_.width),
        static_cast<float>(hitEffect_.height) };

    for (auto& bullet : bullets) {
        if (!bullet.isActive()) continue;

        for (auto* entity : entities) {
            if (!entity->isAlive()) continue;

            if (!CheckCollisionRecs(entity->getBounds(), bullet.getBounds()))
                continue;

            // Draw hit flash centred on the entity.
            Rectangle eb = entity->getBounds();
            Rectangle fx{ eb.x - eb.width / 5.0f, eb.y,
                          eb.width / 1.3f, eb.width / 1.3f };
            DrawTexturePro(hitEffect_, hitSrc, fx, { 0, 0 }, 0.0f, WHITE);

            // Round-robin sound pool to avoid audio clipping.
            PlaySound(hitSounds[soundIndex]);
            soundIndex = (soundIndex + 1) % static_cast<int>(hitSounds.size());

            entity->takeDamage(static_cast<float>(stats_.getHitPower()));
            bullet.deactivate();

            if (!entity->isAlive()) stats_.registerKill();

            break; // One bullet hits one entity.
        }
    }
}

int CollisionSystem::testEntitiesVsPlayer(
    std::vector<ICollidable*> entities,
    Rectangle                 playerRect,
    int                       damagePerHit)
{
    if (IsKeyDown(KEY_L)) return 0;  // God-mode debug key.

    Rectangle playerHitbox = shrink(playerRect, 20.0f);
    int       totalDamage  = 0;

    for (auto* entity : entities) {
        if (!entity->isAlive()) continue;
        if (CheckCollisionRecs(shrink(entity->getBounds(), 20.0f), playerHitbox)) {
            totalDamage += damagePerHit;
            entity->kill();
        }
    }
    return totalDamage;
}
