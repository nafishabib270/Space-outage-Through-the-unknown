#include "ResourceManager.h"
#include <stdexcept>

ResourceManager::ResourceManager() { hitSounds_.resize(HIT_SOUND_COUNT); }
ResourceManager::~ResourceManager() { unload(); }

Sound ResourceManager::getLaserAlias() {
    if (laserAliases_.empty()) return laser_;
    Sound alias = laserAliases_[laserAliasIdx_];
    laserAliasIdx_ = (laserAliasIdx_ + 1) % static_cast<int>(laserAliases_.size());
    return alias;
}

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
    laserAliases_.reserve(LASER_ALIAS_COUNT);
    for (int i = 0; i < LASER_ALIAS_COUNT; ++i) {
        laserAliases_.push_back(LoadSoundAlias(laser_));
    }
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
    for (auto& s : laserAliases_) UnloadSoundAlias(s);
    laserAliases_.clear();
    UnloadSound(laser_);
    for (auto& s : hitSounds_) UnloadSound(s);
    loaded_ = false;
}
