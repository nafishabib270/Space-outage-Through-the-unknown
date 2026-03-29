#include "GameWorld.h"
#include <algorithm>

GameWorld::GameWorld(ResourceManager& resources)
    : resources_(resources)
    , bulletPool_(50, resources.getBullet())
    , spaceship_(resources.getSpaceship(), bulletPool_, SCREEN_W, SCREEN_H)
    , astronaut_(resources.getAstronaut(), bulletPool_, SCREEN_W)
    , collisionSystem_(stats_, resources.getHitEffect())
    , hud_(SCREEN_W, SCREEN_H)
{
    enemySpawner_ = std::make_unique<EnemySpawner>(
        resources.getEnemy(), 10, 10.0f, 1.0f);
    alienSpawner_ = std::make_unique<AlienSpawner>(
        resources.getAlien(), 20, static_cast<float>(SCROLL_SPEED));

    bgSrc_  = { 0, 0,
                static_cast<float>(resources.getBackground().width),
                static_cast<float>(resources.getBackground().height) };
    bgPos1_ = { 0, 0, SCREEN_W, SCREEN_H };
    bgPos2_ = { SCREEN_W - 50.0f, 0, SCREEN_W, SCREEN_H };
}

void GameWorld::reset() {
    stats_.reset();
    bulletPool_.update(0, SCREEN_W);   // Deactivate any lingering bullets.
    spaceship_.reset(SCREEN_W, SCREEN_H);
    astronaut_.reset();
    enemySpawner_->reset();
    alienSpawner_->reset();
    bgPos1_ = { 0, 0, SCREEN_W, SCREEN_H };
    bgPos2_ = { SCREEN_W - 50.0f, 0, SCREEN_W, SCREEN_H };
    onSpaceship_       = true;
    phaseTransitioned_ = false;
    hitSoundIdx_       = 0;
}

bool GameWorld::isGameOver() const {
    return stats_.isAstronautDead();
}

// ---- Background scrolling -----------------------------------------------

void GameWorld::scrollBackground(float dt) {
    bgPos1_.x -= SCROLL_SPEED * dt;
    bgPos2_.x -= SCROLL_SPEED * dt;
    if (bgPos2_.x <= 5.0f) {
        bgPos1_.x = 0;
        bgPos2_.x = SCREEN_W - 50.0f;
    }
    Texture2D bg = resources_.getBackground();
    DrawTexturePro(bg, bgSrc_, bgPos1_, { 0, 0 }, 0.0f, WHITE);
    DrawTexturePro(bg, bgSrc_, bgPos2_, { 0, 0 }, 0.0f, WHITE);
}

// ---- Collidable helpers -------------------------------------------------

std::vector<ICollidable*> GameWorld::getEnemyCollidables() {
    return enemySpawner_->getCollidables();
}

std::vector<ICollidable*> GameWorld::getAllCollidables() {
    auto v = enemySpawner_->getCollidables();
    auto a = alienSpawner_->getCollidables();
    v.insert(v.end(), a.begin(), a.end());
    return v;
}

// ---- Spaceship phase ----------------------------------------------------

void GameWorld::updateSpaceshipPhase(float dt) {
    if (!stats_.isSpaceshipDestroyed()) {
        spaceship_.update(dt);
    } else {
        spaceship_.startCrash();
        spaceship_.update(dt);
        if (spaceship_.hasCrashLanded()) {
            onSpaceship_ = false;
        }
    }

    enemySpawner_->setTarget(spaceship_.getBounds());
    enemySpawner_->update(dt, SCREEN_W, SCREEN_H);
    enemySpawner_->updateAll(dt);

    bulletPool_.update(dt, SCREEN_W);

    // Bullet vs enemy collisions.
    auto enemies = getEnemyCollidables();
    auto& hitSounds = resources_.getHitSounds();
    collisionSystem_.testBulletsVsEntities(
        bulletPool_.getBullets(), enemies, hitSounds, hitSoundIdx_);

    // Enemy vs spaceship contact.
    int damage = collisionSystem_.testEntitiesVsPlayer(
        enemies, spaceship_.getBounds(), ENEMY_DAMAGE);
    if (damage > 0) stats_.damageSpaceship(damage);
}

void GameWorld::drawSpaceshipPhase() {
    spaceship_.draw();
    bulletPool_.draw();
    enemySpawner_->drawAll();
}

// ---- Astronaut phase ----------------------------------------------------

void GameWorld::updateAstronautPhase(float dt) {
    // One-time: slow enemy spawn rate on phase entry.
    if (!phaseTransitioned_) {
        enemySpawner_->setTimeLimit(2.0f);
        phaseTransitioned_ = true;
    }

    // Slide crashed ship off-screen.
    // (Spaceship::update in crash mode handles this via its own x-drift.)

    astronaut_.update(dt);
    bulletPool_.update(dt, SCREEN_W);

    Rectangle astroRect = astronaut_.getBounds();
    enemySpawner_->setTarget(astroRect);
    enemySpawner_->update(dt, SCREEN_W, SCREEN_H);
    enemySpawner_->updateAll(dt);

    alienSpawner_->update(dt, SCREEN_W, SCREEN_H);
    alienSpawner_->updateAll(dt);

    // Bullets vs all entities.
    auto all = getAllCollidables();
    auto& hitSounds = resources_.getHitSounds();
    collisionSystem_.testBulletsVsEntities(
        bulletPool_.getBullets(), all, hitSounds, hitSoundIdx_);

    // Entities vs astronaut.
    int damage = collisionSystem_.testEntitiesVsPlayer(
        all, astroRect, ENEMY_DAMAGE);
    if (damage > 0) stats_.damageAstronaut(damage);
}

void GameWorld::drawAstronautPhase() {
    astronaut_.draw();
    bulletPool_.draw();
    enemySpawner_->drawAll();
    alienSpawner_->drawAll();
}

// ---- Public update / draw -----------------------------------------------

void GameWorld::update(float dt) {
    if (onSpaceship_) updateSpaceshipPhase(dt);
    else              updateAstronautPhase(dt);
}

void GameWorld::draw() {
    scrollBackground(GetFrameTime());

    if (onSpaceship_) drawSpaceshipPhase();
    else              drawAstronautPhase();

    hud_.render(stats_, onSpaceship_);
}
