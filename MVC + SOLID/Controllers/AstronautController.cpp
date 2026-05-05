#include "AstronautController.h"

AstronautController::AstronautController(GameModel& model, Sound laser)
    : model_(model), laserSound_(laser) {}

void AstronautController::handleInput(const InputSnapshot& in, float dt) {
    auto& physics = model_.getPhysics();
    auto& pos     = model_.getAstronautPos();

    if (in.left)  physics.moveAstroLeft (pos, dt);
    if (in.right) physics.moveAstroRight(pos, dt, GameModel::SCREEN_W);
    if (in.jump)  physics.startJump();

    // Mouse-aimed firing
    if (in.fire) {
        auto& bp = model_.getBulletPool();
        bp.tickCooldown(dt);
        if (bp.canFire()) {
            Vector2 centre{ pos.x + pos.width/2.0f, pos.y + pos.height/2.0f };
            Vector2 dir{ in.mousePos.x - centre.x, in.mousePos.y - centre.y };
            float len = std::sqrtf(dir.x*dir.x + dir.y*dir.y);
            if (len != 0) { dir.x /= len; dir.y /= len; }
            Rectangle origin{ pos.x - 40, pos.y - 40, pos.width, pos.width };
            bp.fire(origin, dir, 900.0f);
            bp.resetCooldown();
            PlaySound(laserSound_);
        }
    } else {
        model_.getBulletPool().tickCooldown(dt);
    }
}
