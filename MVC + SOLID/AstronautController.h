#pragma once
#include "model/GameModel.h"
#include "controller/InputHandler.h"
#include "raylib.h"
#include <cmath>

// CONTROLLER — SRP: maps astronaut input to Model physics mutations only.
class AstronautController {
public:
    AstronautController(GameModel& model, Sound laserSound);
    void handleInput(const InputSnapshot& input, float dt);

private:
    GameModel& model_;
    Sound      laserSound_;
};
