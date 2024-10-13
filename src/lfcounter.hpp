#include <atomic>

struct LFCounter {
    //int * firstCounter, secondCounter;
    std::atomic<unsigned int>* memoryOne, *memoryTwo;

    std::atomic<bool> isOneMaxed = std::atomic_bool(false);

    LFCounter(std::atomic<unsigned int>& mem1, std::atomic<unsigned int>& mem2) {
        memoryOne = &mem1;
        memoryTwo = &mem2;
    }

    unsigned long long get();
};
