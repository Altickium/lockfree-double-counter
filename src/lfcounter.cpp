#include "lfcounter.hpp"

#include <atomic>
#include <thread>
#include <cassert>

const unsigned long MAX_THREADS = 256;
unsigned int MAIN_BYTES = 8388607;     // 2 ^ 23 - 1

unsigned long long convertInts(unsigned int first, unsigned int second) {
    unsigned long long firstPart = (first << 10) >> 10;
    unsigned long long secondPart = (second << 10) >> 10;
    return (firstPart << 32) + secondPart;
}

unsigned long long LFCounter::get() {
    unsigned int second = (memoryTwo -> load() << 10) >> 10, first = (memoryOne->load() << 10) >> 10;
    while (second != MAIN_BYTES) {
        while (first == MAIN_BYTES) {
            if (tryIncrement()) {
                return convertInts(second, MAIN_BYTES);
            }
            first = (memoryOne->load() << 10) >> 10;
        } 
        second = memoryTwo -> load();
        while (first != MAIN_BYTES && !memoryOne->compare_exchange_strong(first, first + 1)) { }
        if (first != MAIN_BYTES) {
            if (memoryTwo->load() == second) {
                return convertInts(second, first);
            }
        }
    }
    return convertInts(second, first);
}

bool LFCounter::tryIncrement() {
    size_t tid = std::hash<std::thread::id>{}(std::this_thread::get_id()) % 1024;
    unsigned int first = ((memoryOne -> load() << 10) >> 10) + \
                          (tid << 22);   
    memoryOne -> exchange(first);
    if (memoryOne->load() >> 22 == tid) {
        unsigned int second = ((memoryTwo -> load() << 10) >> 10) + \
                          (tid << 22);   
        memoryTwo -> exchange(second);
        if (memoryTwo->load() >> 22 == tid) {
            if (memoryTwo->compare_exchange_strong(second, second + 1) && memoryOne->compare_exchange_strong(first, 0)) {
                return true;
            }
        }
    }
    return false;
}