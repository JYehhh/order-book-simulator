// OrderBookProcessor.hpp
#pragma once
#include "Types.hpp"
#include "IParamsProcessor.hpp"
#include "TradeNotifier.hpp"
#include "ISubject.hpp"
#include "OrderBook.hpp"
#include <unordered_map>
#include <unordered_set>

class OrderBookProcessor : public IParamsProcessor {
public:
    OrderBookProcessor();
    void register_user(UserId user_id, IObserver* observer);
    Response process_request(const Request& request) override;

private:
    OrderBook ob_;
    std::unique_ptr<ISubject> notifier_; 
    std::unordered_map<OrderId, UserId> order_to_user;       // Maps OrderId -> UserId
    std::unordered_map<UserId, std::unordered_set<OrderId>> user_orders; // Maps UserId -> Orders
    
    std::string set_to_string(std::unordered_set<OrderId> &set);

    void notify_trade_execution(FilledTrades trades);
};
