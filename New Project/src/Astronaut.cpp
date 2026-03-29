#include "Astronaut.h"
#include <cmath>

Astronaut::Astronaut(Texture2D texture, BulletPool& bulletPool, int screenW)
    : texture_(texture)
    , bulletPool_(bulletPool)
    , screenW_(screenW)
    , frameWidth_(texture.width / static_cast<float>(FRAME_COUNT))
{
    srcRect_ = { 0, 0, frameWidth_, static_cast<float>(texture.height) };
}

void Astronaut::reset() {
    rec_        = { 300, GROUND_Y, 165, 155 };
    srcRect_    = { 0, 0, frameWidth_, static_cast<float>(texture_.height) };
    velocityY_  = 0;
    onAir_      = false;
    frameTimer_ = 0;
}

void Astronaut::handleMovement(float dt) {
    if (IsKeyDown(KEY_A)) rec_.x -= WALK_SPEED * dt;
    if (IsKeyDown(KEY_D)) rec_.x += WALK_SPEED * dt;

    // Clamp to screen edges.
    if (rec_.x < 0)                      rec_.x = 0;
    if (rec_.x + rec_.width > screenW_)  rec_.x = screenW_ - rec_.width;
}

void Astronaut::handleJump(float dt) {
    if (IsKeyPressed(KEY_W) && !onAir_) {
        onAir_    = true;
        velocityY_ = -JUMP_SPEED;
    }
    velocityY_ += GRAVITY * dt;
    rec_.y     += velocityY_ * dt;

    if (rec_.y >= GROUND_Y) {
        rec_.y    = GROUND_Y;
        velocityY_ = 0;
        onAir_    = false;
    }
}

void Astronaut::handleShooting(float dt) {
    if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) return;

    bulletPool_.tickCooldown(dt);
    if (!bulletPool_.canFire()) return;

    Vector2 mouse  = GetMousePosition();
    Vector2 centre = { rec_.x + rec_.width  / 2.0f,
                       rec_.y + rec_.height / 2.0f };
    Vector2 dir    = { mouse.x - centre.x, mouse.y - centre.y };
    float   len    = std::sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (len != 0.0f) { dir.x /= len; dir.y /= len; }

    Rectangle fireOrigin{ rec_.x - 40, rec_.y - 40, rec_.width, rec_.width };
    bulletPool_.fire(fireOrigin, dir, 900.0f);
    bulletPool_.resetCooldown();
}

void Astronaut::animateSprite(float dt) {
    if (onAir_) return;
    frameTimer_ += dt;
    if (frameTimer_ >= FRAME_TIME) {
        srcRect_.x += frameWidth_;
        if (srcRect_.x >= texture_.width) srcRect_.x = 0;
        frameTimer_ = 0;
    }
}

void Astronaut::update(float dt) {
    handleMovement(dt);
    handleJump(dt);
    handleShooting(dt);
    animateSprite(dt);
}

void Astronaut::draw() const {
    DrawTexturePro(texture_, srcRect_, rec_, { 0, 0 }, 0.0f, WHITE);
}
