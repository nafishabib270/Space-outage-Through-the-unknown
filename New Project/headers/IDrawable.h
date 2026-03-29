#pragma once

/*
 * IDrawable.h
 * -----------
 * ISP: Clients that only draw should not be forced to depend on
 * update or collision interfaces. This interface is kept intentionally
 * minimal — one method, one concern.
 *
 * DIP: High-level systems (renderers, game loop) depend on this
 * abstraction, never on concrete types like Enemy or Alien.
 */

class IDrawable {
public:
    virtual void draw() const = 0;
    virtual ~IDrawable() = default;
};
