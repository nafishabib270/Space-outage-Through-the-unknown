#pragma once
#include "raylib.h"
#include <vector>
#include <string>

/*
 * ResourceManager — SRP + DIP + RAII
 * ------------------------------------
 * Single Responsibility : The ONLY class that calls LoadTexture /
 *   UnloadTexture / LoadSound / UnloadSound / LoadMusicStream.
 * Dependency Inversion  : Every other class receives Texture2D / Sound
 *   through its constructor — never through ResourceManager directly.
 * RAII                  : Destructor calls unload() automatically.
 *   Double-free is structurally impossible (loaded_ guard).
 */
class ResourceManager {
public:
    static constexpr int HIT_SOUND_COUNT = 20;

    ResourceManager();
    ~ResourceManager();

    ResourceManager(const ResourceManager&)            = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    void load();    // Call once after InitWindow + InitAudioDevice
    void unload();  // Called automatically by destructor

    Texture2D getSpaceship()  const { return spaceship_; }
    Texture2D getBackground() const { return background_; }
    Texture2D getEnemy()      const { return enemy_; }
    Texture2D getBullet()     const { return bullet_; }
    Texture2D getHitEffect()  const { return hitEffect_; }
    Texture2D getAstronaut()  const { return astronaut_; }
    Texture2D getAlien()      const { return alien_; }

    Music&              getBGM()       { return bgm_; }
    Sound               getLaser()     const { return laser_; }
    std::vector<Sound>& getHitSounds() { return hitSounds_; }

private:
    Texture2D spaceship_{};
    Texture2D background_{};
    Texture2D enemy_{};
    Texture2D bullet_{};
    Texture2D hitEffect_{};
    Texture2D astronaut_{};
    Texture2D alien_{};

    Music              bgm_{};
    Sound              laser_{};
    std::vector<Sound> hitSounds_;
    bool               loaded_{ false };
};
