#pragma once

#include "raylib.h"
#include "GameState.h"
#include "SessionStats.h"

/*
 * MenuScreen.h
 * ------------
 * SRP: Owns only the main menu — title, play button, exit button,
 *      last-score display. No game logic runs inside this class.
 *
 * OCP: Adding a settings button or high-score panel means adding
 *      new members here — GameLoop is not modified.
 *
 * DIP: Receives const SessionStats& to display the last score.
 *      It does not know how the score was calculated.
 */

class MenuScreen {
public:
    MenuScreen(int screenW, int screenH);

    // Returns the state the game should transition to.
    GameState update(const SessionStats& stats);
    void      draw  (const SessionStats& stats) const;

private:
    int       screenW_;
    int       screenH_;
    Rectangle playBtn_{ 700, 400, 400, 80 };
    Rectangle exitBtn_{ 700, 500, 400, 80 };

    bool drawButton(Rectangle bounds, const char* text) const;
};
