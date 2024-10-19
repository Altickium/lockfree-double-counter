#include "lfcounter.hpp"

#include <atomic>

unsigned long long convertInts(unsigned int first, unsigned int second) {
    unsigned long long firstPart = first;
    return (firstPart << 32) + second;
}

unsigned long long LFCounter::get() {
    unsigned int second = memoryTwo->load(), first = memoryOne->load();
    while (second != UINT32_MAX) {
        first = memoryOne->load();
        while (first != UINT32_MAX && !memoryOne->compare_exchange_strong(first, first + 1)) {
            second = memoryTwo -> load();
        };

        if (first == UINT32_MAX) {
            if (memoryTwo->compare_exchange_strong(second, second + 1)) {
                return convertInts(second, first);
            }
            memoryOne->compare_exchange_strong(first, 0);
        } else if (first != UINT32_MAX) {
            return convertInts(second, first);
        }
    };
    first = memoryOne->load();
    while (first != UINT32_MAX && !memoryOne->compare_exchange_strong(first, first + 1));
    return convertInts(second, first);
}