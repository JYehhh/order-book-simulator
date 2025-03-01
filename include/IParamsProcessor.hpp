// ParamsProcessor.hpp
#pragma once
#include "Types.hpp"

class IParamsProcessor {
public:
    virtual ~IParamsProcessor() = default;
    virtual Response process_request(const Request& request) = 0;
};