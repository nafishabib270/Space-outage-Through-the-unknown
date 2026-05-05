#pragma once
#include "raylib.h"
// ISP: collision contract only — no rendering, no spawning
class ICollidable {
public:
    virtual Rectangle getBounds()        const = 0;
    virtual bool      isAlive()          const = 0;
    virtual float     getHealth()        const = 0;
    virtual void      takeDamage(float amount) = 0;
    virtual void      kill()                   = 0;
    virtual ~ICollidable() = default;
};
