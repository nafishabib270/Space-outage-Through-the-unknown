#include "model/entities/GroundAlien.h"

void GroundAlien::spawn(float x, float speed) {
    rec_          = { x, GROUND_Y, 200.0f, 100.0f };
    health_       = maxHealth_;
    velocityX_    = speed;
    alive_        = true;
    currentFrame_ = 0;
    frameTimer_   = 0;
}

void GroundAlien::takeDamage(float amount) {
    health_ -= amount;
    if (health_ <= 0) kill();
}

void GroundAlien::kill() {
    alive_  = false;
    health_ = maxHealth_;
}

void GroundAlien::update(float dt) {
    if (!alive_) return;
    rec_.x -= velocityX_ * dt;

    frameTimer_ += dt;
    if (frameTimer_ >= FRAME_TIME) {
        currentFrame_ = (currentFrame_ + 1) % FRAME_COUNT;
        frameTimer_   = 0;
    }
    if (rec_.x + rec_.width < 0 || health_ <= 0) despawn();
}
