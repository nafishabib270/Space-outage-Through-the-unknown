#pragma once

/*
 * GameState.h
 * -----------
 * SRP: The enum class owns exactly one concern — which top-level
 *      screen is currently active. Scoped enum (enum class) prevents
 *      name collisions and avoids implicit int conversions.
 */

enum class GameState {
    Menu,
    Playing,
    Exit
};
