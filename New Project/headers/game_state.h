#ifndef GAME_STATE_H
#define GAME_STATE_H

/*
 * game_state.h - Global game state enum (Single Responsibility)
 *
 * SOLID Principles Applied:
 *  - Single Responsibility: This file owns exactly one concern —
 *    tracking which top-level screen the game is currently showing.
 *    It does NOT own score, health, or any other variable (those live
 *    in their own modules).
 */

typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_EXIT
} GameState;

extern GameState g_current_state;

#endif /* GAME_STATE_H */
