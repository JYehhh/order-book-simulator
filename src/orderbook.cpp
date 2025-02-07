#include <iostream>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <memory>
#include <unordered_map>

enum class OrderType
{
    Limit,
    Market,
    Stop,
};

enum class Side
{
    Buy,
    Sell
};

using Price = int32_t;
using Quantity = uint32_t;
using OrderId = uint64_t;

struct LevelInfo
{
    Price price_;
    Quantity quantity_;
};

using LevelInfos = std::vector<LevelInfo>;

class OrderBookLevelInfos 
{
public:
    OrderBookLevelInfos(LevelInfos &bids, LevelInfos &asks)
    : bids_(bids)
    , asks_(asks)
    {};

    LevelInfos getBids() const { return bids_; }
    LevelInfos getAsks() const { return asks_; }

private:
    LevelInfos bids_;
    LevelInfos asks_;
};

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

class Order 
{
public:
    Order (Price &price, Side &side, OrderType &order_type, Quantity &quantity_initial) 
    : price_(price)
    , side_(side)
    , order_type_(order_type)
    , quantity_initial_(quantity_initial)
    , quantity_filled_(0)
    , quantity_remaining_(quantity_initial)
    {};

    Price getPrice() const { return price_; }
    Side getSide() const { return side_; }
    OrderType getOrderType() const { return order_type_; }
    Quantity getQuantityInitial() const { return quantity_initial_; }
    Quantity getQuantityFilled() const { return quantity_filled_; }
    Quantity getQuantityRemaining() const { return quantity_remaining_; }

    void fill(Quantity quantity_to_fill) {
        if (quantity_to_fill > quantity_remaining_) {
            throw std::runtime_error("Exception: not enough quantity left in order to fill!");
        } else {
            quantity_filled_ += quantity_to_fill;
            quantity_remaining_ -= quantity_to_fill;
        }
    }

private:
    Price price_;
    Side side_;
    OrderType order_type_;
    Quantity quantity_initial_;
    Quantity quantity_filled_;
    Quantity quantity_remaining_;
};

class OrderBook 
{ 
public:
    OrderBook();

    FilledTrades add_order();
    void cancel_order();
    FilledTrades modify_order();

    OrderBookLevelInfos get_levels() const;
    OrderBookLevelInfos display_levels() const;

private:
    // ideally can we not have this break the open closed principal?
    // I'll implement limits first, and then lets see if it can be fixed.
    FilledTrades match();


    // list because random access is possible and easy with the other set
    // list also because iterators remain valid when adding and removing
    // list also because removal of elements is O(1) rather than O(n)
    std::map<Price, std::list<std::shared_ptr<Order>>, std::greater<>> bids_;
    std::map<Price, std::list<std::shared_ptr<Order>>, std::less<>> asks_;

    // The sorting of the price levels ensures TOP status
    // appending to the end of the list ensures FIFO status
    // pro-rata needs to be implemented itself

    using OrderPosition = std::pair<
        std::map<Price, std::list<std::shared_ptr<Order>>>::iterator,  // Price Level Iterator
        std::list<std::shared_ptr<Order>>::iterator                    // Order List Iterator
    >;

    std::unordered_map<OrderId, OrderPosition> order_lookup_;
};


int main()
{
    return 0;
}