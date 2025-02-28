#pragma once
#include "OrderBook.hpp"
#include <nlohmann/json.hpp>

enum ErrorCode {
    SUCCESS,
    ERROR,
};

struct Response {
    std::string print_string;
    ErrorCode ec;
};

class Adapter {
    Response process_params(std::vector<std::string> &json_params) {
        return Response();
    }

    // adapter.process_params();
        // will handle the add order logic, argument validation
        // call the different orderbook commands and return a response object based on if they raise an exception
        // will interpret the orderbook return objects (filled trades) etc... and send a response back to the server.
    // will also implement an subject to which the server is a listener - store a map of all the uids
    // order ID to user mapping will be interpreted and stored here!
};