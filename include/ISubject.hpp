// ISubject.hpp
#pragma once
#include "Types.hpp"
#include <string>

class IObserver;  // Forward declaration

class ISubject {
public:
    virtual ~ISubject() = default;
    virtual void attach(UserId user_id, IObserver* observer) = 0;
    virtual void notify(UserId user_id, const std::string& message) = 0;
};