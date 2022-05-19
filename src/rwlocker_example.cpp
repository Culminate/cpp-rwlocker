/**
 * \file rwlocker_example.cpp
 * \brief Пример применения rwlocker
 * Создан длинный список из строк. Во множетсве потоков заполняются и сверяются строки в списке.
 * Без использования класса rwlocker данные перемешиваются и валидацию не проходят.
 * В этом можно убедиться если включить define EXAMPLE_RWLOCKER_DISABLE.
 */

#include <iostream>
#include <thread>
#include <atomic>
#include <list>
#include <random>
#include <functional>
#include <algorithm>
#include "rwlocker.hpp"

const long long int seed  = EXAMPLE_SEED;
const size_t thread_count = EXAMPLE_THREAD_COUNT;
const size_t list_size    = EXAMPLE_LIST_SIZE;
const size_t string_mult  = EXAMPLE_STRING_MULT;

using std::chrono_literals::operator""ms;

/** Шифтер для генерации числа */
static inline uint64_t xorshift64(uint64_t val)
{
	val ^= val << 13;
	val ^= val >> 7;
	val ^= val << 17;
	return val;
}

/** Функция чтения */
void readsome(std::list<std::string> &data, rwl::rwlocker<> &lock) {
#ifndef EXAMPLE_RWLOCKER_DISABLE
    rwl::unique_read_lock ulock(lock);
#endif
    int val = seed;
    for (auto& el : data) {
        std::string cmp;
        for (size_t i = 0; i < string_mult; i++) {
            cmp += std::to_string(val = xorshift64(val));
        }
        if (el != cmp) {
            std::cerr << "данные не сходятся" << std::endl;
        }
    }
}

/** Функция записи */
void writesome(std::list<std::string> &data, rwl::rwlocker<> &lock) {
#ifndef EXAMPLE_RWLOCKER_DISABLE
    rwl::unique_write_lock ulock(lock);
#endif
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
