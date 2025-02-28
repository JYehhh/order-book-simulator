#include <iostream>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <atomic>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>

#define SPREAD_COLOUR 33
#define ASK_COLOUR 31
#define BID_COLOUR 32

enum class OrderType
{
    LIMIT,
    MARKET,
    // STOP,
};

enum class Side
{
    BUY,
    SELL
};

using Price = int32_t;
using Quantity = uint32_t;
using OrderId = uint64_t;

class FilledTrade 
{
public:
    FilledTrade(Quantity &quantity, Price &price)
    : quantity_(quantity)
    , price_(price)
    {};

    Quantity getQuantity() const { return quantity_; }
    Price getPrice() const { return price_; }

private:
    Quantity quantity_;
    Price price_;
};

using FilledTrades = std::vector<FilledTrade>;

class OrderIdGenerator {
public:
    static OrderId next_id() {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
        static std::atomic<uint32_t> counter(0);  // Counter resets every millisecond

        return (timestamp << 16) | (counter.fetch_add(1) & 0xFFFF);
    }
};

class Order 
{
public:
    Order (Side &side, Price &price, OrderType &order_type, Quantity &quantity_initial) 
    : order_id_(OrderIdGenerator::next_id())
    , price_(price)
    , side_(side)
    , order_type_(order_type)
    , quantity_initial_(quantity_initial)
    , quantity_remaining_(quantity_initial)
    {};

    OrderId getOrderId() const { return order_id_; }
    Price getPrice() const { return price_; }
    Side getSide() const { return side_; }
    OrderType getOrderType() const { return order_type_; }
    Quantity getQuantityInitial() const { return quantity_initial_; }
    Quantity getQuantityFilled() const { return quantity_initial_ - quantity_remaining_; }
    Quantity getQuantityRemaining() const { return quantity_remaining_; }

    bool fill(Quantity quantity_to_fill) {
        if (quantity_to_fill > quantity_remaining_) {
            throw std::logic_error("Logic Error: not enough quantity left in order to fill!");
        } else {
            quantity_remaining_ -= quantity_to_fill;
        }

        return quantity_remaining_ == 0;
    }

private:
    OrderId order_id_;
    Price price_;
    Side side_;
    OrderType order_type_;
    Quantity quantity_initial_;
    Quantity quantity_remaining_;
};

using OrderPtr = std::shared_ptr<Order>;
using OrderPtrList = std::list<OrderPtr>;

class OrderBook 
{ 
public:
    OrderBook() {};

    FilledTrades add_order(Side side, Price price, OrderType order_type, Quantity quantity_initial) {
        // create order
        OrderPtr order = std::make_shared<Order>(side, price, order_type, quantity_initial);

        // add order to order book
        if (side == Side::BUY) {
            bids_levels_[price] += quantity_initial;
            bids_[price].push_back(order);
            order_lookup_[order->getOrderId()] = {bids_.find(price), --bids_[price].end()}; //(logn)
        } else {
            asks_levels_[price] += quantity_initial;
            asks_[price].push_back(order);
            order_lookup_[order->getOrderId()] = {asks_.find(price), --asks_[price].end()}; //(logn)
        }

        return match();
    }

    void cancel_order(OrderId order_id) {
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

    // could make this atomic with inplace modification
    // right now, we cause fifo loss, and also, may impede hft performance due to the lack of atomicity
    FilledTrades modify_order(OrderId old_order_id, Price new_price, Side new_side, OrderType new_order_type, Quantity new_quantity_initial) {
        if (!order_lookup_.count(old_order_id)) {
            throw std::runtime_error("Runtime Error: OrderID not Found!");
        }
        cancel_order(old_order_id);

        add_order(new_side, new_price, new_order_type, new_quantity_initial);

        return match();
    }

    std::string display_levels() const {
        std::stringstream ss;

        // Helper function to generate a block string for a given quantity
        auto generate_blocks = [](Quantity quantity) {
            const int block_size = 5; // Each block represents 5 units

            std::string res = "";
            for (int i = 0; i < quantity / block_size ; ++i) {
                res += "▒";
            }
            return res;
        };

        // Get the best bid and ask prices
        Price best_bid = bids_levels_.empty() ? 0 : bids_levels_.begin()->first;
        Price best_ask = asks_levels_.empty() ? 0 : asks_levels_.begin()->first;
        Price mid_price = (best_bid + best_ask) / 2;

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

private:
    // ideally can we not have this break the open closed principal?
    // I'll implement limits first, and then lets see if it can be fixed.
    FilledTrades match() { // time complexity should only be O(n) where n is number of orders
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

    // list because random access is possible and easy with the other set
    // list also because iterators remain valid when adding and removing
    // list also because removal of elements is O(1) rather than O(n)
    std::map<Price, OrderPtrList, std::greater<>> bids_;
    std::map<Price, OrderPtrList, std::less<>> asks_;
    std::map<Price, Quantity, std::greater<>> bids_levels_;
    std::map<Price, Quantity, std::less<>> asks_levels_;

    // The sorting of the price levels ensures TOP status
    // appending to the end of the list ensures FIFO status
    // pro-rata needs to be implemented itself

    using OrderPosition = std::pair<
        std::map<Price, OrderPtrList>::iterator,  // Price Level Iterator
        OrderPtrList::iterator                    // Order List Iterator
    >;

    std::unordered_map<OrderId, OrderPosition> order_lookup_;
};


int main()
{
    OrderBook ob = OrderBook();

    ob.add_order(Side::BUY, 100, OrderType::LIMIT, 20);
    ob.add_order(Side::BUY, 101, OrderType::LIMIT, 12);
    ob.add_order(Side::SELL, 103, OrderType::LIMIT, 8);
    ob.add_order(Side::SELL, 105, OrderType::LIMIT, 14);

    std::cout << ob.display_levels();

    return 0;
}

// BRAINSTORMING DESIGN PATTERINS
// Factory Pattern for creating orders
// Strategy Pattern for inserting strategies
// Observer Pattern for notifying clients