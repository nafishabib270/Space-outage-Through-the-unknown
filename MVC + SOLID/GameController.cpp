#include "controller/GameController.h"
#include <cstdlib>
#include <ctime>

GameController::GameController()
    : renderer_(model_, resources_, SCREEN_W, SCREEN_H)
    , menuView_(SCREEN_W, SCREEN_H)
    , spaceshipCtrl_(model_, resources_.getLaser())
    , astronautCtrl_(model_, resources_.getLaser())
{
    InitWindow(SCREEN_W, SCREEN_H, "GeoWorld");
    InitAudioDevice();
    SetTargetFPS(TARGET_FPS);

    srand(static_cast<unsigned>(time(nullptr))); // seed ONCE

    resources_.load();
    PlayMusicStream(resources_.getBGM());

    model_.addObserver(&renderer_);
}

void GameController::playSoundForHits(const std::vector<HitEvent>& events) {
    auto& sounds = resources_.getHitSounds();
    for (const auto& ev : events) {
        PlaySound(sounds[hitSoundIdx_]);
        hitSoundIdx_ = (hitSoundIdx_ + 1) % static_cast<int>(sounds.size());
    }
}

void GameController::handleMenu() {
    BeginDrawing();
    menuView_.render(model_.getStats());
    GameState next = menuView_.handleInput();
    EndDrawing();

    if (next == GameState::Playing) {
        model_.reset();
        renderer_.resetThrusters();
        hitSoundIdx_ = 0;
        stateMachine_.transitionTo(GameState::Playing);
    } else if (next == GameState::Exit) {
        stateMachine_.transitionTo(GameState::Exit);
    }
}

void GameController::handleGame() {
    float dt = GetFrameTime();
    UpdateMusicStream(resources_.getBGM());

    InputSnapshot input = inputHandler_.poll();

    // Controller dispatches to the right sub-controller
    if (model_.isOnSpaceship() && !model_.isCrashing()) {
        auto hints = spaceshipCtrl_.handleInput(input, dt);
        renderer_.updateThrusters(hints.up, hints.down, hints.moving);
    } else if (!model_.isOnSpaceship()) {
        astronautCtrl_.handleInput(input, dt);
        renderer_.updateThrusters(false, false, false);
    }

    model_.update(dt);

    // Play hit sounds — audio is a Controller concern (side-effect of Model events)
    playSoundForHits(model_.getLastHitEvents());

    BeginDrawing();
    renderer_.renderGame();
    EndDrawing();

    if (model_.isGameOver())
        stateMachine_.transitionTo(GameState::Menu);
}

void GameController::run() {
    while (!WindowShouldClose() && !stateMachine_.is(GameState::Exit)) {
        switch (stateMachine_.getState()) {
            case GameState::Menu:    handleMenu(); break;
            case GameState::Playing: handleGame(); break;
            default: break;
        }
    }
    resources_.unload();
    CloseAudioDevice();
    CloseWindow();
}
