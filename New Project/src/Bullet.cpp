#include "Bullet.h"

Bullet::Bullet() = default;

void Bullet::activate(Rectangle origin, Vector2 direction, float speed) {
    rec_.x    = origin.x + origin.width  / 2.0f;
    rec_.y    = origin.y + origin.height / 2.0f;
    velocity_ = { direction.x * speed, direction.y * speed };
    active_   = true;
}

void Bullet::deactivate() {
    active_ = false;
}

void Bullet::update(float dt, int screenWidth) {
    if (!active_) return;
    rec_.x += velocity_.x * dt;
    rec_.y += velocity_.y * dt;
    if (rec_.x > screenWidth || rec_.x < -50 ||
        rec_.y < -50       || rec_.y > 2000)
        deactivate();
}

Rectangle Bullet::getBounds() const { return rec_; }
bool      Bullet::isAlive()   const { return active_; }

void Bullet::draw() const {
    if (!active_) return;
    Rectangle src{ 0, 0,
        static_cast<float>(texture_.width),
        static_cast<float>(texture_.height) };
    DrawTexturePro(texture_, src, rec_, { 0, 0 }, 0.0f, WHITE);
}
