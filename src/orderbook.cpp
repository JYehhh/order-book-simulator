#include "OrderBook.hpp"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

#define SPREAD_COLOUR 33
#define ASK_COLOUR 31
#define BID_COLOUR 32

OrderBook::OrderBook() = default;

FilledTrades OrderBook::add_order(const OrderPtr& order) {
    Price price = order->getPrice();
    Side side = order->getSide();
    Quantity quantity_initial = order->getQuantityInitial();

    // Add order to order book
    if (side == Side::BUY) {
        bids_levels_[price] += quantity_initial;
        bids_[price].push_back(order);
        order_lookup_[order->getOrderId()] = {bids_.find(price), --bids_[price].end()};
    } else {
        asks_levels_[price] += quantity_initial;
        asks_[price].push_back(order);
        order_lookup_[order->getOrderId()] = {asks_.find(price), --asks_[price].end()};
    }

    return match();
}

void OrderBook::cancel_order(OrderId order_id) {
    // check if the order exists
    // honestly might not need to do this if we're calling .at()
    if (!order_lookup_.count(order_id)) {
        throw std::runtime_error("Runtime Error: OrderID not Found!");
    }

    // get the price and order iterators
    OrderPosition order_pos = order_lookup_.at(order_id);
    auto price_it = order_pos.first;
    auto order_it = order_pos.second;

    // extract the price level, side, and the list of orders at that price
    auto &orders_at_price = price_it->second;
    Price price = price_it->first;
    Quantity quantity_remaining = (*order_it)->getQuantityRemaining();
    Side side = (*order_it)->getSide();

    // erase order out of list.
    orders_at_price.erase(order_it);

    // if the list is now empty, remove the price level.
    if (side == Side::BUY) {
        bids_levels_.at(price) -= quantity_remaining;
        if (orders_at_price.empty()) bids_.erase(price);
    } else {
        asks_levels_.at(price) -= quantity_remaining;
        if (orders_at_price.empty()) asks_.erase(price);
    }

    // remove the order from the position lookups.
    order_lookup_.erase(order_id);
}

FilledTrades OrderBook::modify_order(OrderId old_order_id, const OrderPtr& new_order) {
    if (!order_lookup_.count(old_order_id)) {
        throw std::runtime_error("Runtime Error: OrderID not Found!");
    }

    cancel_order(old_order_id);
    return add_order(new_order);

    return match();
}

std::string OrderBook::display_levels() const {
    std::stringstream ss;

    // Helper function to generate a block string for a given quantity
    auto generate_blocks = [](Quantity quantity) {
        const Quantity block_size = 5; // Each block represents 5 units

        std::string res = "";
        for (int i = 0; i < static_cast<int>(quantity / block_size) ; ++i) {
            res += "▒";
        }
        return res;
    };

    // Get the best bid and ask prices
    Price best_bid = bids_levels_.empty() ? 0 : bids_levels_.begin()->first;
    Price best_ask = asks_levels_.empty() ? 0 : asks_levels_.begin()->first;

    ss << "==============================\n";

    // Display ASKS
    for (const auto &[price, quantity] : asks_levels_) {
        ss << "\t\033[1;" << ASK_COLOUR << "m" << "$" << std::setw(6) << std::fixed << std::setprecision(2) 
            << price << std::setw(5) << quantity << "\033[0m ";
        ss << generate_blocks(quantity) << "\n";
    }

    // Display Bid ask spread
    ss << "\n\033[1;" << SPREAD_COLOUR << "m======  " << 10000 * (best_ask-best_bid)/best_bid << "bps  ======\033[0m\n\n";

    // Display BIDS
    for (const auto &[price, quantity] : bids_levels_) {
        ss << "\t\033[1;" << BID_COLOUR << "m" << "$" << std::setw(6) << std::fixed << std::setprecision(2)
            << price << std::setw(5) << quantity << "\033[0m ";
            ss << generate_blocks(quantity) << "\n";
    }
    ss << "==============================\n";
    return ss.str();
}

FilledTrades OrderBook::match() {
    FilledTrades trades;

    Price best_bid = bids_levels_.empty() ? 0 : bids_levels_.begin()->first;
    Price best_ask = asks_levels_.empty() ? 0 : asks_levels_.begin()->first;

    while (best_bid >= best_ask) {
        // get the first bid and first ask orders, create trade object
        Order &bid_order = *(bids_.at(best_bid).front());
        Order &ask_order = *(asks_.at(best_ask).front());

        // resting order always goes first
        Price trade_price = bid_order.getOrderId() < ask_order.getOrderId() ? bid_order.getPrice() : ask_order.getPrice();
        Quantity trade_quantity = std::min(bid_order.getQuantityRemaining(), ask_order.getQuantityRemaining());

        trades.emplace_back(trade_quantity, trade_price); 

        // update volumes
        if (bid_order.fill(trade_quantity)) {
            bids_[best_bid].pop_front();
            if (bids_[best_bid].empty()) bids_.erase(best_bid);
        }

        if (ask_order.fill(trade_quantity)) {
            asks_[best_ask].pop_front();
            if (asks_[best_ask].empty()) asks_.erase(best_ask);
        }

        bids_levels_.at(best_bid) -= trade_quantity;
        asks_levels_.at(best_ask) -= trade_quantity;

        // Recalculate spread
        best_bid = bids_levels_.empty() ? 0 : bids_levels_.begin()->first;
        best_ask = asks_levels_.empty() ? 0 : asks_levels_.begin()->first;
    }

    return trades;
}