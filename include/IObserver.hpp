// Listener.hpp
#pragma once

#include <string>

class IObserver {
public:
    virtual ~IObserver() = default;

    // Pure virtual function to handle trade execution notifications
    virtual void update(const std::string &message) = 0;

    // Pure virtual function to handle other types of notifications (if needed)
    // virtual void on_other_event(UserId user_id, SomeData data) = 0;
};