#ifndef SECURITY_HPP
#define SECURITY_HPP

#include <string>
#include "orderbook.hpp"

class Security {
public:
    explicit Security(const std::string& symbol);
    void add_order(const Order& order);
    void process_matching();

private:
    std::string symbol_;
    OrderBook order_book_;
};

#endif