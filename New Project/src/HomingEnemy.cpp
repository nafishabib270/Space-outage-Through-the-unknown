#include "HomingEnemy.h"
#include <cstdlib>

HomingEnemy::HomingEnemy(Texture2D texture)
    : GameEntity(150.0f,
                 static_cast<float>(texture.width)  / 5.0f,
                 static_cast<float>(texture.height) / 5.0f),
      texture_(texture)
{}

void HomingEnemy::spawn(float x, float y, float speed) {
    rec_.x       = x;
    rec_.y       = y;
    health_      = maxHealth_;
    velocityX_   = speed;
    velocityY_   = 0;
    targetLocked_ = false;
    alive_        = true;
}

void HomingEnemy::setTarget(Rectangle target) {
    target_ = target;
}

void HomingEnemy::computeIntercept() {
    float dx = rec_.x - (target_.x + target_.width);
    if (dx <= 1000.0f + static_cast<float>(GetRandomValue(0, 300))) {
        targetLocked_ = true;
        float time = (velocityX_ != 0) ? dx / velocityX_ : 0;
        float dy   = rec_.y - (target_.y + target_.height / 2.0f);
        velocityY_ = (time != 0) ? dy / time : 0;
    }
}

void HomingEnemy::update(float dt) {
    if (!alive_) return;

    rec_.x -= velocityX_ * dt;
    rec_.y -= velocityY_ * dt;

    if (!targetLocked_) computeIntercept();

    // Despawn when off-screen.
    if (rec_.x + rec_.width < 0) despawn();
}

void HomingEnemy::draw() const {
    if (!alive_) return;
    Rectangle src{ 0, 0,
        static_cast<float>(texture_.width),
        static_cast<float>(texture_.height) };
    DrawTexturePro(texture_, src, rec_, { 0, 0 }, 0.0f, WHITE);
}
