#include "InputHandler.h"

InputSnapshot InputHandler::poll() const {
    InputSnapshot s{};
    s.up         = IsKeyDown(KEY_W);
    s.down       = IsKeyDown(KEY_S);
    s.left       = IsKeyDown(KEY_A);
    s.right      = IsKeyDown(KEY_D);
    s.jump       = IsKeyPressed(KEY_W);
    s.fire       = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    s.anyMovement= s.up || s.down || s.left || s.right;
    s.mousePos   = GetMousePosition();
    s.mouseDown  = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    return s;
}
