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
    unsigned int second = memoryTwo->load();
    unsigned int first = memoryOne->load();
    while (true) {
        first = memoryOne->load();
        second = memoryTwo->load();
        while(first != second && !memoryOne->compare_exchange_strong(first, first + 1));
        if (first == second) {
            if (memoryTwo->compare_exchange_strong(second, second + 1)) {
                if (memoryOne->compare_exchange_strong(second, 0)) {
                    return convertInts(second, first);
                }
            }
        } else {
            return convertInts(second, first);
        }
    }
}