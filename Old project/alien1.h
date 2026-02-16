
#ifndef ALIEN_H
#define ALIEN_H

#include "raylib.h"

typedef struct {
	int x_co, y_co;
	float health;
	float velocity_x;
	Rectangle rec;
	int alive;
	float frame_timer;
	int cur_frame;

} Alien1;


void init_alien(Alien1 aliens1[], int alien_cnt, Texture2D alien_tex);
void Spawn_alien(Alien1* alien1, int screen_width, int screen_height, int velo);
void update_alien(Alien1 aliens[], int cnt, int speed, float del_t);
void draw_alien(Alien1 aliens[], int cnt, Texture2D alien_src);

#endif