#include "ResourceManager.h"
#include <stdexcept>

ResourceManager::ResourceManager() { hitSounds_.resize(HIT_SOUND_COUNT); }
ResourceManager::~ResourceManager() { unload(); }

void ResourceManager::load() {
    if (loaded_) return;
    // All assets are in an "assets/" subfolder next to the executable.
    spaceship_  = LoadTexture("assets/spaceship1.png");
    background_ = LoadTexture("assets/Map1.png");
    enemy_      = LoadTexture("assets/enemy.png");
    bullet_     = LoadTexture("assets/bullet_1.png");
    hitEffect_  = LoadTexture("assets/hiteffect.png");
    astronaut_  = LoadTexture("assets/Char_Robot.png");
    alien_      = LoadTexture("assets/Alien1.png");

    bgm_   = LoadMusicStream("assets/background_music_1.mp3");
    laser_ = LoadSound("assets/laser-gun-81720.mp3");
    for (auto& s : hitSounds_) s = LoadSound("assets/hit_sound.wav");

    loaded_ = true;
}

void ResourceManager::unload() {
    if (!loaded_) return;
    UnloadTexture(spaceship_);
    UnloadTexture(background_);
    UnloadTexture(enemy_);
    UnloadTexture(bullet_);
    UnloadTexture(hitEffect_);
    UnloadTexture(astronaut_);
    UnloadTexture(alien_);
    UnloadMusicStream(bgm_);
    UnloadSound(laser_);
    for (auto& s : hitSounds_) UnloadSound(s);
    loaded_ = false;
}
