#include "raylib.h"
#include "game_state.h"
#include "ui.h"
#include "enemy.h"
#include "bullet.h"
#include "booster.h"
#include <stdlib.h>
#include <time.h>
#include "alien1.h"
#include <math.h>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

float enemy_timer = 0;
int enemy_now = 0;
int enemy_speed;
int alien_speed;
float bullet_timer = .2;
int bullet_now = 0;
float enemy_time_limit = 1;
float alien_time_limit = 3;

float max_enemy_time_limit = 2;

float alien_timer = 0;
float next_alien_spawn = 0;


int score = 0;
int spaceship_health = 100;
int maxHealth = 100;
int level = 0;
float gravity = 50;
float downfall_velocity = 0;

void DrawHealthBar(int x, int y, int width, int height, int health, int maxHealth) {
    // Calculate the filled width
    float ratio = (float)health / maxHealth;
    int filledWidth = (int)(width * ratio);

    // Background bar (gray)
    DrawRectangle(x, y, width, height, GRAY);

    // Health portion (green to red depending on health)
    Color healthColor = GREEN;
    if (health < 60 && health >= 30) healthColor = YELLOW;
    else if (health < 30) healthColor = RED;

    DrawRectangle(x, y, filledWidth, height, healthColor);

    // Optional: outline
    DrawRectangleLines(x, y, width, height, BLACK);
}





void ResetGame(Rectangle* spaceship_pos, Enemy enemies[10], Alien1 aliens1[20], bullet bullets[50], int screenWidth, int screenHeight, Texture2D spaceship, Texture2D enemy, Texture2D alien1_tex, Particle boosters[]) {
    // Reset spaceship position
    spaceship_pos->x = screenWidth * 0.2;
    spaceship_pos->y = screenHeight * 0.5;
    spaceship_pos->width = spaceship.width / 3;
    spaceship_pos->height = spaceship.height / 3;
    score = 0;
    spaceship_health = 100;
    level = 0;

    // Reset enemies
    for (int i = 0; i < 10; i++) {
        enemies[i].alive = 0;
        enemies[i].health = 100;
        enemies[i].rec.x = 0;
        enemies[i].rec.y = 0;
        enemies[i].tar_locked = 0;
        enemies[i].velocity_x = 0;
        enemies[i].velocity_y = 0;
        enemies[i].rec.width = enemy.width / 5;
        enemies[i].rec.height = enemy.height / 5;
    }

    for (int i = 0; i < 20; i++) {
        aliens1[i].alive = 0;
        aliens1[i].health = 100;
        aliens1[i].velocity_x = 0;
        aliens1[i].rec.width = alien1_tex.width;
        aliens1[i].rec.height = alien1_tex.height;
    }

    for (int i = 0; i < 50; i++) {
        bullets[i].active = 0;
    }

    for (int i = 0; i < 100; i++) boosters[i].active = 0;


    bullet_now = 0;
    bullet_timer = 0;
    enemy_timer = 0;
    enemy_now = 0;



    enemy_time_limit = 1;
    alien_time_limit = 3;


}

Particle boosters[100];
Particle side_boosters[100];




int main(void)
{

    const int screenWidth = 1800;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "GeoWorld");
    InitAudioDevice();

    Music background_music = LoadMusicStream("background_music_1.mp3");
    Sound laser_gun_sound = LoadSound("laser-gun-81720.mp3");
    Sound hit_sound[20];

    int hit_sound_tracker = 0;

    for (int i = 0; i < 20; i++) {
        hit_sound[i] = LoadSound("hit_sound.wav");
    }

    // Load resources AFTER window is created

    Texture2D spaceship = LoadTexture("spaceship1.png");

    Texture2D background = LoadTexture("Map1.png");
    Rectangle background_src = { 0, 0, background.width, background.height };
    Rectangle background_pos1 = { 0, 0, screenWidth, screenHeight };
    Rectangle background_pos2 = { screenWidth - 50, 0, screenWidth, screenHeight };

    Rectangle spaceship_src = { 0, 0, spaceship.width, spaceship.height };
    Rectangle spaceship_pos = { screenWidth * 0.2, screenHeight * 0.5 , spaceship.width * 0.1f, spaceship.height * 0.1f };

    Texture2D enemy = LoadTexture("enemy.png");
    Rectangle enemy_src = { 0, 0, enemy.width, enemy.height };

    Texture2D bullet_image = LoadTexture("bullet_1.png");
    Texture2D hit_effect_image = LoadTexture("hiteffect.png");
    Rectangle hit_effect_src = { 0, 0, hit_effect_image.width, hit_effect_image.height };


    // code for second part pf the game
    Texture2D map2 = LoadTexture("Map1.png");

    Texture2D astronaut = LoadTexture("Char_Robot.png");
    Rectangle astronaut_src = { 0, 0, astronaut.width / 8, astronaut.height };

    Texture2D alien1_tex = LoadTexture("Alien1.png");
    Rectangle alien1_src = { 0, 0, alien1_tex.width / 8, alien1_tex.height };

    float update_astronaut_src_val = astronaut.width / 8;

    float astronaut_update_time = .075;
    float astronaut_timer = 0;


    int frame_cnt = 7;
    int cur_frame = 0;

    int frame_counter = 0;
    int jump_frame = 0;

    // y-position of the ground    Vector2 scarfy_pos = { screenWidth * .1 , lineY - scarfY_height * 0.40f - 75.0f};

  /*  float gravity = 800.0f;
    float jump_speed = 500;
    float veloccity_y = 0;
    bool on_ground = true;*/

    float gravity = 1500.0f;       // stronger gravity for realistic jump
    float jump_speed = 700.0f;     // jump impulse
    float veloccity_y = 0.0f;
    int astronaut_on_air = 0;


    float ini_spaceship_speed = 200;
    float spaceship_speed = ini_spaceship_speed;
    float spaceship_acceleration = 5;
    float spaceship_friction = 10;

    int x = 0;

    float time_limit = .1f;
    float time = 0;

    float jump_time_limit = 0.1f;

    float scroll_speed = 200;



    float rotation = 0.0f;


    Enemy enemies[10];
    Alien1 aliens1[20];
    bullet bullets[50];

    float shoot_cooldown_timer = .1;

    int on_spaceship = 1;

    /*   int astronaut_on_air = 0;*/

    Rectangle astronaut_pos = { 300, 535, 165, 155 };
    Rectangle char_pos = spaceship_pos;


    next_alien_spawn = GetRandomValue(2, 5); // random between 2–5 seconds

    int once = 1;


    PlayMusicStream(background_music);



    SetTargetFPS(120);
    //--------------------------------------------------------------------------------------
    // Main game

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        UpdateMusicStream(background_music);

        if (current_state == MENU) {
            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("   GeoWorld ", 675, 200, 60, DARKBLUE);

            Rectangle playBtn = { 700, 400, 400, 80 };
            Rectangle exitBtn = { 700, 500, 400, 80 };

            int font_size = 60;
            const char* score_text = TextFormat("SCORE : %d", score);

            int text_width = MeasureText(score_text, font_size);

            DrawText(score_text, screenWidth - text_width - 15, 10, font_size, RED);

            if (button(playBtn, "PLAY")) {

                ResetGame(&spaceship_pos, enemies, aliens1, bullets, screenWidth, screenHeight, spaceship, enemy, alien1_tex, boosters);
                init_enemy(enemies, 10, enemy);
                init_alien(aliens1, 20, alien1_tex);
                InitBoosters(boosters);
                InitBoosters(side_boosters);

                init_bullets(bullets, 50);

                enemy_speed = 10;
                alien_speed = scroll_speed;

                current_state = GAME;
            }

            if (button(exitBtn, "EXIT")) current_state = EXIT;

            EndDrawing();
        }

        else if (current_state == GAME) {

            float dt = GetFrameTime();

            if (level == 0) {


                BeginDrawing();
                ClearBackground(GRAY);

                if (spaceship_health > 0) {
                    if (IsKeyDown(KEY_W)) {

                        if (IsKeyPressed(KEY_W)) {
                            spaceship_speed = ini_spaceship_speed;
                            rotation = 0;
                        }

                        update_side_boosters(side_boosters, spaceship_pos, 1);

                        if (spaceship_pos.y > 0) {
                            spaceship_pos.y -= spaceship_speed * dt;
                            spaceship_speed += spaceship_acceleration;
                        }
                    }

                    if (IsKeyDown(KEY_S)) {
                        if (IsKeyPressed(KEY_S)) {
                            spaceship_speed = ini_spaceship_speed;
                            rotation = 0;
                        }

                        update_side_boosters(side_boosters, spaceship_pos, -1);

                        if (spaceship_pos.y < screenHeight - spaceship_pos.height) {
                            spaceship_pos.y += spaceship_speed * dt;
                            spaceship_speed += spaceship_acceleration;
                        }
                    }

                    if (IsKeyDown(KEY_D)) {
                        if (IsKeyPressed(KEY_D)) {
                            spaceship_speed = ini_spaceship_speed;
                        }
                        if (spaceship_pos.x < screenWidth - spaceship_pos.width) {
                            spaceship_pos.x += spaceship_speed * dt;
                            spaceship_speed += spaceship_acceleration;
                        }
                    }

                    if (IsKeyDown(KEY_A)) {
                        if (IsKeyPressed(KEY_A)) {
                            spaceship_speed = ini_spaceship_speed;
                        }
                        if (spaceship_pos.x > 0) {
                            spaceship_pos.x -= spaceship_speed * dt;
                            spaceship_speed += spaceship_acceleration;
                        }
                    }

                    if (IsKeyReleased(KEY_S) && IsKeyReleased(KEY_W)
                        && IsKeyReleased(KEY_D) && IsKeyReleased(KEY_A)) {
                        if (spaceship_speed > 0) {
                            spaceship_speed -= spaceship_friction * dt;
                        }
                    }
                }

                //if (!IsKeyPressed(KEY_W) && !IsKeyPressed(KEY_S)) {
                //    InitBoosters(side_boosters);
                //}





                //if (character_pos.y > lineY - scarfY_height + 20) {
                //    character_pos.y = lineY - scarfY_height + 20;
                //    veloccity_y = 0;
                //    on_ground = true;
                //    x = 0;
                //}

                background_pos1.x -= scroll_speed * dt;
                background_pos2.x -= scroll_speed * dt;

                if (background_pos2.x <= 5) {
                    background_pos1.x = 0;
                    background_pos2.x = screenWidth - 50;
                }

                DrawTexturePro(background, background_src, background_pos1, (Vector2) { 0, 0 }, 0.0f, WHITE);
                DrawTexturePro(background, background_src, background_pos2, (Vector2) { 0, 0 }, 0.0f, WHITE);



                // Enemy part starts here

                enemy_timer += dt;


                if (enemy_timer >= enemy_time_limit) {

                    if (enemies[enemy_now].alive == 0)   SpawnEnemy(&enemies[enemy_now], screenWidth, screenHeight * .8, enemy_speed);

                    enemy_timer = 0;
                    enemy_now++;
                    if (enemy_now >= 10) {

                        enemy_now = 0;
                        enemy_speed += 2;
                        if (enemy_time_limit > 0.5) enemy_time_limit -= 0.1f;

                    }
                }


                if (on_spaceship) {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                        bullet_timer += dt;



                        if (bullet_timer > shoot_cooldown_timer) {

                            spawn_bullet(&bullets[bullet_now], spaceship_pos, (Vector2) { 1, 0 }, 1200);
                            PlaySound(laser_gun_sound);
                            bullet_timer = 0;
                            bullet_now++;
                            if (bullet_now >= 50) {
                                bullet_now = 0;
                            }
                        }
                    }
                }

                update_enemy(enemies, spaceship_pos, 10, enemy_speed, dt);
                draw_enemy(enemies, 10, enemy);


                if (on_spaceship) {


                    update_bullet_pos(bullets, 50, 1200, dt, screenWidth);
                    draw_bullet(bullets, 50, bullet_image);

                    UpdateBoosters(spaceship_pos, boosters);
                    DrawBoosters(boosters);

                    DrawBoosters(side_boosters);

                    only_update_boosters(side_boosters);






                    int hit_power = 20;



                    for (int i = 0; i < 50; i++) {
                        if (bullets[i].active == 1) {
                            for (int j = 0; j < 10; j++) {
                                if (enemies[j].alive == 1) {
                                    if (CheckCollisionRecs(enemies[j].rec, bullets[i].pos_rec)) {

                                        if (hit_sound_tracker >= 20) {
                                            hit_sound_tracker = 0;
                                        }

                                        Rectangle temp_rec = { enemies[j].rec.x - enemies[j].rec.width / 5, enemies[j].rec.y, enemies[j].rec.width / 1.3, enemies[j].rec.width / 1.3 };
                                        DrawTexturePro(hit_effect_image, hit_effect_src, temp_rec, (Vector2) { 0, 0 }, 0.0f, WHITE);
                                        PlaySound(hit_sound[hit_sound_tracker]);
                                        hit_sound_tracker++;

                                        enemies[j].health -= hit_power;
                                        enemies[j].velocity_x -= 1;
                                        bullets[i].active = 0;

                                        if (enemies[j].health <= 0)  enemies[j].alive = 0, enemies[j].health = 100, score++;;


                                        if (score % 10 == 0) hit_power += 10;
                                    }
                                }
                            }
                        }
                    }

                    for (int i = 0; i < 10; i++) {
                        if (enemies[i].alive) {
                            Rectangle rec1 = { enemies[i].rec.x + 20, enemies[i].rec.y + 20 , enemies[i].rec.width - 40, enemies[i].rec.height - 40 };
                            Rectangle rec2 = { spaceship_pos.x + 20, spaceship_pos.y + 20 , spaceship_pos.width - 40, spaceship_pos.height - 40 };
                            if (CheckCollisionRecs(rec2, rec1) && !IsKeyDown(KEY_L)) {
                                spaceship_health -= 20;
                                enemies[i].alive = 0;

                            }
                        }
                    }



                    if (spaceship_health <= 0) {
                        downfall_velocity += gravity * dt;
                        spaceship_pos.y += downfall_velocity * dt;
                        if (spaceship_pos.y > screenHeight - 300) {
                            on_spaceship = 0;
                        }
                    }
                }

                int game_stop = 0;

                if (game_stop) {
                    current_state = MENU;
                    EndDrawing();
                    continue;

                }


                if (!on_spaceship) {

                    if (spaceship_pos.x > 0)
                        spaceship_pos.x -= scroll_speed * dt;
                }

                if (spaceship_pos.x <= 300 && !on_spaceship) {

                    if (once)
                        enemy_time_limit = max_enemy_time_limit, once = 0;;

                    char_pos = (Rectangle){ astronaut_pos.x + 10, astronaut_pos.y + 100, astronaut_pos.width , astronaut_pos.height };

                    DrawTexturePro(astronaut, astronaut_src, astronaut_pos, (Vector2) { 0, 0 }, 0.0f, WHITE);

                    if (IsKeyDown(KEY_A)) astronaut_pos.x -= 200 * dt;  // move left
                    if (IsKeyDown(KEY_D)) astronaut_pos.x += 200 * dt;  // move right

                    // Optional: Keep astronaut inside screen
                    if (astronaut_pos.x < 0) astronaut_pos.x = 0;
                    if (astronaut_pos.x + astronaut_pos.width > GetScreenWidth())
                        astronaut_pos.x = GetScreenWidth() - astronaut_pos.width;

                    astronaut_timer += dt;
                    if ((astronaut_timer > astronaut_update_time) && (astronaut_on_air == 0)) {
                        astronaut_src.x += update_astronaut_src_val;
                        astronaut_timer = 0;
                    }
                    if (astronaut_src.x >= astronaut.width) astronaut_src.x = 0;

                    // Jump input
                    if (IsKeyPressed(KEY_W) && astronaut_on_air == 0) {
                        astronaut_on_air = 1;
                        veloccity_y = -jump_speed; // upward impulse
                    }

                    // Apply gravity
                    veloccity_y += gravity * dt;
                    astronaut_pos.y += veloccity_y * dt;

                    // Ground collision
                    if (astronaut_pos.y >= 535) {
                        astronaut_pos.y = 535;
                        veloccity_y = 0;
                        astronaut_on_air = 0;
                    }

                    static float alien_timer = 0;
                    alien_timer += dt;

                    alien_timer += dt;

                    if (alien_timer >= next_alien_spawn) {
                        // Find a free alien slot
                        for (int i = 0; i < 20; i++) {
                            if (!aliens1[i].alive) {
                                Spawn_alien(&aliens1[i], screenWidth, screenHeight, alien_speed);
                                break;
                            }
                        }

                        // Reset timer and randomize next spawn interval
                        alien_timer = 0;
                        next_alien_spawn = GetRandomValue(10, 25); // random delay between spawns
                    }


                    Rectangle temp_rec = { astronaut_pos.x - 10, astronaut_pos.y - 30, astronaut_pos.width, astronaut_pos.height };

                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                        bullet_timer += dt;

                        Vector2 mouse_pos = GetMousePosition();

                        Vector2 dir;
                        dir.x = mouse_pos.x - (astronaut_pos.x + astronaut_pos.width / 2);
                        dir.y = mouse_pos.y - (astronaut_pos.y + astronaut_pos.height / 2);
                        float length = sqrt(dir.x * dir.x + dir.y * dir.y);

                        if (length != 0) {
                            dir.x /= length;
                            dir.y /= length;
                        }

                        if (bullet_timer > shoot_cooldown_timer) {

                            spawn_bullet(&bullets[bullet_now], astronaut_pos, dir, 900);
                            PlaySound(laser_gun_sound);
                            bullet_timer = 0;
                            bullet_now++;
                            if (bullet_now >= 50) {
                                bullet_now = 0;
                            }
                        }
                    }

                    update_bullet_pos(bullets, 50, 900, dt, screenWidth);
                    draw_bullet(bullets, 50, bullet_image);

                    int hit_power = 20;

                    for (int i = 0; i < 50; i++) {
                        if (bullets[i].active == 1) {
                            for (int j = 0; j < 10; j++) {
                                if (enemies[j].alive == 1) {
                                    if (CheckCollisionRecs(enemies[j].rec, bullets[i].pos_rec)) {

                                        if (hit_sound_tracker >= 20) {
                                            hit_sound_tracker = 0;
                                        }

                                        Rectangle temp_rec = { enemies[j].rec.x - enemies[j].rec.width / 5, enemies[j].rec.y, enemies[j].rec.width / 1.3, enemies[j].rec.width / 1.3 };
                                        DrawTexturePro(hit_effect_image, hit_effect_src, temp_rec, (Vector2) { 0, 0 }, 0.0f, WHITE);
                                        PlaySound(hit_sound[hit_sound_tracker]);
                                        hit_sound_tracker++;

                                        enemies[j].health -= hit_power;
                                        enemies[j].velocity_x -= 1;
                                        bullets[i].active = 0;

                                        if (enemies[j].health <= 0)  enemies[j].alive = 0, enemies[j].health = 100, score++;;


                                        if (score % 10 == 0) hit_power += 10;
                                    }
                                }
                            }
                        }
                    }
                    for (int i = 0; i < 50; i++) {
                        if (bullets[i].active == 1) {
                            for (int j = 0; j < 10; j++) {
                                if (aliens1[j].alive == 1) {
                                    if (CheckCollisionRecs(aliens1[j].rec, bullets[i].pos_rec)) {

                                        if (hit_sound_tracker >= 20) {
                                            hit_sound_tracker = 0;
                                        }

                                        Rectangle temp_rec = { aliens1[j].rec.x - aliens1[j].rec.width / 5, aliens1[j].rec.y, aliens1[j].rec.width / 1.3, aliens1[j].rec.width / 1.3 };
                                        DrawTexturePro(hit_effect_image, hit_effect_src, temp_rec, (Vector2) { 0, 0 }, 0.0f, WHITE);
                                        PlaySound(hit_sound[hit_sound_tracker]);
                                        hit_sound_tracker++;

                                        aliens1[j].health -= hit_power;
                                        aliens1[j].velocity_x -= 1;
                                        bullets[i].active = 0;

                                        if (aliens1[j].health <= 0)  aliens1[j].alive = 0, aliens1[j].health = 100, score++;;


                                        if (score % 10 == 0) hit_power += 10;
                                    }
                                }
                            }
                        }
                    }

                    for (int i = 0; i < 10; i++) {
                        if (enemies[i].alive) {
                            Rectangle rec1 = { enemies[i].rec.x + 20, enemies[i].rec.y + 20 , enemies[i].rec.width - 40, enemies[i].rec.height - 40 };
                            Rectangle rec2 = { spaceship_pos.x + 20, spaceship_pos.y + 20 , spaceship_pos.width - 40, spaceship_pos.height - 40 };
                            if (CheckCollisionRecs(rec2, rec1) && !IsKeyDown(KEY_L)) {
                                spaceship_health -= 20;
                                enemies[i].alive = 0;

                            }
                        }
                    }



                    update_alien(aliens1, 20, alien_speed, dt);
                    draw_alien(aliens1, 20, alien1_tex);




                }


                if (spaceship_pos.x > 0) DrawTexturePro(spaceship, spaceship_src, spaceship_pos, (Vector2) { 0, 0 }, rotation, WHITE);

                DrawHealthBar(25, 25, 300, 50, spaceship_health, maxHealth);

                int font_size = 60;
                const char* score_text = TextFormat("SCORE : %d", score);

                int text_width = MeasureText(score_text, font_size);

                DrawText(score_text, screenWidth - text_width - 15, 10, font_size, WHITE);




            }
            else {

                /*    DrawTexturePro(map2, _src, background_pos1, (Vector2) { 0, 0 }, 0.0f, WHITE);*/



            }


            EndDrawing();
            //----------------------------------------------------------------------------------
        }
    }

    UnloadTexture(spaceship);
    UnloadTexture(background);
    UnloadTexture(spaceship);

    UnloadTexture(enemy);

    UnloadMusicStream(background_music);

    for (int i = 0; i < 20; i++) {

        UnloadSound(hit_sound[i]);

    }
    UnloadSound(laser_gun_sound);

    CloseAudioDevice();
    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
