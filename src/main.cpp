#include "lfcounter.hpp"

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>

std::atomic<int> atomic_count{0};
 
std::mutex cout_mutex;
int completed_writes{0};
 
constexpr int global_max_count{72};
constexpr int writes_per_line{8};
constexpr int max_delay{100};
 
template<int Max>
int random_value()
{
    static std::uniform_int_distribution<int> distr{1, Max};
    static std::random_device engine;
    static std::mt19937 noise{engine()};
    return distr(noise);
}
 
int main()
{   
    std::atomic<unsigned int> mem1, mem2;
    LFCounter lf(mem1, mem2);

    auto work = [&lf](const std::string id)
    {
        for (int count{}; (count = ++atomic_count) <= global_max_count;)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(random_value<max_delay>()));
 
            {
 
                const bool new_line = ++completed_writes % writes_per_line == 0;
 
                std::cout << id << std::setw(3) << " " << lf.get() << " "
                          << (new_line ? "\n" : "") << std::flush;
            }
        }
    };

    std::jthread j1(work, "░"), j2(work, "▒"), j3(work, "▓"), j4(work, "█");

}