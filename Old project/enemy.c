#include <stdlib.h>
#include <raylib.h>
#include "enemy.h"


void init_enemy(Enemy enemies[], int enemy_cnt, Texture2D enemy_tex) {

	for (int i = 0; i < enemy_cnt; i++) {
		enemies[i].alive = 0;
		enemies[i].health = 100;
		enemies[i].tar_locked = 0;
		enemies[i].velocity_x = 0;
		enemies[i].velocity_y = 0;
		enemies[i].rec.width = enemy_tex.width / 5;
		enemies[i].rec.height = enemy_tex.height / 5;
	}
}

	void SpawnEnemy(Enemy *enemy, int screen_width, int screen_height, int velo) {
		enemy->rec.x = screen_width + 10;
		enemy->rec.y = rand() % (screen_height - (int)enemy->rec.height);
		enemy->health = 100;
		enemy->velocity_x = velo;
		enemy->tar_locked = 0;
		enemy->velocity_y = 0;
		enemy->alive = 1;
	}

void update_enemy(Enemy enemies[],Rectangle char_pos, int cnt, float del_t) {



	for (int i = 0; i < cnt; i++) {
		if (enemies[i].alive == 1) {
			enemies[i].rec.x -= enemies[i].velocity_x * del_t;
			enemies[i].rec.y -= enemies[i].velocity_y * del_t;


			if (enemies[i].tar_locked == 0) {

				float dif = enemies[i].rec.x - char_pos.x + char_pos.width;
				srand(time(NULL));

				if (dif <= 1000 + (rand() % 300)) {
					enemies[i].tar_locked = 1;

					float speed_y = 0;

					float time = dif / enemies[i].velocity_x;
					float dis = enemies[i].rec.y - (float)enemies[i].rec.height / 2 - (char_pos.y - (float)char_pos.height / 2);


					if(time != 0)	speed_y = dis / time;
		
				
					enemies[i].velocity_y = speed_y;
				}
			}

			if (enemies[i].rec.x + enemies[i].rec.width < 0) {
				enemies[i].alive = 0; // offscreen
			}
			//if (enemies[i].rec.x + enemies[i].rec.width < 0) {
			//	enemies[i].alive = 0; // offscreen
			//}
			//if (enemies[i].rec.x + enemies[i].rec.width < 0) {
			//	enemies[i].alive = 0; // offscreen
			//}
			//if (enemies[i].rec.x + enemies[i].rec.width < 0) {
			//	enemies[i].alive = 0; // offscreen
			//}
		}
	}

}

void draw_enemy(Enemy enemies[], int cnt, Texture2D enemy_src) {
	for (int i = 0; i < cnt; i++) {
		if (enemies[i].alive) {
			DrawTexturePro(enemy_src, (Rectangle) {0, 0, enemy_src.width, enemy_src.height},  enemies[i].rec, (Vector2) { 0, 0 }, 0.0, WHITE);
		}
	}
}

