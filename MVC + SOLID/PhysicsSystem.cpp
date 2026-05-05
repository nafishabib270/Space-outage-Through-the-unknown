#include "model/systems/PhysicsSystem.h"
#include <algorithm>

void PhysicsSystem::reset() {
    speed_            = INIT_SPEED;
    downfallVelocity_ = 0;
    velocityY_        = 0;
    onAir_            = false;
}

void PhysicsSystem::moveUp(Rectangle& pos, float dt, int screenH) {
    if (pos.y > 0) { pos.y -= speed_ * dt; speed_ += ACCELERATION; }
}
void PhysicsSystem::moveDown(Rectangle& pos, float dt, int screenH) {
    if (pos.y < screenH - pos.height) { pos.y += speed_ * dt; speed_ += ACCELERATION; }
}
void PhysicsSystem::moveLeft(Rectangle& pos, float dt) {
    if (pos.x > 0) { pos.x -= speed_ * dt; speed_ += ACCELERATION; }
}
void PhysicsSystem::moveRight(Rectangle& pos, float dt, int screenW) {
    if (pos.x < screenW - pos.width) { pos.x += speed_ * dt; speed_ += ACCELERATION; }
}
void PhysicsSystem::applyFriction(float dt) {
    if (speed_ > 0) speed_ = std::max(0.0f, speed_ - FRICTION * dt);
}

bool PhysicsSystem::updateCrash(Rectangle& pos, float dt, int screenH) {
    downfallVelocity_ += CRASH_GRAVITY * dt;
    pos.y             += downfallVelocity_ * dt;
    return pos.y > screenH - 300;
}

void PhysicsSystem::startJump() {
    if (!onAir_) { onAir_ = true; velocityY_ = -JUMP_SPEED; }
}

void PhysicsSystem::updateAstronaut(Rectangle& pos, float dt) {
    velocityY_ += ASTRO_GRAVITY * dt;
    pos.y      += velocityY_ * dt;
    if (pos.y >= GROUND_Y) { pos.y = GROUND_Y; velocityY_ = 0; onAir_ = false; }
}

void PhysicsSystem::moveAstroLeft(Rectangle& pos, float dt) {
    pos.x = std::max(0.0f, pos.x - WALK_SPEED * dt);
}
void PhysicsSystem::moveAstroRight(Rectangle& pos, float dt, int screenW) {
    pos.x = std::min(static_cast<float>(screenW) - pos.width, pos.x + WALK_SPEED * dt);
}
