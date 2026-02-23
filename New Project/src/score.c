#include "score.h"

void session_reset(SessionStats *s) {
    s->score             = 0;
    s->spaceship_health  = MAX_HEALTH;
    s->astronaut_health  = MAX_HEALTH;
    s->hit_power         = 10;
    s->level             = 0;
}

void session_register_kill(SessionStats *s) {
    s->score++;
    /* Every 10 kills the player's bullets deal more damage. */
    if (s->score % 10 == 0) {
        s->hit_power += 10;
    }
}

int session_damage_spaceship(SessionStats *s, int amount) {
    s->spaceship_health -= amount;
    return (s->spaceship_health <= 0);
}

int session_damage_astronaut(SessionStats *s, int amount) {
    s->astronaut_health -= amount;
    return (s->astronaut_health <= 0);
}
