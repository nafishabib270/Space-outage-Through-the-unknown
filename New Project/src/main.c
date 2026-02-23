/*
 * main.c - Game entry point and top-level loop (Single Responsibility)
 *
 * SOLID Principles Applied:
 *  - Single Responsibility: main.c is an ORCHESTRATOR only.  It owns
 *    resource loading/unloading, the game loop, and calls into focused
 *    modules.  No game logic (collision math, spawn timing, movement
 *    physics) lives here.
 *  - Dependency Inversion: main.c depends on module abstractions
 *    (enemy.h, collision.h, spawner.h, …), not on implementation
 *    details buried inside those modules.
 *  - Open/Closed: Adding a new level or enemy type means writing a new
 *    module and calling it from the appropriate state handler below —
 *    the existing handlers are not modified.
 *
 * Key improvements over the original main.c
 * ------------------------------------------
 *  1. ~30 global variables replaced by structs passed by pointer.
 *  2. Six copy-pasted collision loops replaced by collision_bullets_vs_entities().
 *  3. Spawn-timing state moved to EnemySpawner / AlienSpawner.
 *  4. Spaceship/astronaut physics extracted to their own modules.
 *  5. All UI calls go through ui.h (ui_button, ui_health_bar, ui_score).
 *  6. DrawHealthBar() removed from main.c — it now lives in ui.c.
 */

#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#include "game_state.h"
#include "score.h"
#include "ui.h"
#include "entity.h"
#include "enemy.h"
#include "alien.h"
#include "bullet.h"
#include "booster.h"
#include "collision.h"
#include "spawner.h"
#include "spaceship.h"
#include "astronaut.h"

/* ---------- constants -------------------------------------------------- */

#define SCREEN_W        1800
#define SCREEN_H         900
#define HIT_SOUND_CNT     20
#define GRAVITY        1500.0f
#define JUMP_SPEED      700.0f
#define SCROLL_SPEED    200.0f
#define SHOOT_COOLDOWN    0.1f
#define ENEMY_DAMAGE      20
#define ALIEN_DAMAGE      20

/* ---------- resource bundle (Dependency Inversion — main owns assets) -- */

typedef struct {
    Texture2D spaceship;
    Texture2D background;
    Texture2D enemy_tex;
    Texture2D alien_tex;
    Texture2D astronaut_tex;
    Texture2D bullet_tex;
    Texture2D hit_effect_tex;
    Music     bgm;
    Sound     laser;
    Sound     hit[HIT_SOUND_CNT];
} Resources;

static void load_resources(Resources *r) {
    r->spaceship     = LoadTexture("spaceship1.png");
    r->background    = LoadTexture("Map1.png");
    r->enemy_tex     = LoadTexture("enemy.png");
    r->alien_tex     = LoadTexture("Alien1.png");
    r->astronaut_tex = LoadTexture("Char_Robot.png");
    r->bullet_tex    = LoadTexture("bullet_1.png");
    r->hit_effect_tex= LoadTexture("hiteffect.png");
    r->bgm           = LoadMusicStream("background_music_1.mp3");
    r->laser         = LoadSound("laser-gun-81720.mp3");
    for (int i = 0; i < HIT_SOUND_CNT; i++)
        r->hit[i]    = LoadSound("hit_sound.wav");
}

static void unload_resources(Resources *r) {
    UnloadTexture(r->spaceship);
    UnloadTexture(r->background);
    UnloadTexture(r->enemy_tex);
    UnloadTexture(r->alien_tex);
    UnloadTexture(r->astronaut_tex);
    UnloadTexture(r->bullet_tex);
    UnloadTexture(r->hit_effect_tex);
    UnloadMusicStream(r->bgm);
    UnloadSound(r->laser);
    for (int i = 0; i < HIT_SOUND_CNT; i++)
        UnloadSound(r->hit[i]);
}

/* ---------- game world (all mutable state in one struct) --------------- */

typedef struct {
    /* --- entity pools --- */
    Enemy         enemies[MAX_ENEMIES];
    Alien         aliens[MAX_ALIENS];
    Bullet        bullets[MAX_BULLETS];
    Particle      rear_boosters[MAX_PARTICLES];
    Particle      side_boosters[MAX_PARTICLES];

    /* --- controllers --- */
    SpaceshipState ship;
    AstronautState astro;

    /* --- spawners --- */
    EnemySpawner  enemy_spawner;
    AlienSpawner  alien_spawner;

    /* --- session stats --- */
    SessionStats  stats;

    /* --- scrolling background --- */
    Rectangle     bg_src;
    Rectangle     bg_pos1;
    Rectangle     bg_pos2;

    /* --- bullet state --- */
    int           bullet_slot;
    float         bullet_timer;

    /* --- phase flags --- */
    int           on_spaceship;  /* 1 = piloting, 0 = astronaut phase */
    int           once;          /* one-shot flag for phase transition */
    int           hit_snd_idx;
} GameWorld;

/* Initialise / reset the world for a fresh game. */
static void world_reset(GameWorld *w, const Resources *r) {
    session_reset(&w->stats);

    enemy_init_pool(w->enemies, MAX_ENEMIES, r->enemy_tex);
    alien_init_pool(w->aliens,  MAX_ALIENS);
    bullet_init_pool(w->bullets, MAX_BULLETS);
    booster_init(w->rear_boosters);
    booster_init(w->side_boosters);

    spaceship_init(&w->ship, SCREEN_W, SCREEN_H,
                   r->spaceship.width, r->spaceship.height);
    astronaut_init(&w->astro, r->astronaut_tex);

    enemy_spawner_init(&w->enemy_spawner, 10.0f);
    alien_spawner_init(&w->alien_spawner, SCROLL_SPEED);

    w->bg_src  = (Rectangle){0, 0, (float)r->background.width,
                                   (float)r->background.height};
    w->bg_pos1 = (Rectangle){0, 0, SCREEN_W, SCREEN_H};
    w->bg_pos2 = (Rectangle){SCREEN_W - 50, 0, SCREEN_W, SCREEN_H};

    w->bullet_slot  = 0;
    w->bullet_timer = 0;
    w->on_spaceship = 1;
    w->once         = 1;
    w->hit_snd_idx  = 0;
}

/* ---------- state handlers --------------------------------------------- */

static void handle_menu(GameWorld *w, const Resources *r) {
    BeginDrawing();
    ClearBackground(BLACK);

    DrawText(" Space Outage ", 675, 200, 60, DARKBLUE);

    Rectangle play_btn = {700, 400, 400, 80};
    Rectangle exit_btn = {700, 500, 400, 80};

    ui_score(SCREEN_W, w->stats.score, 60, RED);

    if (ui_button(play_btn, "PLAY")) {
        world_reset(w, r);
        g_current_state = STATE_GAME;
    }
    if (ui_button(exit_btn, "EXIT"))
        g_current_state = STATE_EXIT;

    EndDrawing();
}

/* ---- helper: fire a bullet from origin toward mouse / fixed direction -- */

static void try_fire_bullet(GameWorld *w, Rectangle origin, Vector2 dir,
                             float speed, const Resources *r, float dt)
{
    w->bullet_timer += dt;
    if (w->bullet_timer < SHOOT_COOLDOWN) return;

    bullet_spawn(&w->bullets[w->bullet_slot], origin, dir, speed);
    PlaySound(r->laser);
    w->bullet_timer = 0;
    w->bullet_slot  = (w->bullet_slot + 1) % MAX_BULLETS;
}

/* ---- run collision pass for both entity types -------------------------- */

static void run_bullet_collisions(GameWorld *w, const Resources *r) {
    /* Bullets vs homing enemies. */
    collision_bullets_vs_entities(
        w->bullets, MAX_BULLETS,
        (EntityBase *)w->enemies, MAX_ENEMIES,   /* LSP: EntityBase is first member */
        &w->stats, r->hit_effect_tex,
        r->hit, &w->hit_snd_idx, HIT_SOUND_CNT
    );
    /* Bullets vs ground aliens. */
    collision_bullets_vs_entities(
        w->bullets, MAX_BULLETS,
        (EntityBase *)w->aliens, MAX_ALIENS,
        &w->stats, r->hit_effect_tex,
        r->hit, &w->hit_snd_idx, HIT_SOUND_CNT
    );
}

/* ---- LEVEL 0: spaceship phase ----------------------------------------- */

static void update_draw_spaceship_phase(GameWorld *w, const Resources *r, float dt) {
    /* --- scrolling background --- */
    w->bg_pos1.x -= SCROLL_SPEED * dt;
    w->bg_pos2.x -= SCROLL_SPEED * dt;
    if (w->bg_pos2.x <= 5) {
        w->bg_pos1.x = 0;
        w->bg_pos2.x = SCREEN_W - 50;
    }
    DrawTexturePro(r->background, w->bg_src, w->bg_pos1, (Vector2){0,0}, 0, WHITE);
    DrawTexturePro(r->background, w->bg_src, w->bg_pos2, (Vector2){0,0}, 0, WHITE);

    /* --- spaceship movement (only while alive) --- */
    if (w->stats.spaceship_health > 0) {
        int was_up   = IsKeyDown(KEY_W);
        int was_down = IsKeyDown(KEY_S);
        spaceship_handle_input(&w->ship, SCREEN_W, SCREEN_H, dt);
        if (was_up)   booster_update_side(w->side_boosters, w->ship.rec,  1);
        if (was_down) booster_update_side(w->side_boosters, w->ship.rec, -1);
    }

    /* --- crash sequence --- */
    if (w->stats.spaceship_health <= 0) {
        if (spaceship_update_crash(&w->ship, GRAVITY, SCREEN_H, dt)) {
            w->on_spaceship = 0;
        }
    }

    /* --- enemy spawning & movement --- */
    enemy_spawner_update(&w->enemy_spawner, w->enemies, MAX_ENEMIES,
                         SCREEN_W, SCREEN_H, dt);
    enemy_update_all(w->enemies, MAX_ENEMIES, w->ship.rec, dt);
    enemy_draw_all(w->enemies, MAX_ENEMIES, r->enemy_tex);

    /* --- shooting --- */
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 right = {1, 0};
        try_fire_bullet(w, w->ship.rec, right, 1200, r, dt);
    }

    bullet_update_all(w->bullets, MAX_BULLETS, dt, SCREEN_W);
    bullet_draw_all(w->bullets, MAX_BULLETS, r->bullet_tex);

    /* --- boosters --- */
    booster_update(w->rear_boosters, w->ship.rec);
    booster_draw(w->rear_boosters);
    booster_draw(w->side_boosters);
    booster_coast(w->side_boosters);

    /* --- collisions --- */
    run_bullet_collisions(w, r);

    int ship_hit = collision_entities_vs_player(
        (EntityBase *)w->enemies, MAX_ENEMIES, w->ship.rec, ENEMY_DAMAGE);
    if (ship_hit) session_damage_spaceship(&w->stats, ship_hit);

    /* --- draw spaceship --- */
    if (w->ship.rec.x > 0) {
        Rectangle ship_src = {0, 0,
            (float)r->spaceship.width, (float)r->spaceship.height};
        DrawTexturePro(r->spaceship, ship_src, w->ship.rec,
                       (Vector2){0,0}, w->ship.rotation, WHITE);
    }

    ui_health_bar(25, 25, 300, 50,
                  w->stats.spaceship_health, MAX_HEALTH);
    ui_score(SCREEN_W, w->stats.score, 60, WHITE);
}

/* ---- LEVEL 0: astronaut phase (after ship crashes) -------------------- */

static void update_draw_astronaut_phase(GameWorld *w, const Resources *r, float dt) {
    /* Slide the crashed ship off screen. */
    if (w->ship.rec.x > 0)
        w->ship.rec.x -= SCROLL_SPEED * dt;

    /* One-time: slow enemy spawn rate when transitioning. */
    if (w->once) {
        w->enemy_spawner.time_limit = 2.0f;
        w->once = 0;
    }

    Rectangle astro_target = {
        w->astro.rec.x + 200, w->astro.rec.y + 75,
        w->astro.rec.width + 10, w->astro.rec.height + 10
    };
    enemy_update_all(w->enemies, MAX_ENEMIES, astro_target, dt);
    enemy_draw_all(w->enemies, MAX_ENEMIES, r->enemy_tex);

    astronaut_update(&w->astro, GRAVITY, JUMP_SPEED, dt, SCREEN_W);
    astronaut_draw(&w->astro, r->astronaut_tex);

    /* --- directional shooting toward mouse --- */
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        Vector2 centre = {
            w->astro.rec.x + w->astro.rec.width  / 2.0f,
            w->astro.rec.y + w->astro.rec.height / 2.0f
        };
        Vector2 dir = {mouse.x - centre.x, mouse.y - centre.y};
        float   len = sqrtf(dir.x * dir.x + dir.y * dir.y);
        if (len != 0) { dir.x /= len; dir.y /= len; }

        Rectangle fire_origin = {
            w->astro.rec.x - 40, w->astro.rec.y - 40,
            w->astro.rec.width, w->astro.rec.width
        };
        try_fire_bullet(w, fire_origin, dir, 900, r, dt);
    }

    bullet_update_all(w->bullets, MAX_BULLETS, dt, SCREEN_W);
    bullet_draw_all(w->bullets, MAX_BULLETS, r->bullet_tex);

    /* --- alien spawning & movement --- */
    alien_spawner_update(&w->alien_spawner, w->aliens, MAX_ALIENS,
                         SCREEN_W, SCREEN_H, dt);
    alien_update_all(w->aliens, MAX_ALIENS, dt);
    alien_draw_all(w->aliens, MAX_ALIENS, r->alien_tex);

    /* --- collisions --- */
    run_bullet_collisions(w, r);

    int astro_hit = collision_entities_vs_player(
        (EntityBase *)w->enemies, MAX_ENEMIES, w->astro.rec, ENEMY_DAMAGE);
    astro_hit += collision_entities_vs_player(
        (EntityBase *)w->aliens,  MAX_ALIENS,  w->astro.rec, ALIEN_DAMAGE);
    if (astro_hit) {
        if (session_damage_astronaut(&w->stats, astro_hit)) {
            g_current_state = STATE_MENU; /* Game over → return to menu. */
        }
    }

    ui_health_bar(25, 25, 300, 50,
                  w->stats.astronaut_health, MAX_HEALTH);
    ui_score(SCREEN_W, w->stats.score, 60, WHITE);
}

/* ---- top-level GAME handler ------------------------------------------- */

static void handle_game(GameWorld *w, const Resources *r) {
    float dt = GetFrameTime();

    BeginDrawing();
    ClearBackground(GRAY);

    if (w->on_spaceship)
        update_draw_spaceship_phase(w, r, dt);
    else
        update_draw_astronaut_phase(w, r, dt);

    EndDrawing();
}

/* ---------- entry point ------------------------------------------------ */

int main(void) {
    srand((unsigned)time(NULL));

    InitWindow(SCREEN_W, SCREEN_H, "GeoWorld");
    InitAudioDevice();

    Resources r;
    load_resources(&r);
    PlayMusicStream(r.bgm);

    GameWorld world = {0};
    world_reset(&world, &r);

    SetTargetFPS(120);

    while (!WindowShouldClose() && g_current_state != STATE_EXIT) {
        UpdateMusicStream(r.bgm);

        switch (g_current_state) {
            case STATE_MENU: handle_menu(&world, &r); break;
            case STATE_GAME: handle_game(&world, &r); break;
            default: break;
        }
    }

    unload_resources(&r);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
