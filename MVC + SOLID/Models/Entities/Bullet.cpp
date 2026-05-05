#include "Bullet.h"

void Bullet::activate(Rectangle origin, Vector2 dir, float speed) {
    rec_      = { origin.x + origin.width / 2.0f,
                  origin.y + origin.height / 2.0f, 20, 10 };
    velocity_ = { dir.x * speed, dir.y * speed };
    active_   = true;
}

void Bullet::deactivate() { active_ = false; }

void Bullet::update(float dt, int sw) {
    if (!active_) return;
    rec_.x += velocity_.x * dt;
    rec_.y += velocity_.y * dt;
    if (rec_.x > sw || rec_.x < -50 || rec_.y < -50 || rec_.y > 2000)
        deactivate();
}
