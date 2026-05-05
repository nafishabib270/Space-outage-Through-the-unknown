#pragma once
// ISP: rendering contract only — no update, no game logic
class IRenderable {
public:
    virtual void render() const = 0;
    virtual ~IRenderable() = default;
};
