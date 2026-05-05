#include "model/SessionStats.h"

void SessionStats::reset() {
    score_           = 0;
    spaceshipHealth_ = MAX_HEALTH;
    astronautHealth_ = MAX_HEALTH;
    hitPower_        = BASE_DAMAGE;
    level_           = 0;
}

void SessionStats::registerKill() {
    ++score_;
    if (score_ % 10 == 0) hitPower_ += 10;
}

void SessionStats::damageSpaceship(int amount) {
    spaceshipHealth_ = std::max(0, spaceshipHealth_ - amount);
}

void SessionStats::damageAstronaut(int amount) {
    astronautHealth_ = std::max(0, astronautHealth_ - amount);
}
