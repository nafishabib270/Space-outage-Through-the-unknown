#pragma once
#include "raylib.h"
#include "SessionStats.h"
#include "GameState.h"

// VIEW — SRP: menu rendering and button click detection only.
// Returns GameState so Controller can act on the result.
class MenuView {
public:
    MenuView(int screenW, int screenH);
    void      render(const SessionStats& stats) const;
    GameState handleInput() const;   // reads mouse, returns transition or Menu
private:
    int screenW_, screenH_;
    Rectangle playBtn_{ 700, 400, 400, 80 };
    Rectangle exitBtn_{ 700, 500, 400, 80 };
    bool drawButton(Rectangle b, const char* text) const;
};
