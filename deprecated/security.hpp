#ifndef SECURITY_HPP
#define SECURITY_HPP

#include <string>
#include "orderbook.hpp"

class Security {
public:
    explicit Security(const std::string& symbol);

    void add_order(const Order& order);
    // void match_orders();

private:
    std::string symbol_;
    OrderBook order_book_;
};

#endif