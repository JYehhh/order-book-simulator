#include "OrderJsonDecoder.hpp"

nlohmann::json OrderJsonDecoder::decode_request(const std::string& json_string) {
    nlohmann::json json;

    try {
        // Parse the JSON string
        json = nlohmann::json::parse(json_string);

        // Validate the JSON based on the command
        std::string command = json.at("command");
        if (command == "ADD") {
            validate_add_order(json);
        } else if (command == "CANCEL") {
            validate_cancel_order(json);
        } else if (command == "MODIFY") {
            validate_modify_order(json);
        } else if (command == "DISPLAY") {
            validate_display_order(json);
        } else if (command == "ORDERS") {
            validate_user_orders(json);
        } else {
            throw std::invalid_argument("Unknown command. Valid commands: ADD, CANCEL, MODIFY, DISPLAY, ORDERS.");
        }
    } catch (const nlohmann::json::parse_error& e) {
        throw std::invalid_argument("Invalid JSON format: " + std::string(e.what()));
    } catch (const nlohmann::json::out_of_range& e) {
        throw std::invalid_argument("Missing required field in JSON: " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw std::invalid_argument("Validation error: " + std::string(e.what()));
    }

    return json;
}

void OrderJsonDecoder::validate_add_order(const nlohmann::json& json) {
    if (!json.contains("side") || !json.contains("price") || !json.contains("quantity")) {
        throw std::invalid_argument("Invalid ADD order: missing required fields.");
    }

    std::string side = json.at("side");
    if (side != "BUY" && side != "SELL") {
        throw std::invalid_argument("Invalid side in ADD order. Expected: BUY or SELL.");
    }

    Price price = json.at("price");
    if (price <= 0) {
        throw std::invalid_argument("Price in ADD order must be a positive number.");
    }

    Quantity quantity = json.at("quantity");
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity in ADD order must be a positive number.");
    }
}

void OrderJsonDecoder::validate_cancel_order(const nlohmann::json& json) {
    if (!json.contains("order_id")) {
        throw std::invalid_argument("Invalid CANCEL order: missing order_id.");
    }

    OrderId order_id = json.at("order_id");
    if (order_id < 0) {
        throw std::invalid_argument("Order ID in CANCEL order must be a positive number.");
    }
}

void OrderJsonDecoder::validate_modify_order(const nlohmann::json& json) {
    if (!json.contains("order_id") || !json.contains("new_price") || !json.contains("new_side") || !json.contains("new_quantity")) {
        throw std::invalid_argument("Invalid MODIFY order: missing required fields.");
    }

    OrderId order_id = json.at("order_id");
    if (order_id < 0) {
        throw std::invalid_argument("Order ID in MODIFY order must be a positive number.");
    }

    Price new_price = json.at("new_price");
    if (new_price <= 0) {
        throw std::invalid_argument("New price in MODIFY order must be a positive number.");
    }

    std::string new_side = json.at("new_side");
    if (new_side != "BUY" && new_side != "SELL") {
        throw std::invalid_argument("Invalid new side in MODIFY order. Expected: BUY or SELL.");
    }

    Quantity new_quantity = json.at("new_quantity");
    if (new_quantity <= 0) {
        throw std::invalid_argument("New quantity in MODIFY order must be a positive number.");
    }
}

void OrderJsonDecoder::validate_display_order([[maybe_unused]] const nlohmann::json& json) {
    // No additional fields to validate for DISPLAY command
}

void OrderJsonDecoder::validate_user_orders([[maybe_unused]] const nlohmann::json& json) {
    // No additional fields to validate for ORDERS command
}