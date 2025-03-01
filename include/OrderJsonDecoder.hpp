// OrderJsonDecoder.hpp
#pragma once
#include <nlohmann/json.hpp>
#include <stdexcept>
#include "Types.hpp"

class OrderJsonDecoder {
public:
    static nlohmann::json decode_request(const std::string& json_string);
private:
    static void validate_add_order(const nlohmann::json& json);
    static void validate_cancel_order(const nlohmann::json& json);
    static void validate_modify_order(const nlohmann::json& json);
    static void validate_display_order([[maybe_unused]] const nlohmann::json& json);
    static void validate_user_orders([[maybe_unused]] const nlohmann::json& json);
};
