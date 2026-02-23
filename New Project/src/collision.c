#include "collision.h"
#include "raylib.h"

/* Shrink a rectangle inward by `margin` pixels on each side. */
static Rectangle shrink_rec(Rectangle r, float margin) {
    return (Rectangle){
        r.x + margin, r.y + margin,
        r.width  - margin * 2,
        r.height - margin * 2
    };
}

void collision_bullets_vs_entities(
    Bullet        bullets[],   int bullet_count,
    EntityBase    entities[],  int entity_count,
    SessionStats *stats,
    Texture2D     hit_tex,
    Sound         hit_sounds[], int *sound_idx, int sound_count)
{
    Rectangle hit_src = {0, 0, (float)hit_tex.width, (float)hit_tex.height};

    for (int i = 0; i < bullet_count; i++) {
        if (!bullets[i].active) continue;

        for (int j = 0; j < entity_count; j++) {
            if (!entities[j].alive) continue;

            if (!CheckCollisionRecs(entities[j].rec, bullets[i].rec)) continue;

            /* Draw hit effect centred on the entity. */
            Rectangle effect_rec = {
                entities[j].rec.x - entities[j].rec.width  / 5.0f,
                entities[j].rec.y,
                entities[j].rec.width  / 1.3f,
                entities[j].rec.width  / 1.3f
            };
            DrawTexturePro(hit_tex, hit_src, effect_rec,
                           (Vector2){0, 0}, 0.0f, WHITE);

            /* Play a hit sound (round-robin pool to avoid clipping). */
            PlaySound(hit_sounds[*sound_idx]);
            *sound_idx = (*sound_idx + 1) % sound_count;

            /* Apply damage. */
            entities[j].health -= stats->hit_power;
            bullets[i].active   = 0;

            if (entities[j].health <= 0) {
                entities[j].alive  = 0;
                entities[j].health = 150.0f; /* Reset for next spawn. */
                session_register_kill(stats);
            }

            break; /* One bullet can only hit one entity. */
        }
    }
}

int collision_entities_vs_player(
    EntityBase  entities[], int entity_count,
    Rectangle   player_rec,
    int         damage)
{
    /* God-mode debug key — unchanged from original. */
    if (IsKeyDown(KEY_L)) return 0;

    Rectangle player_hitbox = shrink_rec(player_rec, 20.0f);
    int total_damage = 0;

    for (int i = 0; i < entity_count; i++) {
        if (!entities[i].alive) continue;
        Rectangle entity_hitbox = shrink_rec(entities[i].rec, 20.0f);
        if (CheckCollisionRecs(player_hitbox, entity_hitbox)) {
            total_damage      += damage;
            entities[i].alive  = 0;
        }
    }
    return total_damage;
}
