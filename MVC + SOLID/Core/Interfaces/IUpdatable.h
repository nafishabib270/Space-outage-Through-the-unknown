#pragma once
// ISP: update contract only — no rendering, no input
class IUpdatable {
public:
    virtual void update(float dt) = 0;
    virtual ~IUpdatable() = default;
};
