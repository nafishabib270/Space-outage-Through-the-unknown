#pragma once
#include "model/GameModel.h"
#include "view/GameRenderer.h"
#include "view/MenuView.h"
#include "core/GameStateMachine.h"
#include "core/ResourceManager.h"
#include "controller/InputHandler.h"
#include "controller/SpaceshipController.h"
#include "controller/AstronautController.h"

/*
 * GameController — CONTROLLER ROOT — MVC
 * -----------------------------------------
 * Owns the application loop. Mediates between Model and View:
 *   1. Polls InputHandler for raw input (no raylib calls elsewhere)
 *   2. Dispatches to SpaceshipController or AstronautController
 *   3. Calls model_.update() to advance game state
 *   4. Calls renderer_.renderGame() inside BeginDrawing/EndDrawing
 *   5. Manages GameStateMachine transitions
 *
 * SOLID:
 *   SRP : Orchestration only — no physics math, no rendering code.
 *   DIP : Depends on GameModel& and GameRenderer& abstractions.
 *   OCP : New states (Pause, GameOver) add a new handler method.
 */
class GameController {
public:
    static constexpr int SCREEN_W   = 1800;
    static constexpr int SCREEN_H   = 900;
    static constexpr int TARGET_FPS = 120;

    GameController();
    ~GameController() = default;

    GameController(const GameController&)            = delete;
    GameController& operator=(const GameController&) = delete;

    void run();

private:
    ResourceManager        resources_;
    GameModel              model_;
    GameRenderer           renderer_;
    MenuView               menuView_;
    GameStateMachine       stateMachine_;
    InputHandler           inputHandler_;
    SpaceshipController    spaceshipCtrl_;
    AstronautController    astronautCtrl_;

    void handleMenu();
    void handleGame();

    void playSoundForHits(const std::vector<HitEvent>& events);
    int  hitSoundIdx_{ 0 };
};
