#include "Adapter.hpp"

void Adapter::establish_listener(UserId uid, std::shared_ptr<IObserver> user_session) {
    listeners[uid] = user_session;
}

OrderBookResponse Adapter::process_request(OrderBookRequest &req) {
    OrderBookResponse res;
    res.ec = ErrorCode::SUCCESS;

    std::string json_params = req.json_string;
    UserId user_id = req.user_id;

    nlohmann::json params = nlohmann::json::parse(json_params);
    try {
        if (params["command"] == "ADD") {
            FilledTrades trades_executed = ob_.add_order(params["side"], params["price"], OrderType::LIMIT, params["quantity"]);
            notify_trade_execution(trades_executed);
            res.print_string = "Successfully Added Order!";
        } else if (params["command"] == "CANCEL") {
            ob_.cancel_order(params["order_id"]);
            res.print_string = "Successfully Cancelled Order!";
        } else if (params["command"] == "MODIFY") {
            FilledTrades trades_executed = ob_.modify_order(params["order_id"], params["new_price"], params["new_side"], OrderType::LIMIT, params["new_quantity"]);
            notify_trade_execution(trades_executed);
            res.print_string = "Successfully Modified Order!";
        } else if (params["command"] == "DISPLAY") {
            res.print_string = ob_.display_levels();
        } else if (params["command"] == "ORDERS") {
            res.print_string = set_to_string(user_orders.at(user_id));
        } else {
            throw std::invalid_argument("CRITICAL-ERROR: Command sent from client not found!");
        }
    } catch (const std::exception &e) {
        res.ec = ErrorCode::ERROR;
        res.print_string = e.what();
    }
    
    return res;
}

std::string Adapter::set_to_string(std::unordered_set<OrderId> &set) {
    std::string res = "";
    for (const OrderId &order : set) {
        res += std::to_string(order) + " ";
    }
    return res;
}

void Adapter::notify_trade_execution(FilledTrades trades) {
    for (FilledTrade &filled : trades) {
        listeners.at(filled.getOrderExecution().buyside)->update("Your buy trade has been executed!");
        listeners.at(filled.getOrderExecution().sellside)->update("Your sell trade has been executed!");
    }
}