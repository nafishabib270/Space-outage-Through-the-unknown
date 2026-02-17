#include "booster.h"


void InitBoosters(Particle boosters[]) {
    for (int i = 0; i < 100; i++) boosters[i].active = 0;
}

void UpdateBoosters(Rectangle spaceship_pos, Particle boosters[]) {
    // spawn new particles
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 100; j++) {
            if (!boosters[j].active) {
                boosters[j].pos = (Vector2){ spaceship_pos.x + 3, spaceship_pos.y + spaceship_pos.height / 2 + GetRandomValue(-10, 10) };
                boosters[j].radius = GetRandomValue(2, 6);
                boosters[j].life = 1.0f;
                boosters[j].color = GRAY;
                boosters[j].active = 1;
                break;
            }
        }
    }
    for (int i = 0; i < 100; i++) {
        if (boosters[i].active) {
            boosters[i].pos.x -= 3; // move left with rocket
            boosters[i].pos.y = spaceship_pos.y + spaceship_pos.height / 2 + GetRandomValue(-10, 10);
            boosters[i].life -= 0.025f;
            if (boosters[i].radius > 0) boosters[i].radius -= 0.1f;
            if (boosters[i].life <= 0) boosters[i].active = 0;
        }
    }
}

void DrawBoosters(Particle boosters[]) {
    for (int i = 0; i < 100; i++) {
        if (boosters[i].active) {
            Color c = boosters[i].color;

            c.a = (unsigned char)(boosters[i].life * 255); // fade out
            if (boosters[i].radius > 0)
                DrawCircleV(boosters[i].pos, boosters[i].radius, c);
        }
    }
}

void update_side_boosters(Particle side_boosters[], Rectangle spaceship_pos, int direction) {

    // spawn new particles
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 100; j++) {
            if (!side_boosters[j].active) {
                side_boosters[j].pos = (Vector2){ spaceship_pos.x + spaceship_pos.width / 5, spaceship_pos.y + spaceship_pos.height / 2 + GetRandomValue(-10, 10) };
                side_boosters[j].radius = GetRandomValue(2, 6);
                side_boosters[j].life = 1.0f;
                side_boosters[j].color = GRAY;
                side_boosters[j].active = 1;
                break;
            }
        }
    }

    for (int i = 0; i < 100; i++) {
        if (side_boosters[i].active) {
            side_boosters[i].pos.x -= 3; // move left with rocket
            side_boosters[i].pos.y += ((direction == 1) ? 1 : -1) * 3;
            side_boosters[i].life -= 0.025f;
            if (side_boosters[i].life <= 0) side_boosters[i].active = 0;
        }
    }


}

void only_update_boosters(Particle side_boosters[]) {
    for (int i = 0; i < 100; i++) {
        if (side_boosters[i].active) {
            side_boosters[i].pos.x -= 3;
            side_boosters[i].life -= 0.025f;
            
            if (side_boosters[i].life <= 0) side_boosters[i].active = 0;
        }
    }
}