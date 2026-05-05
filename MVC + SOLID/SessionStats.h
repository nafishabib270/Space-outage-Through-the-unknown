#pragma once
#include <algorithm>

/*
 * SessionStats — MODEL — SRP
 * ---------------------------
 * Owns all per-session numeric state: score, health, hit power, level.
 * No rendering, no physics. One reason to change: scoring rules.
 *
 * MVC: Pure Model data. View reads it (const ref). Controller mutates
 *      it through methods (registerKill, damageSpaceship, etc.).
 */
class SessionStats {
public:
    static constexpr int MAX_HEALTH  = 100;
    static constexpr int BASE_DAMAGE = 10;

    void reset();

    void registerKill();
    void damageSpaceship(int amount);
    void damageAstronaut(int amount);

    int  getScore()            const { return score_; }
    int  getSpaceshipHealth()  const { return spaceshipHealth_; }
    int  getAstronautHealth()  const { return astronautHealth_; }
    int  getHitPower()         const { return hitPower_; }
    int  getLevel()            const { return level_; }
    void setLevel(int l)             { level_ = l; }

    bool spaceshipDestroyed()  const { return spaceshipHealth_ <= 0; }
    bool astronautDead()       const { return astronautHealth_  <= 0; }

private:
    int score_           { 0 };
    int spaceshipHealth_ { MAX_HEALTH };
    int astronautHealth_ { MAX_HEALTH };
    int hitPower_        { BASE_DAMAGE };
    int level_           { 0 };
};
