#pragma once

#include "raylib.h"

/*
 * ICollidable.h
 * -------------
 * ISP: Classes that participate in collision queries expose only the
 * minimum required — their bounding box and alive state.
 *
 * DIP: The CollisionSystem depends on this abstraction, not on
 * concrete Enemy or Alien classes.
 *
 * LSP: Any ICollidable (Enemy, Alien, Spaceship) can be passed to the
 * CollisionSystem interchangeably and behave correctly.
 */

class ICollidable {
public:
    virtual Rectangle getBounds()  const = 0;
    virtual bool      isAlive()    const = 0;
    virtual float     getHealth()  const = 0;
    virtual void      takeDamage(float amount) = 0;
    virtual void      kill() = 0;
    virtual ~ICollidable() = default;
};
