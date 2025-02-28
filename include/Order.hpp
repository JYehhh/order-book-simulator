#pragma once
#include "Enums.hpp"
#include "OrderIdGenerator.hpp"
#include <memory>
#include <list>

class Order {
public:
    Order(Side& side, Price& price, OrderType& order_type, Quantity& quantity_initial);
    
    OrderId getOrderId() const;
    Price getPrice() const;
    Side getSide() const;
    OrderType getOrderType() const;
    Quantity getQuantityInitial() const;
    Quantity getQuantityFilled() const;
    Quantity getQuantityRemaining() const;
    bool fill(Quantity quantity_to_fill);

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