#include "ThrusterEffect.h"

ThrusterEffect::ThrusterEffect(int poolSize)
    : pool_(poolSize)
{}

void ThrusterEffect::emit(Vector2 origin) {
    for (auto& p : pool_) {
        if (p.active) continue;
        p.pos    = origin;
        p.radius = static_cast<float>(GetRandomValue(2, 6));
        p.life   = 1.0f;
        p.color  = GRAY;
        p.active = true;
        return;   // Emit one particle at a time.
    }
}

void ThrusterEffect::ageAll() {
    for (auto& p : pool_) {
        if (!p.active) continue;
        p.pos.x -= 3.0f;
        p.life  -= 0.025f;
        if (p.radius > 0) p.radius -= 0.1f;
        if (p.life  <= 0) p.active  = false;
    }
}

void ThrusterEffect::emitRear(Rectangle ship) {
    for (int i = 0; i < 2; ++i) {
        Vector2 origin{
            ship.x + 3.0f,
            ship.y + ship.height / 2.0f +
                static_cast<float>(GetRandomValue(-10, 10))
        };
        emit(origin);
    }
    // Rear particles drift along with the exhaust.
    for (auto& p : pool_) {
        if (!p.active) continue;
        p.pos.y = ship.y + ship.height / 2.0f +
                  static_cast<float>(GetRandomValue(-10, 10));
    }
    ageAll();
}

void ThrusterEffect::emitSideUp(Rectangle ship) {
    for (int i = 0; i < 2; ++i) {
        Vector2 origin{
            ship.x + ship.width / 5.0f,
            ship.y + ship.height / 2.0f +
                static_cast<float>(GetRandomValue(-10, 10))
        };
        emit(origin);
    }
    for (auto& p : pool_) {
        if (!p.active) continue;
        p.pos.x -= 3.0f;
        p.pos.y += 3.0f;   // Exhaust drifts downward when going up.
    }
    ageAll();
}

void ThrusterEffect::emitSideDown(Rectangle ship) {
    for (int i = 0; i < 2; ++i) {
        Vector2 origin{
            ship.x + ship.width / 5.0f,
            ship.y + ship.height / 2.0f +
                static_cast<float>(GetRandomValue(-10, 10))
        };
        emit(origin);
    }
    for (auto& p : pool_) {
        if (!p.active) continue;
        p.pos.x -= 3.0f;
        p.pos.y -= 3.0f;   // Exhaust drifts upward when going down.
    }
    ageAll();
}

void ThrusterEffect::coast() {
    for (auto& p : pool_) {
        if (!p.active) continue;
        p.pos.x -= 3.0f;
    }
    ageAll();
}

void ThrusterEffect::update(float /*dt*/) {
    // Particle lifecycle is managed per-emission-mode.
    // This satisfies IUpdatable for systems that call update generically.
}

void ThrusterEffect::draw() const {
    for (const auto& p : pool_) {
        if (!p.active || p.radius <= 0) continue;
        Color c = p.color;
        c.a     = static_cast<unsigned char>(p.life * 255.0f);
        DrawCircleV(p.pos, p.radius, c);
    }
}

void ThrusterEffect::reset() {
    for (auto& p : pool_) p.active = false;
}
