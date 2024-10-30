#include "lfcounter.hpp"

#include <atomic>
#include <thread>
#include <cassert>

const unsigned long MAX_THREADS = 256;

unsigned long long convertInts(unsigned int first, unsigned int second) {
    unsigned long long firstPart = first;
    unsigned long long secondPart = second;
    return (firstPart << 32) + secondPart;
}

unsigned long long LFCounter::get() {
    while (true) {
        auto first = memoryOne->load();
        auto second = memoryTwo->load();
        if (first == 0) {
            second = memoryOne->fetch_add(1);
            first = memoryTwo->fetch_add(1);
        }
        while(first != std::numeric_limits<uint32_t>::max() && first != 0 && !memoryOne->compare_exchange_strong(first, first + 1));
        if (first == std::numeric_limits<uint32_t>::max()) {
            if (memoryOne->compare_exchange_strong(first, 0)) {
                return convertInts(second, first);
            }
        } else if (first != 0) {
            return convertInts(second, first);
        }
    }
}