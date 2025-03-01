#pragma once

#include <cstdint>

using UserId = uint32_t;

enum ErrorCode {
    SUCCESS,
    ERROR,
};

struct Request {
    UserId user_id;
    std::string json_string;

    Request(const UserId uid, const std::string json) : user_id(uid), json_string(std::move(json)) {}
};

struct Response {
    std::string print_string;
    ErrorCode ec;
};