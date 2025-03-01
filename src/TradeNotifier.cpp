#include "TradeNotifier.hpp"
#include "IObserver.hpp"

void TradeNotifier::attach(UserId user_id, IObserver* observer) {
    users_[user_id] = observer;
}

void TradeNotifier::notify(UserId user_id, const std::string& message) {
    users_.at(user_id)->update(message);
}