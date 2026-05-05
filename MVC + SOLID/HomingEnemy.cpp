#include "model/entities/HomingEnemy.h"
#include <cstdlib>

HomingEnemy::HomingEnemy() : width_(0), height_(0) {}

void HomingEnemy::spawn(float x, float y, float speed) {
    rec_         = { x, y, 120.0f, 80.0f };
    health_      = maxHealth_;
    velocityX_   = speed;
    velocityY_   = 0;
    targetLocked_= false;
    alive_       = true;
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

void HomingEnemy::computeIntercept(Rectangle target) {
    float dx = rec_.x - (target.x + target.width);
    // rand() is fine here — srand() is called once in main()
    if (dx <= 1000.0f + static_cast<float>(rand() % 300)) {
        targetLocked_ = true;
        float time = (velocityX_ != 0) ? dx / velocityX_ : 0;
        float dy   = rec_.y - (target.y + target.height / 2.0f);
        velocityY_ = (time != 0) ? dy / time : 0;
    }
}

void HomingEnemy::update(float dt, Rectangle target) {
    if (!alive_) return;
    rec_.x -= velocityX_ * dt;
    rec_.y -= velocityY_ * dt;
    if (!targetLocked_) computeIntercept(target);
    if (rec_.x + rec_.width < 0) despawn();
}
