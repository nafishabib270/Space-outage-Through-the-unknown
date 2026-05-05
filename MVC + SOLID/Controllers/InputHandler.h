#pragma once
#include "raylib.h"

/*
 * InputHandler — CONTROLLER — SRP
 * ----------------------------------
 * Polls raw raylib input and exposes clean booleans.
 * No game logic, no physics, no state mutation.
 * Controller classes use these booleans to decide what to tell the Model.
 */
struct InputSnapshot {
    bool up, down, left, right;
    bool jump;
    bool fire;
    bool anyMovement;
    Vector2 mousePos;
    bool mouseDown;
};

class InputHandler {
public:
    InputSnapshot poll() const;
};
