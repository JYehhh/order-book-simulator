#pragma once
#include "Order.hpp"
#include "FilledTrade.hpp"
#include <map>
#include <list>
#include <unordered_map>
#include <string>

class OrderBook {
public:
    OrderBook();
    FilledTrades add_order(Side side, Price price, OrderType order_type, Quantity quantity_initial);
    void cancel_order(OrderId order_id);
    FilledTrades modify_order(OrderId old_order_id, Price new_price, Side new_side, OrderType new_order_type, Quantity new_quantity_initial);
    std::string display_levels() const;

private:
    FilledTrades match();

    // user needs to be able to:
        // get_orders -> that will allow them to cancel order
            // store a map: {user_id, vector<order_ids>}
        // observe the trades that get made that get made
            // observer pattern???? When a trade gets made, queue a message to the client.
            // Some sort of queue
    
    // Object Structure:
        // should I keep the 'FilledTrades' object hidden? 
        // 

    
    std::map<Price, OrderPtrList, std::greater<>> bids_;
    std::map<Price, OrderPtrList, std::less<>> asks_;
    std::map<Price, Quantity, std::greater<>> bids_levels_;
    std::map<Price, Quantity, std::less<>> asks_levels_;

    using OrderPosition = std::pair<
        std::map<Price, OrderPtrList>::iterator,
        OrderPtrList::iterator
    >;
    std::unordered_map<OrderId, OrderPosition> order_lookup_;
};