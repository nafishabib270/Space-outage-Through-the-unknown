#include "SpaceshipController.h"
#include "ResourceManager.h"

SpaceshipController::SpaceshipController(GameModel& model, ResourceManager& resources)
    : model_(model), resources_(resources) {}

SpaceshipController::ThrusterHints
SpaceshipController::handleInput(const InputSnapshot& in, float dt) {
    auto& physics = model_.getPhysics();
    auto& pos     = model_.getSpaceshipPos();
    bool up = false, down = false;

    if (in.up)    { if (IsKeyPressed(KEY_W)) physics.resetSpeed(); physics.moveUp(pos, dt, GameModel::SCREEN_H); up = true; }
    if (in.down)  { if (IsKeyPressed(KEY_S)) physics.resetSpeed(); physics.moveDown(pos, dt, GameModel::SCREEN_H); down = true; }
    if (in.left)  { if (IsKeyPressed(KEY_A)) physics.resetSpeed(); physics.moveLeft(pos, dt); }
    if (in.right) { if (IsKeyPressed(KEY_D)) physics.resetSpeed(); physics.moveRight(pos, dt, GameModel::SCREEN_W); }
    if (!in.anyMovement) physics.applyFriction(dt);

    // Firing
    if (in.fire) {
        auto& bp = model_.getBulletPool();
        bp.tickCooldown(dt);
        if (bp.canFire()) {
            bp.fire(pos, { 1.0f, 0.0f }, 1200.0f);
            bp.resetCooldown();
            PlaySound(resources_.getLaserAlias());
        }
    } else {
        model_.getBulletPool().tickCooldown(dt);
    }

    return { up, down, in.anyMovement };
}
