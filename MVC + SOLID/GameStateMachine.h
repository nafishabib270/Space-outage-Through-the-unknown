#pragma once
#include "core/GameState.h"

/*
 * GameStateMachine — SRP
 * ------------------------
 * Owns the current GameState and all valid transitions.
 * No rendering, no physics, no input — only state logic.
 *
 * MVC role: shared by Controller (transitions) and View (reads state
 * to decide what to render) without either knowing the other.
 */
class GameStateMachine {
public:
    GameStateMachine() = default;

    GameState getState() const { return state_; }

    void transitionTo(GameState next) { state_ = next; }
    bool is(GameState s)        const { return state_ == s; }

private:
    GameState state_{ GameState::Menu };
};
