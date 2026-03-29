#include "Spaceship.h"
#include <cmath>

Spaceship::Spaceship(Texture2D texture, BulletPool& bulletPool,
                     int screenW, int screenH)
    : texture_(texture)
    , bulletPool_(bulletPool)
    , screenW_(screenW)
    , screenH_(screenH)
{
    reset(screenW, screenH);
}

void Spaceship::reset(int screenW, int screenH) {
    rec_             = { screenW * 0.2f, screenH * 0.5f,
                         texture_.width  / 3.0f,
                         texture_.height / 3.0f };
    speed_           = INITIAL_SPEED;
    rotation_        = 0;
    downfallVelocity_ = 0;
    crashing_        = false;
    crashLanded_     = false;
    screenW_         = screenW;
    screenH_         = screenH;
    rearThrusters_.reset();
    sideThrusters_.reset();
}

void Spaceship::handleInput(float dt) {
    bool moving = false;

    if (IsKeyDown(KEY_W)) {
        if (IsKeyPressed(KEY_W)) { speed_ = INITIAL_SPEED; rotation_ = 0; }
        if (rec_.y > 0) { rec_.y -= speed_ * dt; speed_ += ACCELERATION; }
        sideThrusters_.emitSideUp(rec_);
        moving = true;
    }
    if (IsKeyDown(KEY_S)) {
        if (IsKeyPressed(KEY_S)) { speed_ = INITIAL_SPEED; rotation_ = 0; }
        if (rec_.y < screenH_ - rec_.height) { rec_.y += speed_ * dt; speed_ += ACCELERATION; }
        sideThrusters_.emitSideDown(rec_);
        moving = true;
    }
    if (IsKeyDown(KEY_D)) {
        if (IsKeyPressed(KEY_D)) speed_ = INITIAL_SPEED;
        if (rec_.x < screenW_ - rec_.width) { rec_.x += speed_ * dt; speed_ += ACCELERATION; }
        moving = true;
    }
    if (IsKeyDown(KEY_A)) {
        if (IsKeyPressed(KEY_A)) speed_ = INITIAL_SPEED;
        if (rec_.x > 0) { rec_.x -= speed_ * dt; speed_ += ACCELERATION; }
        moving = true;
    }

    if (!moving) {
        if (speed_ > 0) speed_ -= FRICTION * dt;
        sideThrusters_.coast();
    }

    // Shooting — fires horizontally to the right.
    bulletTimer_ += dt;
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && bulletPool_.canFire()) {
        bulletPool_.fire(rec_, { 1.0f, 0.0f }, 1200.0f);
        bulletPool_.resetCooldown();
        bulletTimer_ = 0;
    }
    bulletPool_.tickCooldown(dt);
}

void Spaceship::updateCrash(float dt) {
    downfallVelocity_ += GRAVITY * dt;
    rec_.y            += downfallVelocity_ * dt;
    if (rec_.y > screenH_ - 300.0f) crashLanded_ = true;
}

void Spaceship::update(float dt) {
    if (crashing_) updateCrash(dt);
    else           handleInput(dt);

    rearThrusters_.emitRear(rec_);
}

void Spaceship::draw() const {
    rearThrusters_.draw();
    sideThrusters_.draw();

    if (rec_.x > 0) {
        Rectangle src{ 0, 0,
            static_cast<float>(texture_.width),
            static_cast<float>(texture_.height) };
        DrawTexturePro(texture_, src, rec_, { 0, 0 }, rotation_, WHITE);
    }
}
