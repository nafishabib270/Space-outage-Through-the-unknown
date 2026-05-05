#pragma once
#include "model/GameModel.h"
#include "controller/InputHandler.h"
#include "raylib.h"

// CONTROLLER — SRP: maps spaceship input to Model physics mutations only.
class SpaceshipController {
public:
    SpaceshipController(GameModel& model, Sound laserSound);
    // Returns {movingUp, movingDown, anyMoving} for View thruster update
    struct ThrusterHints { bool up, down, moving; };
    ThrusterHints handleInput(const InputSnapshot& input, float dt);

private:
    GameModel& model_;
    Sound      laserSound_;
};
