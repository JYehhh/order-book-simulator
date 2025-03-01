#pragma once
#include "Types.hpp"
#include <cstdint>
#include <vector>

struct OrderExecution {
    OrderId buyside;
    OrderId sellside;
};

class FilledTrade {
public:
    FilledTrade(Quantity& quantity, Price& price);
    Quantity getQuantity() const;
    Price getPrice() const;
    OrderExecution getOrderExecution() const;

private:
    Quantity quantity_;
    Price price_;
    OrderExecution orders_executed_;
};

using FilledTrades = std::vector<FilledTrade>;