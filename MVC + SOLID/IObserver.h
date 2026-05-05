#pragma once
// MVC + DIP: View registers as an observer of Model events.
// Model never imports View — it calls onModelChanged() through this abstraction.
class IObserver {
public:
    virtual void onModelChanged() = 0;
    virtual ~IObserver() = default;
};
