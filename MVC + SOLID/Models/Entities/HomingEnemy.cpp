#include "HomingEnemy.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>

HomingEnemy::HomingEnemy() : width_(0), height_(0) {}

void HomingEnemy::spawn(float x, float y, float speed) {
    rec_            = { x, y, 120.0f, 80.0f };
    health_         = maxHealth_;
    velocityX_      = -speed;  // negative to move left
    velocityY_      = 0;
    tracking_       = false;
    // Replicate original: lock-on range is 1000 + rand()%300 (horizontal, not euclidean)
    detectionRange_ = 1000.0f + static_cast<float>(rand() % 301);
    alive_          = true;
}

void HomingEnemy::takeDamage(float amount) {
    health_ -= amount;
    float originalSpeed = sqrt(velocityX_*velocityX_ + velocityY_*velocityY_);
    float newSpeed = std::max(0.0f, originalSpeed - 1.0f);
    if (newSpeed > 0 && originalSpeed > 0) {
        velocityX_ *= (newSpeed / originalSpeed);
        velocityY_ *= (newSpeed / originalSpeed);
    } else {
        velocityX_ = 0;
        velocityY_ = 0;
    }
    if (health_ <= 0) kill();
}

void HomingEnemy::kill() {
    alive_  = false;
    health_ = maxHealth_;
}

void HomingEnemy::updateTracking(Rectangle target) {
    if (tracking_) return;

    // Horizontal gap: enemy front edge to target back edge
    const float dif = rec_.x - (target.x + target.width);
    if (dif <= detectionRange_) {
        tracking_ = true;

        const float enemyCenterY  = rec_.y + rec_.height * 0.5f;
        const float targetCenterY = target.y + target.height * 0.5f;
        const float dis           = targetCenterY - enemyCenterY;

        const float time = (velocityX_ < 0.0f) ? (dif / -velocityX_) : 0.0f;
        if (time != 0.0f) {
            velocityY_ = dis / time;
            float speed = std::sqrt(velocityX_ * velocityX_ + velocityY_ * velocityY_);
            constexpr float lockOnBoost = 1.35f;
            velocityX_ *= lockOnBoost;
            velocityY_ *= lockOnBoost;
        } else {
            velocityY_ = 0.0f;
        }
    }
}

void HomingEnemy::clampPosition() {
    rec_.y = std::clamp(rec_.y, 0.0f, 720.0f);
}

void HomingEnemy::update(float dt, Rectangle target) {
    if (!alive_) return;

    updateTracking(target);
    rec_.x += velocityX_ * dt;
    rec_.y += velocityY_ * dt;
    clampPosition();

    if (rec_.x + rec_.width < 0) despawn();
}