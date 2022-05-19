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

const std::size_t PASS = 10000;

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
    auto timerand = std::bind(std::uniform_int_distribution<>(1,10),std::default_random_engine());
    return std::chrono::milliseconds(timerand());
}

auto boolrand = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());

TEST_CASE("only read") {
    pass_data test_data;
    std::list<std::thread> run;
    while (run.size() <= PASS) {
        run.push_back(std::thread(readfunc, std::ref(test_data), 0ms));
    }
    
    for (auto & th : run) {
        th.join();
    }
}

TEST_CASE("only read with timeout") {
    pass_data test_data;
    std::list<std::thread> run;
    while (run.size() <= PASS) {
        run.push_back(std::thread(readfunc, std::ref(test_data), mrand()));
    }
    
    for (auto & th : run) {
        th.join();
    }
}

TEST_CASE("only write") {
    pass_data test_data;
    std::list<std::thread> run;
    while (run.size() <= PASS) {
        run.push_back(std::thread(writefunc, std::ref(test_data), 0ms));
    }
    
    for (auto & th : run) {
        th.join();
    }
}

TEST_CASE("only write with timeout") {
    pass_data test_data;
    std::list<std::thread> run;
    while (run.size() <= PASS) {
        run.push_back(std::thread(writefunc, std::ref(test_data), mrand()));
    }
    
    for (auto & th : run) {
        th.join();
    }
}

TEST_CASE("write/read collisions") {
    pass_data test_data;

    std::list<std::thread> run;
    while (run.size() <= PASS) {
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

TEST_CASE("write/read collisions with timeout") {
    pass_data test_data;

    std::list<std::thread> run;
    while (run.size() <= PASS) {
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

enum class t {
    R,
    W
};

TEST_CASE("read/write order") {
    std::list<t> order;
    std::list<std::thread> run;
    std::mutex readlock;
    rwl::rwlocker lock;


    auto read = [&]() {
        rwl::unique_read_lock ulock(lock);
        std::unique_lock ureadlock(readlock);
        order.push_back(t::R);
        std::this_thread::sleep_for(10ms);
    };

    auto write = [&]() {
        rwl::unique_write_lock ulock(lock);
        order.push_back(t::W);
        std::this_thread::sleep_for(10ms);
    };

    for (size_t i = 0; i < 100; i++) {
        run.push_back(std::thread(write));
        std::this_thread::sleep_for(1ms);

        for (size_t i = 0; i < 3; i++) {
            run.push_back(std::thread(read));
        }

        run.push_back(std::thread(write));

        for (auto& th : run) {
            th.join();
        }
        
        std::list<t> trueorder{ t::W, t::W, t::R, t::R, t::R };

        std::string msg;

        CHECK(order == trueorder);
        order.clear();
        run.clear();
    }
}