#pragma once
#include "raylib.h"
#include <vector>

// VIEW — SRP: particle emission and rendering only. No game state.
struct ThrusterParticle {
    Vector2 pos{};  float radius{ 0 };
    float life{ 0 }; Color color{ GRAY }; bool active{ false };
};

class ThrusterEffect {
public:
    explicit ThrusterEffect(int poolSize = 100);
    void emitRear     (Rectangle ship);
    void emitSideUp   (Rectangle ship);
    void emitSideDown (Rectangle ship);
    void coast();
    void render() const;
    void reset();
private:
    std::vector<ThrusterParticle> pool_;
    void emit(Vector2 origin);
    void age();
};
