#include "bullet.h"


void init_bullets(bullet bullets[], int cnt) {
	for (int i = 0; i < cnt; i++) {
		bullets[i].active = 0;
	}
}


void spawn_bullet(bullet* b, Rectangle spaceship_pos, Vector2 dir, float speed) {
	Rectangle rec = { spaceship_pos.x + spaceship_pos.width - spaceship_pos.width * .2, spaceship_pos.y + spaceship_pos.height / 1.5 , spaceship_pos.width / 6, spaceship_pos.height / 6 };
	b->active = 1;
	b->pos_rec = rec;
	b->velocity.x = dir.x * speed;
	b->velocity.y = dir.y * speed;

}

void update_bullet_pos(bullet bullets[], int cnt, float speed, float dt, int screen_width) {

	for (int i = 0; i < cnt; i++) {
		if (bullets[i].active == 1) {
		
			bullets[i].pos_rec.x += bullets[i].velocity.x * dt;
			bullets[i].pos_rec.y += bullets[i].velocity.y * dt;

			if (bullets[i].pos_rec.x > screen_width || bullets[i].pos_rec.x < 0 ||
				bullets[i].pos_rec.y > GetScreenHeight() || bullets[i].pos_rec.y < 0) {
				bullets[i].active = 0;
			}
		}
	}

}

void  draw_bullet(bullet bullets[], int cnt, Texture2D bullet_image) {
	Rectangle src_rec = { 0, 0, bullet_image.width, bullet_image.height };

	for (int i = 0; i < cnt; i++) {

		if (bullets[i].active == 1)
			DrawTexturePro(bullet_image, src_rec, bullets[i].pos_rec, (Vector2) { 0, 0 }, 0.0, WHITE);

	}
}