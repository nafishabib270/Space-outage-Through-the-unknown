#pragma once

/*
 * IUpdatable.h
 * ------------
 * ISP: Separates update logic from rendering. Entities that update
 * but don't draw (e.g. a ScoreTracker) can implement only this
 * interface without being forced to implement draw().
 *
 * LSP: Any IUpdatable can be stored in a collection and updated
 * uniformly without knowing its concrete type.
 */

class IUpdatable {
public:
    virtual void update(float dt) = 0;
    virtual ~IUpdatable() = default;
};
