#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "raylib.h"
#include "enemy.h" // Optional if you reuse structs
#include "alien1.h"

void init_alien(Alien1 aliens1[], int alien_cnt, Texture2D alien_tex) {
    for (int i = 0; i < alien_cnt; i++) {
        aliens1[i].alive = 0;
        aliens1[i].health = 100;
        aliens1[i].velocity_x = 0;
        aliens1[i].rec.width = 200;
            aliens1[i].rec.height = 100;
            aliens1[i].frame_timer = 0;
            aliens1[i].cur_frame = 0;

    }
}

void Spawn_alien(Alien1* alien1, int screen_width, int screen_height, int velo) {
    alien1->rec.x = screen_width + 10;
    alien1->rec.y = 535;
    alien1->health = 100;
    alien1->velocity_x = velo;
    alien1->alive = 1;
}

void update_alien(Alien1 aliens1[], int cnt, int speed, float dt) {
    const int total_frames = 8;       // Assuming 8 frames in your alien sprite sheet
    const float frame_time = 0.1f;    // Time per frame (0.1s per frame = 10fps)

    for (int i = 0; i < cnt; i++) {
        if (!aliens1[i].alive) continue;

        // Move toward astronaut
        aliens1[i].rec.x -= aliens1[i].velocity_x * dt;

        // Animate
        aliens1[i].frame_timer += dt;
        if (aliens1[i].frame_timer >= frame_time) {
            aliens1[i].cur_frame++;
            if (aliens1[i].cur_frame >= total_frames)
                aliens1[i].cur_frame = 0;
            aliens1[i].frame_timer = 0;
        }

        // Off-screen or dead cleanup
        if (aliens1[i].rec.x < -aliens1[i].rec.width || aliens1[i].health <= 0)
            aliens1[i].alive = 0;
    }
}


void draw_alien(Alien1 aliens[], int cnt, Texture2D alien_src) {
    int total_frames = 8;
    int frame_width = alien_src.width / total_frames;

    for (int i = 0; i < cnt; i++) {
        if (aliens[i].alive) {
            Rectangle src = {
                aliens[i].cur_frame * frame_width,
                0,
                frame_width,
                alien_src.height
            };

            DrawTexturePro(
                alien_src,
                src,
                aliens[i].rec,
                (Vector2) {
                0, 0
            },
                0.0f,
                WHITE
            );
        }
    }
}


