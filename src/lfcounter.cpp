#include "lfcounter.hpp"

#include <atomic>

unsigned long long LFCounter::get() {
    unsigned int first = memoryOne->load();
    while (first != UINT32_MAX && memoryOne->compare_exchange_weak(first, first + 1,std::memory_order_release, std::memory_order_relaxed)) {

    };
    
    if (first != UINT32_MAX) {
        return first;
    }
    
    //First bool change
    bool isFirstMaxed = isOneMaxed.load();
    while (isOneMaxed.compare_exchange_weak(isFirstMaxed, true, std::memory_order_release, std::memory_order_relaxed));
    if (!isFirstMaxed) {
        return first;
    }

    unsigned int second = memoryTwo->load();
    while (second != UINT32_MAX && memoryOne->compare_exchange_weak(second, second + 1,std::memory_order_release, std::memory_order_relaxed)) {

    };

    long long convertedFirst = first;
    return (unsigned long long) (convertedFirst << 32) + second;
}