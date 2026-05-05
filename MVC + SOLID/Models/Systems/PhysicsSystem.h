#pragma once
#include "raylib.h"

/*
 * PhysicsSystem — MODEL SYSTEM — SRP
 * -------------------------------------
 * Owns all numeric physics: spaceship WASD movement, crash fall,
 * astronaut gravity and jump. No rendering, no input reading.
 *
 * Controller calls these methods with the result of input checks.
 * Model stores the resulting positions. View reads them for rendering.
 */
class PhysicsSystem {
public:
    // Spaceship constants
    static constexpr float INIT_SPEED    = 200.0f;
    static constexpr float ACCELERATION  = 5.0f;
    static constexpr float FRICTION      = 10.0f;
    static constexpr float CRASH_GRAVITY = 1500.0f;

    // Astronaut constants
    static constexpr float JUMP_SPEED    = 700.0f;
    static constexpr float ASTRO_GRAVITY = 1500.0f;
    static constexpr float GROUND_Y      = 535.0f;
    static constexpr float WALK_SPEED    = 200.0f;
    static constexpr float SCROLL_SPEED  = 200.0f;

    void reset();

    // Spaceship physics — called by SpaceshipController
    void moveUp   (Rectangle& pos, float dt, int screenH);
    void moveDown (Rectangle& pos, float dt, int screenH);
    void moveLeft (Rectangle& pos, float dt);
    void moveRight(Rectangle& pos, float dt, int screenW);
    void applyFriction(float dt);
    void resetSpeed() { speed_ = INIT_SPEED; }
    float getSpeed() const { return speed_; }

    // Crash physics — returns true when ship hits ground
    bool updateCrash(Rectangle& pos, float dt, int screenH);

    // Astronaut physics — called by AstronautController
    void startJump();
    void updateAstronaut(Rectangle& pos, float dt);
    bool isOnAir() const { return onAir_; }
    void moveAstroLeft (Rectangle& pos, float dt);
    void moveAstroRight(Rectangle& pos, float dt, int screenW);

private:
    float speed_{ INIT_SPEED };
    float downfallVelocity_{ 0 };
    float velocityY_{ 0 };
    bool  onAir_{ false };
};
