#include "GameModel.h"
#include <algorithm>

GameModel::GameModel() { reset(); }

void GameModel::reset() {
    stats_.reset();
    physics_.reset();
    bulletPool_    = BulletPool{ 50, 0.1f };
    enemySpawner_.reset(10.0f, 1.0f);
    alienSpawner_.reset();
    spaceshipPos_ = { SCREEN_W * 0.2f, SCREEN_H * 0.5f, 120, 70 };
    astronautPos_ = { 300, 535, 165, 155 };
    bgPos1_       = { 0, 0, SCREEN_W, SCREEN_H };
    bgPos2_       = { SCREEN_W - 50.0f, 0, SCREEN_W, SCREEN_H };
    onSpaceship_       = true;
    phaseTransitioned_ = false;
    lastHitEvents_.clear();
}

void GameModel::scrollBackground(float dt) {
    bgPos1_.x -= PhysicsSystem::SCROLL_SPEED * dt;
    bgPos2_.x -= PhysicsSystem::SCROLL_SPEED * dt;
    if (bgPos2_.x <= 5.0f) {
        bgPos1_.x = 0;
        bgPos2_.x = SCREEN_W - 50.0f;
    }
}

std::vector<ICollidable*> GameModel::getAllCollidables() {
    auto v = enemySpawner_.getCollidables();
    auto a = alienSpawner_.getCollidables();
    v.insert(v.end(), a.begin(), a.end());
    return v;
}

void GameModel::updateSpaceshipPhase(float dt) {
    // Bullets & enemies
    bulletPool_.update(dt, SCREEN_W);
    enemySpawner_.update(dt, SCREEN_W, SCREEN_H, spaceshipPos_);

    auto enemies = enemySpawner_.getCollidables();

    // Collision — returns hit events for View to render
    lastHitEvents_ = collisionSystem_.testBulletsVsEntities(
        bulletPool_.getBullets(), enemies);

    int dmg = collisionSystem_.testEntitiesVsPlayer(
        enemies, spaceshipPos_, ENEMY_DAMAGE);
    if (dmg > 0) stats_.damageSpaceship(dmg);

    // Crash physics
    if (stats_.spaceshipDestroyed()) {
        bool landed = physics_.updateCrash(spaceshipPos_, dt, SCREEN_H);
        if (landed) onSpaceship_ = false;
    }
}

void GameModel::updateAstronautPhase(float dt) {
    if (!phaseTransitioned_) {
        enemySpawner_.setTimeLimit(2.0f);
        phaseTransitioned_ = true;
    }

    // Slide crashed ship off screen
    if (spaceshipPos_.x > 0)
        spaceshipPos_.x -= PhysicsSystem::SCROLL_SPEED * dt;

    physics_.updateAstronaut(astronautPos_, dt);
    bulletPool_.update(dt, SCREEN_W);

    Rectangle astroTarget{ astronautPos_.x, astronautPos_.y,
                            astronautPos_.width, astronautPos_.height };
    enemySpawner_.update(dt, SCREEN_W, SCREEN_H, astroTarget);
    alienSpawner_.update(dt, SCREEN_W);

    auto all = getAllCollidables();
    lastHitEvents_ = collisionSystem_.testBulletsVsEntities(
        bulletPool_.getBullets(), all);

    int dmg = collisionSystem_.testEntitiesVsPlayer(
        all, astronautPos_, ENEMY_DAMAGE);
    if (dmg > 0) stats_.damageAstronaut(dmg);
}

void GameModel::update(float dt) {
    scrollBackground(dt);
    if (onSpaceship_) updateSpaceshipPhase(dt);
    else              updateAstronautPhase(dt);
    notifyObservers();
}

void GameModel::notifyObservers() {
    for (auto* o : observers_) o->onModelChanged();
}

void GameModel::removeObserver(IObserver* o) {
    observers_.erase(std::remove(observers_.begin(), observers_.end(), o), observers_.end());
}
