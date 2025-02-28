#pragma once
#include <cstdint>
#include "Enums.hpp"

class OrderIdGenerator {
public:
    static OrderId next_id();
};