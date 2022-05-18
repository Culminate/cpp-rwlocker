#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_COLORS_ANSI
#include <doctest/doctest.h>

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <thread>
#include <random>
#include <list>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

#include "rwlocker.hpp"

using std::chrono_literals::operator""ms;

struct pass_data {
    rwl::rwlocker<> lock;
    std::atomic_int readcount = 0;
    std::atomic_int writecount = 0;
};

void readfunc(pass_data& data, std::chrono::milliseconds timeout) {
    rwl::unique_read_lock ulock(data.lock);
    data.readcount++;
    CHECK(data.writecount == 0);
    if (timeout != 0ms) {
        std::this_thread::sleep_for(timeout);
    }
    data.readcount--;
}

void writefunc(pass_data& data, std::chrono::milliseconds timeout) {
    rwl::unique_write_lock ulock(data.lock);
    data.writecount++;
    CHECK(data.writecount == 1);
    CHECK(data.readcount == 0);
    if (timeout != 0ms) {
        std::this_thread::sleep_for(timeout);
    }
    data.writecount--;
}

std::chrono::milliseconds mrand() {
    auto timerand = std::bind(std::uniform_int_distribution<>(10,100),std::default_random_engine());
    return std::chrono::milliseconds(timerand());
}

auto boolrand = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());

TEST_CASE("testing write/read collisions") {
    pass_data test_data;

    std::list<std::thread> run;
    while (run.size() <= 10000) {
        if (boolrand()) {
            run.push_back(std::thread(readfunc, std::ref(test_data), 0ms));
        } else {
            run.push_back(std::thread(writefunc, std::ref(test_data), 0ms));
        }
    }
    
    for (auto & th : run) {
        th.join();
    }
}

TEST_CASE("testing write/read collisions with timeout") {
    pass_data test_data;

    std::list<std::thread> run;
    while (run.size() <= 10000) {
        if (boolrand()) {
            run.push_back(std::thread(readfunc, std::ref(test_data), mrand()));
        } else {
            run.push_back(std::thread(writefunc, std::ref(test_data), mrand()));
        }
    }
    
    for (auto & th : run) {
        th.join();
    }
}