#include <iostream>
#include <thread>
#include <atomic>
#include <list>
#include <random>
#include <functional>
#include <algorithm>
#include "rwlocker.hpp"

using std::chrono_literals::operator""ms;

static inline uint64_t xorshift64(uint64_t val)
{
	val ^= val << 13;
	val ^= val >> 7;
	val ^= val << 17;
	return val;
}

const long long int seed = 289352958;
const size_t thread_count = 100;
const size_t list_size = 1000;
const size_t string_mult = 100;


void readsome(std::list<std::string> &data, rwl::rwlocker<> &lock) {
    rwl::unique_read_lock ulock(lock);
    int val = seed;
    for (auto& el : data) {
        std::string cmp;
        for (size_t i = 0; i < string_mult; i++) {
            cmp += std::to_string(val = xorshift64(val));
        }
        if (el != cmp) {
            std::cout << "данные не сходятся" << std::endl;
        }
    }
}

void writesome(std::list<std::string> &data, rwl::rwlocker<> &lock) {
    rwl::unique_write_lock ulock(lock);
    int val = seed;
    for (auto& el : data) {
        el.clear();
        for (size_t i = 0; i < string_mult; i++) {
            el += std::to_string(val = xorshift64(val));
        }
    }
}

int main(int argc, char const *argv[])
{
    std::list<std::string> data_array;
    rwl::rwlocker lock;

    auto boolrand = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());

    data_array.assign(list_size, "");
    writesome(data_array, lock);

    std::list<std::thread> run;
    while (run.size() <= thread_count) {
        switch (boolrand()) {
        case 0:
            run.push_back(std::thread(readsome, std::ref(data_array), std::ref(lock)));
            break;
        case 1:
            run.push_back(std::thread(writesome, std::ref(data_array), std::ref(lock)));
            break;
        }
    }
    
    for (auto & th : run) {
        th.join();
    }

    readsome(data_array, lock);

    return 0;
}
