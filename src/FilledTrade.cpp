#include "FilledTrade.hpp"

FilledTrade::FilledTrade(Quantity& quantity, Price& price)
    : quantity_(quantity), price_(price) {}

Quantity FilledTrade::getQuantity() const { return quantity_; }
Price FilledTrade::getPrice() const { return price_; }
OrderExecution FilledTrade::getOrderExecution() const { return orders_executed_; }