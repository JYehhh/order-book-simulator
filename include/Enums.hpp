#pragma once
#include <cstdint>

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