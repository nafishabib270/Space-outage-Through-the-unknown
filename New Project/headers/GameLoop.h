#pragma once

#include "GameState.h"
#include "GameWorld.h"
#include "MenuScreen.h"
#include "ResourceManager.h"

/*
 * GameLoop.h
 * ----------
 * SRP: Owns exactly one thing — the top-level state machine that
 *      decides which screen is active and drives its update/draw
 *      cycle. No physics, no rendering logic, no asset loading live
 *      here.
 *
 * OCP: Adding a new screen (e.g. PauseScreen, SettingsScreen) means
 *      adding a new GameState value, a new screen member, and a new
 *      case in the dispatch — existing handlers are untouched.
 *
 * DIP: Holds ResourceManager, GameWorld, and MenuScreen by value.
 *      GameWorld receives ResourceManager& so it never owns assets.
 *      GameLoop is the composition root — it is the only place where
 *      concrete types are wired together.
 */

class GameLoop {
public:
    static constexpr int SCREEN_W  = 1800;
    static constexpr int SCREEN_H  = 900;
    static constexpr int TARGET_FPS = 120;

    GameLoop();
    ~GameLoop() = default;

    // No copy — owns window and audio device lifetime.
    GameLoop(const GameLoop&)            = delete;
    GameLoop& operator=(const GameLoop&) = delete;

    // Runs until the player exits or closes the window.
    void run();

private:
    ResourceManager resources_;
    GameWorld       world_;
    MenuScreen      menu_;
    GameState       state_{ GameState::Menu };

    void handleMenu();
    void handleGame();
};
