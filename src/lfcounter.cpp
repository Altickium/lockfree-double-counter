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
    unsigned int second = memoryTwo -> load(), first = first = memoryOne->load();
    while (true) {
        unsigned int nfirst = first + 1, nsecond = second;
        if (first == UINT32_MAX) {
            nfirst = 0;
            nsecond = second + 1;
        }
        if (!memoryTwo->compare_exchange_strong(second, nsecond)) {
            continue;
        }
        if (!memoryOne->compare_exchange_strong(first, nfirst)) {
            continue;
        }
        if (memoryTwo->compare_exchange_strong(second, second)) {
            return convertInts(nsecond, nfirst);
        }
    }
}