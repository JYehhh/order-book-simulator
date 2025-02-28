#include "Order.hpp"
#include <stdexcept>

Order::Order(Side& side, Price& price, OrderType& order_type, Quantity& quantity_initial)
    : order_id_(OrderIdGenerator::next_id()),
      price_(price),
      side_(side),
      order_type_(order_type),
      quantity_initial_(quantity_initial),
      quantity_remaining_(quantity_initial) {}

OrderId Order::getOrderId() const { return order_id_; }
Price Order::getPrice() const { return price_; }
Side Order::getSide() const { return side_; }
OrderType Order::getOrderType() const { return order_type_; }
Quantity Order::getQuantityInitial() const { return quantity_initial_; }
Quantity Order::getQuantityFilled() const { return quantity_initial_ - quantity_remaining_; }
Quantity Order::getQuantityRemaining() const { return quantity_remaining_; }

bool Order::fill(Quantity quantity_to_fill) {
    if (quantity_to_fill > quantity_remaining_) 
        throw std::logic_error("Logic Error: Not enough quantity to fill!");
    quantity_remaining_ -= quantity_to_fill;
    return quantity_remaining_ == 0;
}