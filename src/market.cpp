#include <iostream>
#include <deque>

#include <boost/asio.hpp>

class Market {

    // markets will contain methods to interact with securities
        // it will hold a list of securities
        // methods:
            // place order
            // cancel order

    // SECURITY:
        // variables: securities will hold an orderbook
        // method: recalculate trading price
        // attempt_order_match: maybe done everytime an order is placed on the market 
    
    // ORDER MATCHING ENGINE:
        // strategy pattern??
        // should this be in a class? maybe a static class?

    // ORDERBOOKS:
        // will hold a set of orders (of different types)
        // do multiple types of orders exist on the same orderbook? - yeah I think so

};