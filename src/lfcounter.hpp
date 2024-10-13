#include <atomic>

struct LFCounter {
    //int * firstCounter, secondCounter;
    std::atomic_uint32_t *memoryOne, *memoryTwo;

    std::atomic_bool isOneMaxed = std::atomic_bool(false);

    LFCounter(std::atomic_uint32_t & mem1, std::atomic_uint32_t & mem2) {
        memoryOne = &mem1;
        memoryTwo = &mem2;
    }

    unsigned long long get();
};
