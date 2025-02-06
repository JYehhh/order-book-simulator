#ifndef MARKET_HPP
#define MARKET_HPP

#include <unordered_map>
#include <memory>
#include <string>
#include "security.hpp"
#include "user.hpp"

class Market {
public:
    // Constructor
    Market();

    // Creation and Deletion
    void create_security(const std::string& symbol);
    void create_user(const std::string &user_id);
    void create_order(const std::string& symbol);

    void delete_security(const std::string& symbol);
    void delete_user(const std::string &user_id);
    void delete_order(const std::string& symbol);

    // Query
    

private:
    // void match_orders(const std::string& symbol);
    std::unordered_map<std::string, std::shared_ptr<Security>> securities_;
    std::unordered_map<std::string, std::shared_ptr<User>> users_;
};

#endif