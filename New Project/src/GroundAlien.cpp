#include "GroundAlien.h"

GroundAlien::GroundAlien(Texture2D texture)
    : GameEntity(200.0f, 200.0f, 100.0f),
      texture_(texture),
      frameWidth_(texture.width / FRAME_COUNT)
{}

void GroundAlien::spawn(float x, float y, float speed) {
    rec_.x        = x;
    rec_.y        = GROUND_Y;
    health_       = maxHealth_;
    velocityX_    = speed;
    alive_        = true;
    currentFrame_ = 0;
    frameTimer_   = 0;
}

void GroundAlien::update(float dt) {
    if (!alive_) return;

    rec_.x -= velocityX_ * dt;

    // Animate sprite sheet.
    frameTimer_ += dt;
    if (frameTimer_ >= FRAME_TIME) {
        currentFrame_ = (currentFrame_ + 1) % FRAME_COUNT;
        frameTimer_   = 0;
    }

    if (rec_.x < -rec_.width || health_ <= 0) despawn();
}

void GroundAlien::draw() const {
    if (!alive_) return;
    Rectangle src{
        static_cast<float>(currentFrame_ * frameWidth_), 0,
        static_cast<float>(frameWidth_),
        static_cast<float>(texture_.height)
    };
    DrawTexturePro(texture_, src, rec_, { 0, 0 }, 0.0f, WHITE);
}
