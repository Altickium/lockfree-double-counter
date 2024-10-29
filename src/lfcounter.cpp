#include "lfcounter.hpp"

#include <atomic>
#include <thread>
#include <cassert>

const unsigned long MAX_THREADS = 256;
unsigned int MAIN_BYTES = 8388607;     // 2 ^ 23 - 1

unsigned long long convertInts(unsigned int first, unsigned int second) {
    unsigned long long firstPart = (first << 22) >> 22;
    unsigned long long secondPart = second;
    return (firstPart << 32) + secondPart;
}

unsigned long long LFCounter::get() {
    unsigned int gen1 = (memoryTwo -> load() >> 20);
    unsigned int second = (memoryTwo -> load() << 22) >> 22;
    unsigned int gen3 = ((memoryTwo -> load() << 12) >> 22);
    unsigned int gen33 = ((memoryTwo -> load() << 12) >> 22);
    unsigned int first = memoryOne->load();
    unsigned int gen4 = ((memoryTwo -> load() << 12) >> 22);
    unsigned int gen2 = (memoryTwo -> load() >> 20);
    do {
        gen33 = ((memoryTwo -> load() << 12) >> 22);
        gen1 = (memoryTwo -> load() >> 20);
        first = 1 + memoryOne->fetch_add(1);
        if (first == 0) {
            gen1++;
            second += gen1 << 20;
            if (memoryTwo->compare_exchange_strong(second, second + 1)) {
                do {
                    gen3 = ((memoryTwo -> load() << 12) >> 22);
                    if (memoryOne->compare_exchange_strong(first, 0)) {
                        gen3++;
                        uint32_t flg = (gen1 << 22) + (gen3 << 10) + second + 1;
                        memoryTwo->compare_exchange_strong(flg, flg);
                    }
                    gen4 = ((memoryTwo -> load() << 12) >> 22);
                } while(gen3 != gen4);
            }
        }
        gen2 = (memoryTwo -> load() >> 20);
        gen4 = ((memoryTwo -> load() << 12) >> 22);
    } while (gen1 != gen2 && gen33 != gen4);
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