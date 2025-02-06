#ifndef ORDER_HPP
#define ORDER_HPP

#include <string>
#include <order_type.hpp>


class Order {
public:
    Order(std::string symbol, int quantity, OrderSide side)
        : symbol_(std::move(symbol)), quantity_(quantity), side_(side) {}

    virtual ~Order() = default;

    virtual OrderType getType() const = 0;
    virtual double getPrice() const = 0;  // Override in subclasses

    std::string getSymbol() const { return symbol_; }
    int getQuantity() const { return quantity_; }
    OrderSide getSide() const { return side_; }

protected:
    std::string symbol_;
    int quantity_;
    OrderSide side_;
};

class MarketOrder : public Order {
public:
    MarketOrder(std::string symbol, int quantity, OrderSide side)
        : Order(std::move(symbol), quantity, side) {}

    OrderType getType() const override { return OrderType::MARKET; }
    double getPrice() const override { return 0.0; }  // Market orders have no fixed price
};

class LimitOrder : public Order {
public:
    LimitOrder(std::string symbol, int quantity, OrderSide side, double limit_price)
        : Order(std::move(symbol), quantity, side), limit_price_(limit_price) {}

    OrderType getType() const override { return OrderType::LIMIT; }
    double getPrice() const override { return limit_price_; }

private:
    double limit_price_;
};

class StopOrder : public Order {
public:
    StopOrder(std::string symbol, int quantity, OrderSide side, double stop_price)
        : Order(std::move(symbol), quantity, side), stop_price_(stop_price) {}

    OrderType getType() const override { return OrderType::STOP; }
    double getPrice() const override { return stop_price_; }

    bool isTriggered(double market_price) const {
        return (side_ == OrderSide::BUY && market_price >= stop_price_) ||
               (side_ == OrderSide::SELL && market_price <= stop_price_);
    }

private:
    double stop_price_;
};

#endif // ORDER_HPP