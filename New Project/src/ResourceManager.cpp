#include "ResourceManager.h"

ResourceManager::ResourceManager() {
    hitSounds_.resize(HIT_SOUND_COUNT);
}

ResourceManager::~ResourceManager() {
    unload();
}

void ResourceManager::load() {
    if (loaded_) return;

    spaceship_  = LoadTexture("spaceship1.png");
    background_ = LoadTexture("Map1.png");
    enemy_      = LoadTexture("enemy.png");
    alien_      = LoadTexture("Alien1.png");
    astronaut_  = LoadTexture("Char_Robot.png");
    bullet_     = LoadTexture("bullet_1.png");
    hitEffect_  = LoadTexture("hiteffect.png");

    bgm_   = LoadMusicStream("background_music_1.mp3");
    laser_ = LoadSound("laser-gun-81720.mp3");

    for (auto& s : hitSounds_) s = LoadSound("hit_sound.wav");

    loaded_ = true;
}

void ResourceManager::unload() {
    if (!loaded_) return;

    UnloadTexture(spaceship_);
    UnloadTexture(background_);
    UnloadTexture(enemy_);
    UnloadTexture(alien_);
    UnloadTexture(astronaut_);
    UnloadTexture(bullet_);
    UnloadTexture(hitEffect_);

    UnloadMusicStream(bgm_);
    UnloadSound(laser_);
    for (auto& s : hitSounds_) UnloadSound(s);

    loaded_ = false;
}
