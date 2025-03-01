#include "OrderJsonEncoder.hpp"

std::string OrderJsonEncoder::make_order(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> params;
    std::string p;

    while (iss >> p) params.push_back(p);

    if (params.empty()) {
        std::cerr << "Invalid command. Please enter a valid order.\n";
        throw std::invalid_argument("Invalid command. Please enter a valid order.\n");
    }

    std::string order_type = params[0];
    std::string json_message;

    if (order_type == "ADD") {
        json_message = make_add_order(params);
    } else if (order_type == "CANCEL") {
        json_message = make_cancel_order(params);
    } else if (order_type == "MODIFY") {
        json_message = make_modify_order(params);
    } else if (order_type == "DISPLAY") {
        json_message = make_display_order(params);
    } else if (order_type == "ORDERS") {
        json_message = make_user_orders(params);
    } else {
        throw std::runtime_error("Unknown command. Valid commands: ADD, CANCEL, MODIFY, DISPLAY, ORDERS.");
    }

    return json_message;
}

std::string OrderJsonEncoder::make_add_order(const std::vector<std::string>& params) {
    if (params.size() != 4) {
        throw std::runtime_error("Invalid ADD order format. Expected: ADD BUY/SELL PRICE QUANTITY");
    }

    std::string command = params[0];
    std::string side = params[1];  // BUY or SELL
    Price price = std::stod(params[2]);
    Quantity quantity = std::stoi(params[3]);

    if (price <= 0 || quantity <= 0) {
        throw std::runtime_error("Price and Quantity must be positive numbers.");
    }

    nlohmann::json add_order = {
        {"command", command},
        {"side", side},
        {"price", price},
        {"quantity", quantity},
    };
    return add_order.dump();
}

std::string OrderJsonEncoder::make_cancel_order(const std::vector<std::string>& params) {
    if (params.size() != 2) {
        throw std::runtime_error("Invalid CANCEL order format. Expected: CANCEL ORDER_ID");
    }

    std::string command = params[0];
    OrderId order_id = std::stoi(params[1]);

    if (order_id < 0) {
        throw std::runtime_error("Order ID must be a positive number.");
    }

    nlohmann::json cancel_order = {
        {"command", command},
        {"order_id", order_id},
    };
    return cancel_order.dump();
}

std::string OrderJsonEncoder::make_modify_order(const std::vector<std::string>& params) {
    if (params.size() != 5) {
        throw std::runtime_error("Invalid MODIFY order format. Expected: MODIFY ORDER_ID NEW_PRICE BUY/SELL NEW_QUANTITY");
    }

    std::string command = params[0];
    OrderId order_id = std::stoi(params[1]);
    Price new_price = std::stod(params[2]);
    std::string new_side = params[3];  // BUY or SELL
    Quantity new_quantity = std::stoi(params[4]);

    if (order_id < 0 || new_price <= 0 || new_quantity <= 0) {
        throw std::runtime_error("Order ID must be positive, and Price/Quantity must be greater than zero.");
    }

    nlohmann::json modify_order = {
        {"command", command},
        {"order_id", order_id},
        {"new_side", new_side},
        {"new_price", new_price},
        {"new_quantity", new_quantity},
    };
    return modify_order.dump();
}

std::string OrderJsonEncoder::make_display_order(const std::vector<std::string>& params) {
    if (params.size() != 1) {
        throw std::runtime_error("Invalid DISPLAY command format. Expected: DISPLAY");
    }

    std::string command = params[0];

    nlohmann::json display_order = {
        {"command", command},
    };
    return display_order.dump();
}

std::string OrderJsonEncoder::make_user_orders(const std::vector<std::string>& params) {
    if (params.size() != 1) {
        throw std::runtime_error("Invalid ORDERS command format. Expected: ORDERS");
    }

    std::string command = params[0];

    nlohmann::json user_orders = {
        {"command", command},
    };
    return user_orders.dump();
}