#pragma once

/*
 * ISpawnable.h
 * ------------
 * ISP: Only classes that need to be spawned and despawned implement
 * this interface. It is not bundled into a mega-interface with draw
 * or update, keeping implementations focused.
 *
 * OCP: The SpawnSystem depends on this abstraction. New spawnable
 * entity types can be added (implement ISpawnable) without modifying
 * the SpawnSystem.
 */

class ISpawnable {
public:
    virtual void spawn(float x, float y, float speed) = 0;
    virtual void despawn() = 0;
    virtual bool isActive() const = 0;
    virtual ~ISpawnable() = default;
};
