#pragma once
#include "ICollidable.h"
#include "Bullet.h"
#include "SessionStats.h"
#include "raylib.h"
#include <vector>

/*
 * CollisionSystem — MODEL SYSTEM — SRP + DIP
 * -------------------------------------------
 * SRP : One job — detect overlaps and apply immediate consequences.
 * DIP : Accepts ICollidable* — never imports HomingEnemy or GroundAlien.
 * OCP : New entity types require zero changes here.
 * LSP : HomingEnemy* and GroundAlien* substitute identically.
 *
 * MVC : Pure Model system. View renders hit effects separately using
 *       data provided by the Model; it does NOT call DrawTexturePro here
 *       (that would mix Model and View concerns).
 *
 * NOTE: In MVC the hit flash is a VIEW concern. CollisionSystem exposes
 *       a HitEvent list so GameRenderer can draw effects without
 *       CollisionSystem ever touching rendering.
 */

struct HitEvent {
    Rectangle entityBounds;  // View uses this to place the hit effect sprite
    bool      killed;
};

class CollisionSystem {
public:
    CollisionSystem(SessionStats& stats);

    // Returns list of hit events so the View can render effects
    std::vector<HitEvent> testBulletsVsEntities(
        std::vector<Bullet>&      bullets,
        std::vector<ICollidable*> entities);

    // Returns total damage dealt to player this frame
    int testEntitiesVsPlayer(
        std::vector<ICollidable*> entities,
        Rectangle                 playerRect,
        int                       damagePerHit);

private:
    SessionStats& stats_;
    static Rectangle shrink(Rectangle r, float m);
};
