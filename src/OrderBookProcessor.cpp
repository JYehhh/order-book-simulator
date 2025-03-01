#include "OrderBookProcessor.hpp"

OrderBookProcessor::OrderBookProcessor() : notifier_(std::make_unique<TradeNotifier>()) {}

void OrderBookProcessor::register_user(UserId user_id, IObserver* observer) {
    notifier_->attach(user_id, observer);
}

OrderPtr OrderBookProcessor::create_order(UserId user_id, Side side, Price price, OrderType order_type, Quantity quantity) {
    auto order = std::make_shared<Order>(side, price, order_type, quantity);
    order_to_user[order->getOrderId()] = user_id;
    user_orders[user_id].insert(order->getOrderId());
    return order;
}

Response OrderBookProcessor::process_request(const Request& req) {
    Response res;
    res.ec = ErrorCode::SUCCESS;

    std::string json_params = req.json_string;
    UserId user_id = req.user_id;

    try {
        nlohmann::json params = OrderJsonDecoder::decode_request(json_params);

        if (params["command"] == "ADD") {
            Side side = params["side"];
            Price price = params["price"];
            Quantity quantity = params["quantity"];
            OrderType order_type = OrderType::LIMIT;

            OrderPtr order = create_order(user_id, side, price, order_type, quantity);

            FilledTrades trades_executed = ob_.add_order(order);
            notify_trade_execution(trades_executed);
            res.print_string = "Successfully Added Order!";
        } 
        else if (params["command"] == "CANCEL") {
            OrderId order_id = params["order_id"];

            ob_.cancel_order(order_id);

            user_orders[user_id].erase(order_id);

            order_to_user.erase(order_id);

            res.print_string = "Successfully Cancelled Order!";
        } 
        else if (params["command"] == "MODIFY") {
            OrderId old_order_id = params["order_id"];
            Price new_price = params["new_price"];
            Side new_side = params["new_side"];
            Quantity new_quantity = params["new_quantity"];
            OrderType new_order_type = OrderType::LIMIT;

            OrderPtr new_order = create_order(user_id, new_side, new_price, new_order_type, new_quantity);

            FilledTrades trades_executed = ob_.modify_order(old_order_id, new_order);
            notify_trade_execution(trades_executed);

            user_orders[user_id].erase(old_order_id);

            order_to_user.erase(old_order_id);

            res.print_string = "Successfully Modified Order!";
        } 
        else if (params["command"] == "DISPLAY") {
            res.print_string = ob_.display_levels();
        } 
        else if (params["command"] == "ORDERS") {
            res.print_string = set_to_string(user_orders.at(user_id));
        } 
        else {
            throw std::invalid_argument("CRITICAL-ERROR: Command sent from client not found!");
        }
    } catch (const std::exception &e) {
        res.ec = ErrorCode::ERROR;
        res.print_string = e.what();
    }
    
    return res;
}

std::string OrderBookProcessor::set_to_string(std::unordered_set<OrderId> &set) {
    std::string res = "";
    for (const OrderId &order : set) {
        res += std::to_string(order) + " ";
    }
    return res;
}

void OrderBookProcessor::notify_trade_execution(FilledTrades trades) {
    for (FilledTrade &filled : trades) {
        notifier_->notify(order_to_user.at(filled.getOrderExecution().buyside), "Your buy trade has been executed!");
        notifier_->notify(order_to_user.at(filled.getOrderExecution().sellside), "Your sell trade has been executed");
    }
}

