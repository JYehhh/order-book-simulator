#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <queue>
#include <vector>
#include "order.hpp"

class OrderBook {
public:
    void add_order(const Order& order);
    // void match_orders();

private:
    std::priority_queue<Order> buy_orders_;
    std::priority_queue<Order> sell_orders_;
};

#endif