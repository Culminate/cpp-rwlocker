#include <iostream>
#include <thread>
#include <atomic>
#include "rwlocker.hpp"

rwl::rwlocker locker;
std::atomic_uint64_t inc = 0;

void read() {
    using namespace std::chrono_literals;
    rwl::unique_read_lock ulock(locker);
    std::cout << "read\n";
    std::this_thread::sleep_for(1s);
    std::cout << "eread\n";
}

void write() {
    using namespace std::chrono_literals;
    rwl::unique_write_lock ulock(locker);
    std::cout << "write\n";
    std::this_thread::sleep_for(2s);
    std::cout << "ewrite\n";
}

int main(int argc, char const *argv[])
{
    std::thread t1(read);
    std::thread t2(write);
    std::thread t3(read);
    std::thread t5(write);
    std::thread t4(read);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    return 0;
}
