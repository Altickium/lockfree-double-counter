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
        auto second = memoryTwo->load();
        auto first = memoryOne->load();
        while(first != std::numeric_limits<uint32_t>::max() && !memoryOne->compare_exchange_strong(first, first + 1));
        if (first == std::numeric_limits<uint32_t>::max() - 1) {
            first++;
            (*memoryTwo)++;
            if (memoryOne->compare_exchange_strong(first, 0)) {
                return convertInts(second, first);
            }
        } else if (first == std::numeric_limits<uint32_t>::max()){
            while(second != std::numeric_limits<uint32_t>::max() && !memoryTwo->compare_exchange_strong(second, second + 1));
            return convertInts(second, first);
        }
        if (!memoryTwo->compare_exchange_strong(second, second)) {
            continue;
        }
        return convertInts(second, first);
    }
}