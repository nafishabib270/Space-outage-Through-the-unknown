
#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

typedef struct {
	int x_co, y_co;
	float health;
	float velocity_x;
	float velocity_y;
	Rectangle rec;
	int alive;
	int tar_locked;
} Enemy;


void init_enemy(Enemy enemies[], int enemy_cnt, Texture2D enemy_tex);
void SpawnEnemy(Enemy* enemy, int screen_width, int screen_height, int velo);
void update_enemy(Enemy enemies[], Rectangle char_pos, int cnt, int speed, float del_t);
void draw_enemy(Enemy enemies[], int cnt, Texture2D enemy_src);

#endif