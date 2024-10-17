#include <atomic>

unsigned long long convertInts(unsigned int first, unsigned int second);

struct LFCounter {
    std::atomic_uint32_t *memoryOne, *memoryTwo;

    LFCounter(std::atomic_uint32_t & mem1, std::atomic_uint32_t & mem2) {
        memoryOne = &mem1;
        memoryTwo = &mem2;
    }

    unsigned long long get();
};
