#include "lfcounter.hpp"

#include <atomic>
#include <thread>
#include <cassert>

const unsigned long MAX_THREADS = 256;

unsigned long long convertInts(unsigned int first, unsigned int second) {
    unsigned long long firstPart = first;
    return (firstPart << 32) + second;
}

unsigned long long LFCounter::get() {
    unsigned int second = -1, first = -1;
    while (second != UINT32_MAX) {
        second = memoryTwo -> load(), first = memoryOne->load();
        while (first != UINT32_MAX && !memoryOne->compare_exchange_strong(first, first + 1)) {
        }
        if (first == UINT32_MAX) {
            size_t tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
            if (tid % MAX_THREADS == 0) {
                memoryTwo->compare_exchange_strong(second, second + 1);
                memoryOne->compare_exchange_strong(first, 0);
                memoryOne->notify_all();
                return convertInts(second, first);
            }
            memoryOne->wait(UINT32_MAX);
        } else {
            return convertInts(second, first);
        }
    }
    second = memoryTwo->load();
    first = memoryOne->load();
    while (first != UINT32_MAX && !memoryOne->compare_exchange_strong(first, first + 1));
    return convertInts(second, first);
}