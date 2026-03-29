#pragma once

#include "raylib.h"
#include <vector>
#include <string>

/*
 * ResourceManager.h
 * -----------------
 * SRP: The single class responsible for loading, owning, and unloading
 *      every raylib asset (textures, sounds, music). No other class
 *      calls LoadTexture() or UnloadTexture().
 *
 * DIP: All other classes receive their textures and sounds through
 *      their constructors. They depend on data types (Texture2D, Sound),
 *      never on ResourceManager itself. ResourceManager is only used
 *      by Game to build the system graph.
 *
 * OCP: New assets are added by adding new members and loading them in
 *      load() — existing assets are untouched.
 */

class ResourceManager {
public:
    static constexpr int HIT_SOUND_COUNT = 20;

    ResourceManager();
    ~ResourceManager();

    // No copy — resources are owned uniquely.
    ResourceManager(const ResourceManager&)            = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    void load();
    void unload();

    // Asset accessors — return by value (raylib handles are small structs).
    Texture2D getSpaceship()    const { return spaceship_; }
    Texture2D getBackground()   const { return background_; }
    Texture2D getEnemy()        const { return enemy_; }
    Texture2D getAlien()        const { return alien_; }
    Texture2D getAstronaut()    const { return astronaut_; }
    Texture2D getBullet()       const { return bullet_; }
    Texture2D getHitEffect()    const { return hitEffect_; }

    Music&               getBGM()      { return bgm_; }
    Sound                getLaser()    const { return laser_; }
    std::vector<Sound>&  getHitSounds()      { return hitSounds_; }

private:
    Texture2D          spaceship_{};
    Texture2D          background_{};
    Texture2D          enemy_{};
    Texture2D          alien_{};
    Texture2D          astronaut_{};
    Texture2D          bullet_{};
    Texture2D          hitEffect_{};

    Music              bgm_{};
    Sound              laser_{};
    std::vector<Sound> hitSounds_;

    bool               loaded_{ false };
};
