#include "HomingEnemy.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>

HomingEnemy::HomingEnemy() : width_(0), height_(0) {}

void HomingEnemy::spawn(float x, float y, float speed) {
    rec_           = { x, y, 120.0f, 80.0f };
    health_        = maxHealth_;
    velocityX_     = speed;
    velocityY_     = 0;
    tracking_      = false;
    detectionRange_ = 500.0f + static_cast<float>(rand() % 301); // 500-800 px
    chaseSpeedY_   = 120.0f + static_cast<float>(rand() % 81);  // 120-200 px/sec
    alive_         = true;
}

void HomingEnemy::takeDamage(float amount) {
    health_ -= amount;
    velocityX_ = std::max(0.0f, velocityX_ - 1.0f);
    if (health_ <= 0) kill();
}

void HomingEnemy::kill() {
    alive_   = false;
    health_  = maxHealth_;
}

void HomingEnemy::updateTracking(Rectangle target) {
    const float targetCenterX = target.x + target.width * 0.5f;
    const float targetCenterY = target.y + target.height * 0.5f;
    const float enemyCenterX  = rec_.x + rec_.width * 0.5f;
    const float enemyCenterY  = rec_.y + rec_.height * 0.5f;

    const float dx = enemyCenterX - targetCenterX;
    const float dy = targetCenterY - enemyCenterY;
    const float distance = std::sqrtf(dx * dx + dy * dy);

    if (!tracking_ && distance <= detectionRange_) {
        tracking_ = true;
    }

    if (tracking_) {
        const float directionY = (distance > 0.0f) ? (dy / distance) : 0.0f;
        velocityY_ = directionY * chaseSpeedY_;
    } else {
        velocityY_ = 0;
    }
}

void HomingEnemy::clampPosition() {
    rec_.y = std::clamp(rec_.y, 0.0f, 720.0f);
}

void HomingEnemy::update(float dt, Rectangle target) {
    if (!alive_) return;

    updateTracking(target);
    rec_.x -= velocityX_ * dt;
    rec_.y += velocityY_ * dt;
    clampPosition();

    if (rec_.x + rec_.width < 0) despawn();
}
