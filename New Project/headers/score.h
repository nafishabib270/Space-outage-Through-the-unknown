#ifndef SCORE_H
#define SCORE_H

/*
 * score.h - Score and session-level statistics (Single Responsibility)
 *
 * SOLID Principles Applied:
 *  - Single Responsibility: Previously, score, spaceship_health, level,
 *    hit_power, and maxHealth were scattered as globals in main.c.
 *    Grouping them here gives one module one reason to change: when
 *    scoring rules change, only this module is affected.
 *  - Open/Closed: New per-level bonuses or multipliers can be added
 *    by extending SessionStats and its functions without touching
 *    collision or rendering code.
 */

#define MAX_HEALTH 100

typedef struct {
    int   score;
    int   spaceship_health;
    int   astronaut_health;
    int   hit_power;
    int   level;
} SessionStats;

/* Initialise all fields to their start-of-game defaults. */
void  session_reset(SessionStats *s);

/* Award a kill. Updates score and, every 10 kills, increases hit_power. */
void  session_register_kill(SessionStats *s);

/* Apply damage to spaceship. Returns 1 if spaceship is now destroyed. */
int   session_damage_spaceship(SessionStats *s, int amount);

/* Apply damage to astronaut. Returns 1 if astronaut is dead. */
int   session_damage_astronaut(SessionStats *s, int amount);

#endif /* SCORE_H */
