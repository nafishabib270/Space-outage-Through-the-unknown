#ifndef booster

#include "raylib.h"
#define booster 

typedef struct {
    Vector2 pos;
    float radius;
    float life;
    Color color;
    int active;
} Particle;

void InitBoosters(Particle boosters[]);
void UpdateBoosters(Rectangle spaceship_pos, Particle boosters[]);
void DrawBooster(Particle boosters[]);
void update_side_boosters(Particle side_boosters[], Rectangle spaceship_pos, int direction);
void only_update_boosters(Particle side_boosters[]);

#endif 
