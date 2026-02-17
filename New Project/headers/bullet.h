#ifndef bullet_h

#define bullet_h

#include "raylib.h"


typedef struct {
	Rectangle pos_rec;
	Vector2 velocity;
	int active;
} bullet;

void init_bullets(bullet bullets[], int cnt);
void spawn_bullet(bullet* b, Rectangle spaceship_pos, Vector2 dir, float speed);

void update_bullet_pos(bullet bullets[], int cnt, float speed, float dt, int screen_width);

void  draw_bullet(bullet bullets[], int cnt, Texture2D bullet_image);

#endif