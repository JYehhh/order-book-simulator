#ifndef MARKET_HPP
#define MARKET_HPP

#include <unordered_map>
#include <memory>
#include <string>
#include "security.hpp"

class Market {
public:
    Market();

    void create_security(const std::string& symbol);
    void place_order(const std::string& symbol);
    void match_orders(const std::string& symbol);

private:
    std::unordered_map<std::string, std::shared_ptr<Security>> securities_;
};

#endif