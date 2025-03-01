#ifndef USER_HPP
#define USER_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include "order.hpp"

struct Holding {
    std::string symbol;
    int quantity;  // Aggregated stock count
};

class User {
public:
    User(std::string id, double balance);
    // update
    void add_holding(const std::string& security, int quantity);
    void remove_holding(const std::string& security, int quantity);
    void update_balance(const int new_bal);
    
    // query
    double get_balance() const { return balance_; }
    void adjust_balance(double amount) { balance_ += amount; }
    
    const std::unordered_map<std::string, Holding>& get_holdings() const { return holdings_; }
    const std::vector<std::shared_ptr<Order>>& get_active_orders() const { return active_orders_; }

private:
    std::string user_id_;
    double balance_;
    std::unordered_map<std::string, Holding> holdings_;
    std::vector<std::shared_ptr<Order>> active_orders_;
};

#endif // USER_HPP