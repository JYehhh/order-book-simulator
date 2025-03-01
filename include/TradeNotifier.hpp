// TradeNotifier.hpp
#pragma once
#include "ISubject.hpp"
#include <unordered_map>

class TradeNotifier : public ISubject {
public:
    void attach(UserId user_id, IObserver* observer) override;
    void notify(UserId user_id, const std::string& message) override;

private:
    std::unordered_map<UserId, IObserver*> users_;
};