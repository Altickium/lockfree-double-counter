#include "lfcounter.hpp"

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <cassert>

std::atomic_ullong atomic_count{0};

int completed_writes{0};
 
constexpr unsigned long long global_max_count{1000};
 
int main()
{   
    std::atomic_uint32_t mem1(0), mem2(0);
    LFCounter lf(mem1, mem2);
    bool* checker = new bool[global_max_count]{false};
    auto work = [&lf, &checker]()
    {
        for (size_t count{}; (count = ++atomic_count) < global_max_count;)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            {
                unsigned long long val = lf.get();
                checker[val] = true; 
            }
        }
    };

    std::jthread j1(work), j2(work), j3(work), j4(work);
    j1.join();
    j2.join();
    j3.join();
    j4.join();
    for (size_t i = 1; i<global_max_count - 1; ++i) {
        assert(checker[i]);
    }
    delete[] checker;
}