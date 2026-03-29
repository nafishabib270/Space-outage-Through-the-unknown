#pragma once

/*
 * SessionStats.h
 * --------------
 * SRP: One class, one job — track per-session game statistics.
 *      Score, health, and hit power are the ONLY members. No rendering,
 *      no physics, no entity knowledge.
 *
 * OCP: New stats (e.g. combo multiplier) are added as new members
 *      with new methods here. Nothing else changes.
 *
 * DIP: CollisionSystem receives SessionStats& and calls registerKill()
 *      or damagePlayer() — it depends on this interface, not on raw
 *      ints buried in a game loop.
 */

class SessionStats {
public:
    static constexpr int MAX_HEALTH  = 100;
    static constexpr int BASE_DAMAGE = 10;

    SessionStats();

    void reset();

    void  registerKill();     // Increments score; boosts hit power every 10 kills.
    void  damageSpaceship(int amount);
    void  damageAstronaut(int amount);

    int   getScore()             const { return score_; }
    int   getSpaceshipHealth()   const { return spaceshipHealth_; }
    int   getAstronautHealth()   const { return astronautHealth_; }
    int   getHitPower()          const { return hitPower_; }
    int   getLevel()             const { return level_; }
    void  setLevel(int lvl)            { level_ = lvl; }

    bool  isSpaceshipDestroyed() const { return spaceshipHealth_ <= 0; }
    bool  isAstronautDead()      const { return astronautHealth_  <= 0; }

private:
    int score_           { 0 };
    int spaceshipHealth_ { MAX_HEALTH };
    int astronautHealth_ { MAX_HEALTH };
    int hitPower_        { BASE_DAMAGE };
    int level_           { 0 };
};
