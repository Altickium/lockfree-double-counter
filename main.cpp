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
    static std::mutex rand_mutex;
    std::lock_guard lock{rand_mutex};
    return distr(noise);
}
 
int main()
{
    auto work = [](const std::string id)
    {
        for (int count{}; (count = ++atomic_count) <= global_max_count;)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(random_value<max_delay>()));
 
            // print thread `id` and `count` value
            {
                std::lock_guard lock{cout_mutex};
 
                const bool new_line = ++completed_writes % writes_per_line == 0;
 
                std::cout << id << std::setw(3) << count << "  "
                          << (new_line ? "\n" : "") << std::flush;
            }
        }
    };
 
    std::jthread j1(work, "░"), j2(work, "▒"), j3(work, "▓"), j4(work, "█");
}