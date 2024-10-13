#include "lfcounter.hpp"

#include <atomic>

unsigned long long LFCounter::get() {
    unsigned int first = memoryOne->load();
    while (first != UINT32_MAX && !memoryOne->compare_exchange_strong(first, first + 1)) {

    };
    
    if (first != UINT32_MAX) {
        return first;
    }
    
    //First bool change
    bool isFirstMaxed = isOneMaxed.load();
    while (!isOneMaxed.compare_exchange_strong(isFirstMaxed, true));
    if (!isFirstMaxed) {
        return first;
    }

    unsigned int second = memoryTwo->load();
    while (second != UINT32_MAX && !memoryOne->compare_exchange_strong(second, second + 1)) {

    };

    long long convertedFirst = first;
    return (unsigned long long) (convertedFirst << 32) + second;
}