#include "GameLoop.h"

/*
 * GameLoop.cpp — Composition Root
 * --------------------------------
 * This is the only file in the project where concrete types are
 * constructed and wired together. Every other system receives its
 * dependencies through constructors (DIP).
 *
 * Construction order matters:
 *   1. ResourceManager — loaded after InitWindow/InitAudioDevice.
 *   2. GameWorld       — receives ResourceManager& (never owns assets).
 *   3. MenuScreen      — receives screen dimensions only.
 */

GameLoop::GameLoop()
    : world_(resources_)           // DIP: world depends on ResourceManager&
    , menu_(SCREEN_W, SCREEN_H)
{
    InitWindow(SCREEN_W, SCREEN_H, "GeoWorld");
    InitAudioDevice();
    SetTargetFPS(TARGET_FPS);

    resources_.load();
    PlayMusicStream(resources_.getBGM());
}

// ---- State handlers ------------------------------------------------------

void GameLoop::handleMenu() {
    BeginDrawing();
    menu_.draw(world_.getStats());

    // Input check — may transition to Playing or Exit.
    Vector2 mouse = GetMousePosition();
    Rectangle playBtn{ 700, 400, 400, 80 };
    Rectangle exitBtn{ 700, 500, 400, 80 };

    if (CheckCollisionPointRec(mouse, playBtn) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        world_.reset();
        state_ = GameState::Playing;
    }
    if (CheckCollisionPointRec(mouse, exitBtn) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        state_ = GameState::Exit;
    }

    EndDrawing();
}

void GameLoop::handleGame() {
    float dt = GetFrameTime();

    world_.update(dt);

    BeginDrawing();
    world_.draw();
    EndDrawing();

    // Transition back to menu when astronaut dies.
    if (world_.isGameOver()) {
        state_ = GameState::Menu;
    }
}

// ---- Main loop -----------------------------------------------------------

void GameLoop::run() {
    while (!WindowShouldClose() && state_ != GameState::Exit) {
        UpdateMusicStream(resources_.getBGM());

        switch (state_) {
            case GameState::Menu:    handleMenu(); break;
            case GameState::Playing: handleGame(); break;
            default:                              break;
        }
    }

    // Destructor of ResourceManager calls unload() automatically.
    CloseAudioDevice();
    CloseWindow();
}
