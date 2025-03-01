#pragma once
#include <cstdint>
#include "Types.hpp"

class OrderIdGenerator {
public:
    static OrderId next_id();
};