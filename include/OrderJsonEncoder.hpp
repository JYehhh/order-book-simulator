// OrderJsonEncoder.hpp
#pragma once
#include <nlohmann/json.hpp>
#include <sstream>
#include <iostream>
#include "Types.hpp"
#include <vector>
#include <string>
#include <stdexcept>

class OrderJsonEncoder {
public:
    static std::string make_order(const std::string& input);
private:
    static std::string make_add_order(const std::vector<std::string>& params);
    static std::string make_cancel_order(const std::vector<std::string>& params);
    static std::string make_modify_order(const std::vector<std::string>& params);
    static std::string make_display_order(const std::vector<std::string>& params);
    static std::string make_user_orders(const std::vector<std::string>& params);
};