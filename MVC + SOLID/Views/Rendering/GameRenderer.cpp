#include "GameRenderer.h"

GameRenderer::GameRenderer(const GameModel& model,
                           const ResourceManager& res,
                           int sw, int sh)
    : model_(model), res_(res), hud_(sw, sh), screenW_(sw), screenH_(sh)
{
    astroFrameWidth_ = res_.getAstronaut().width / 8.0f;
    astroFrameX_     = 0;
}

void GameRenderer::onModelChanged() { /* future: dirty flag optimisation */ }

void GameRenderer::drawBackground() const {
    Texture2D bg  = res_.getBackground();
    Rectangle src = { 0, 0, (float)bg.width, (float)bg.height };
    DrawTexturePro(bg, src, model_.getBgPos1(), {0,0}, 0, WHITE);
    DrawTexturePro(bg, src, model_.getBgPos2(), {0,0}, 0, WHITE);
}

void GameRenderer::drawSpaceship() const {
    Rectangle pos = model_.getSpaceshipPos();
    if (pos.x <= 0) return;
    Texture2D t   = res_.getSpaceship();
    Rectangle src = { 0, 0, (float)t.width, (float)t.height };
    DrawTexturePro(t, src, pos, {0,0}, model_.getSpaceshipRot(), WHITE);
}

void GameRenderer::drawAstronaut() const {
    if (model_.isOnSpaceship()) return;
    Texture2D t = res_.getAstronaut();
    if (astroFrameWidth_ <= 0.0f) astroFrameWidth_ = t.width / 8.0f;
    float dt = GetFrameTime();
    astroFrameTimer_ += dt;
    if (astroFrameTimer_ >= 0.075f) {
        astroFrameX_ += astroFrameWidth_;
        if (astroFrameX_ >= t.width) astroFrameX_ = 0;
        astroFrameTimer_ = 0;
    }
    Rectangle src = { astroFrameX_, 0, astroFrameWidth_, (float)t.height };
    DrawTexturePro(t, src, model_.getAstronautPos(), {0,0}, 0, WHITE);
}

void GameRenderer::drawEnemies() const {
    Texture2D t   = res_.getEnemy();
    Rectangle src = { 0, 0, (float)t.width, (float)t.height };
    for (const auto& e : model_.getEnemySpawner().getEnemies()) {
        if (!e.isAlive()) continue;
        DrawTexturePro(t, src, e.getBounds(), {0,0}, 0, WHITE);
    }
}

void GameRenderer::drawAliens() const {
    Texture2D t = res_.getAlien();
    int fw = t.width / GroundAlien::FRAME_COUNT;
    for (const auto& a : model_.getAlienSpawner().getAliens()) {
        if (!a.isAlive()) continue;
        Rectangle src = { (float)(a.getCurrentFrame() * fw), 0, (float)fw, (float)t.height };
        DrawTexturePro(t, src, a.getBounds(), {0,0}, 0, WHITE);
    }
}

void GameRenderer::drawBullets() const {
    Texture2D t   = res_.getBullet();
    Rectangle src = { 0, 0, (float)t.width, (float)t.height };
    for (const auto& b : model_.getBulletPool().getBullets()) {
        if (!b.isActive()) continue;
        DrawTexturePro(t, src, b.getBounds(), {0,0}, 0, WHITE);
    }
}

void GameRenderer::drawHitEffects() const {
    Texture2D t   = res_.getHitEffect();
    Rectangle src = { 0, 0, (float)t.width, (float)t.height };
    for (const auto& ev : model_.getLastHitEvents()) {
        Rectangle fx = { ev.entityBounds.x - ev.entityBounds.width / 5.0f,
                         ev.entityBounds.y,
                         ev.entityBounds.width / 1.3f,
                         ev.entityBounds.width / 1.3f };
        DrawTexturePro(t, src, fx, {0,0}, 0, WHITE);
    }
}

void GameRenderer::drawHUD() const {
    hud_.render(model_.getStats(), model_.isOnSpaceship());
}

void GameRenderer::renderGame() const {
    ClearBackground(GRAY);
    drawBackground();
    drawSpaceship();
    rearThrusters_.render();
    sideThrusters_.render();
    drawEnemies();
    drawAliens();
    drawBullets();
    drawHitEffects();
    drawAstronaut();
    drawHUD();
}

void GameRenderer::updateThrusters(bool up, bool down, bool moving) {
    Rectangle ship = model_.getSpaceshipPos();
    rearThrusters_.emitRear(ship);
    if (up)         sideThrusters_.emitSideUp(ship);
    else if (down)  sideThrusters_.emitSideDown(ship);
    else if (!moving) sideThrusters_.coast();
}

void GameRenderer::resetThrusters() {
    rearThrusters_.reset();
    sideThrusters_.reset();
}
