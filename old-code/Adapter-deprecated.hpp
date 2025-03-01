#pragma once
#include <unordered_set>
#include "OrderBook.hpp"
#include <nlohmann/json.hpp>
#include "Types.hpp"
#include "IObserver.hpp"

enum ErrorCode {
    SUCCESS,
    ERROR,
};

struct OrderBookRequest {
    UserId user_id;
    std::string json_string;

    OrderBookRequest(const UserId uid, const std::string json) : user_id(uid), json_string(std::move(json)) {}
};

struct OrderBookResponse {
    std::string print_string;
    ErrorCode ec;
};

class Adapter {
    public:
    Adapter() = default;

    void establish_listener(UserId uid, std::shared_ptr<IObserver> user_session);

    OrderBookResponse process_request(OrderBookRequest &req);

    private:
    OrderBook ob_;
    std::unordered_map<UserId, std::shared_ptr<IObserver>> listeners;    // Maps UserId -> UserSession
    std::unordered_map<OrderId, UserId> order_to_user;       // Maps OrderId -> UserId
    std::unordered_map<UserId, std::unordered_set<OrderId>> user_orders; // Maps UserId -> Orders
    
    std::string set_to_string(std::unordered_set<OrderId> &set);

    void notify_trade_execution(FilledTrades trades);
};