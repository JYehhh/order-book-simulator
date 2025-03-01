#pragma once

#include <cstdint>
#include <string>
#include <utility>

using UserId = uint32_t;

enum ErrorCode {
    SUCCESS,
    ERROR,
};

struct Request {
    UserId user_id;
    std::string json_string;

    Request(const UserId uid, const std::string json) : user_id(uid), json_string(std::move(json)) {}
};

struct Response {
    std::string print_string;
    ErrorCode ec;
};

enum class OrderType {
    LIMIT,
    MARKET
};

enum class Side {
    BUY,
    SELL
};

using Price = double;
using Quantity = uint32_t;
using OrderId = uint64_t;