#include "OrderIdGenerator.hpp"
#include <chrono>
#include <atomic>

OrderId OrderIdGenerator::next_id() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    static std::atomic<uint32_t> counter(0);
    return (timestamp << 16) | (counter.fetch_add(1) & 0xFFFF);
}