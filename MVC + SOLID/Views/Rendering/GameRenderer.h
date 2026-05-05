#pragma once
#include "GameModel.h"
#include "ResourceManager.h"
#include "HUD.h"
#include "ThrusterEffect.h"
#include "IObserver.h"

/*
 * GameRenderer — VIEW ROOT — MVC + SRP
 * ----------------------------------------
 * Single Responsibility : Render the game world. No logic, no input,
 *   no state mutation. Every method is const or reads only.
 *
 * MVC contract:
 *   Implements IObserver — Model notifies it after each update.
 *   Reads GameModel via const reference — never writes to Model.
 *   Receives ResourceManager& for textures — never loads assets itself.
 *
 * The hit-effect sprites are drawn here using HitEvent data produced
 * by CollisionSystem — keeping draw calls out of the Model entirely.
 */
class GameRenderer : public IObserver {
public:
    GameRenderer(const GameModel& model,
                 const ResourceManager& res,
                 int screenW, int screenH);

    // IObserver — called by Model after each update tick
    void onModelChanged() override;

    // Called by GameController each frame inside BeginDrawing/EndDrawing
    void renderGame() const;

    // Thruster effects are VIEW state (visual only, not game state)
    void updateThrusters(bool movingUp, bool movingDown, bool moving);
    void resetThrusters();

private:
    const GameModel&       model_;
    const ResourceManager& res_;
    HUD                    hud_;
    ThrusterEffect         rearThrusters_;
    ThrusterEffect         sideThrusters_;
    int screenW_, screenH_;

    void drawBackground()     const;
    void drawSpaceship()      const;
    void drawAstronaut()      const;
    void drawEnemies()        const;
    void drawAliens()         const;
    void drawBullets()        const;
    void drawHitEffects()     const;
    void drawHUD()            const;

    // Astronaut sprite animation state (VIEW-only — not in Model)
    mutable float astroFrameTimer_{ 0 };
    mutable float astroFrameX_{ 0 };
    float         astroFrameWidth_{ 0 };
};
