
#pragma once

#include <mutex>
#include <condition_variable>

namespace rwl {
    template<typename Mutex = std::mutex, typename CV = std::condition_variable>
    template<typename Mutex = std::mutex, typename CV = std::condition_variable, typename Count = int>
    class rwlocker
    {
    private:
        Mutex locker;
        CV read_queue;
        CV write_queue;
        Count wait_writers   = 0;
        Count active_readers = 0;
        Count active_writers = 0;
    public:
        rwlocker() = default ;
        rwlocker (const rwlocker&) = delete;

        void read_lock() {
            std::unique_lock ulock(locker);

            while (wait_writers || active_writers) {
                read_queue.wait(ulock);
            }

            active_readers++;
        }
        
        void read_unlock() {
            std::unique_lock ulock(locker);

            write_queue.notify_one();

            active_readers--;
        }

        void write_lock() {
            std::unique_lock ulock(locker);

            wait_writers++;
            while (active_writers || active_readers) {
                write_queue.wait(ulock);
            }
            wait_writers--;

            active_writers++;
        }

        void write_unlock() {
            std::unique_lock ulock(locker);

            if (wait_writers) {
                write_queue.notify_one();
            } else {
                read_queue.notify_all();
            }

            active_writers--;
        }
    };

    template<typename READLOCK>
    class unique_read_lock
    {
    private:
        READLOCK &rlock;
    public:
        unique_read_lock(const unique_read_lock&) = delete;
        unique_read_lock(READLOCK &initrlock) : rlock(initrlock) {
            rlock.read_lock();
        }

        ~unique_read_lock() {
            rlock.read_unlock();
        }
    };

    template<typename WRITELOCK>
    class unique_write_lock
    {
    private:
        WRITELOCK &rlock;
    public:
        unique_write_lock(const unique_write_lock&) = delete;
        unique_write_lock(WRITELOCK &initrlock) : rlock(initrlock) {
            rlock.write_lock();
        }

        ~unique_write_lock() {
            rlock.write_unlock();
        }
    }; 
}