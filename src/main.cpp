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
 
constexpr unsigned long long global_max_count{1000 * 1000};
//constexpr int writes_per_line{8};
//constexpr int max_delay{2};
 
/*template<int Max>
int random_value()
{
    static std::uniform_int_distribution<int> distr{1, Max};
    static std::random_device engine;
    static std::mt19937 noise{engine()};
    return distr(noise);
}*/
 
int main()
{   
    std::atomic_uint32_t mem1, mem2;
    LFCounter lf(mem1, mem2);
    bool* checker = new bool[global_max_count]{false};
    auto work = [&lf, &checker]()
    {
        for (size_t count{}; (count = ++atomic_count) < global_max_count;)
        {
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
    for (size_t i = 0; i<global_max_count - 1; ++i) {
        assert(checker[i]);
    }
    delete[] checker;
}