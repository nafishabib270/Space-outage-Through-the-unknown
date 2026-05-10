#pragma once
#include "GameModel.h"
#include "InputHandler.h"
#include "raylib.h"
#include <cmath>

class ResourceManager;

// CONTROLLER — SRP: maps astronaut input to Model physics mutations only.
class AstronautController {
public:
    AstronautController(GameModel& model, ResourceManager& resources);
    void handleInput(const InputSnapshot& input, float dt);

private:
    GameModel&       model_;
    ResourceManager& resources_;
};
