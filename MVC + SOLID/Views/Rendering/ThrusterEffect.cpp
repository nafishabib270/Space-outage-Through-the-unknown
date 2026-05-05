#include "ThrusterEffect.h"

ThrusterEffect::ThrusterEffect(int s) : pool_(s) {}

void ThrusterEffect::emit(Vector2 o) {
    for (auto& p : pool_) {
        if (p.active) continue;
        p = { o, static_cast<float>(GetRandomValue(2,6)), 1.0f, GRAY, true };
        return;
    }
}

void ThrusterEffect::age() {
    for (auto& p : pool_) {
        if (!p.active) continue;
        p.pos.x -= 3.0f; p.life -= 0.025f;
        if (p.radius > 0) p.radius -= 0.1f;
        if (p.life <= 0)  p.active = false;
    }
}

void ThrusterEffect::emitRear(Rectangle s) {
    for (int i = 0; i < 2; ++i)
        emit({ s.x + 3.0f, s.y + s.height/2.0f + GetRandomValue(-10,10) });
    for (auto& p : pool_) if (p.active) p.pos.y = s.y + s.height/2.0f + GetRandomValue(-10,10);
    age();
}

void ThrusterEffect::emitSideUp(Rectangle s) {
    for (int i = 0; i < 2; ++i)
        emit({ s.x + s.width/5.0f, s.y + s.height/2.0f + GetRandomValue(-10,10) });
    for (auto& p : pool_) if (p.active) { p.pos.x -= 3; p.pos.y += 3; }
    age();
}

void ThrusterEffect::emitSideDown(Rectangle s) {
    for (int i = 0; i < 2; ++i)
        emit({ s.x + s.width/5.0f, s.y + s.height/2.0f + GetRandomValue(-10,10) });
    for (auto& p : pool_) if (p.active) { p.pos.x -= 3; p.pos.y -= 3; }
    age();
}

void ThrusterEffect::coast() {
    for (auto& p : pool_) if (p.active) p.pos.x -= 3;
    age();
}

void ThrusterEffect::render() const {
    for (const auto& p : pool_) {
        if (!p.active || p.radius <= 0) continue;
        Color c = p.color; c.a = static_cast<unsigned char>(p.life * 255);
        DrawCircleV(p.pos, p.radius, c);
    }
}

void ThrusterEffect::reset() { for (auto& p : pool_) p.active = false; }
