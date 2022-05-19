
#pragma once

#include <mutex>
#include <condition_variable>

namespace rwl {
    /**
     * Класс реализующий примитив синхронизации один писатель, много читателей.
     * 
     * Использование:
     * Если у нас есть ресурс в многопоточной среде который нужно защитить от одновременной записи,
     * но при этом от чтения защищать не требуется.
     * Метод read_lock позовляет открыть критическую секцию для чтения.
     * Метод read_unlock позволяет закрыть критическую секцию для чтения.
     * 
     * Метод write_lock позовляет открыть критическую секцию для записи.
     * Метод write_unlock позволяет закрыть критическую секцию для записи.
     * 
     * 
     * Ограничения на использование исходят из типов, которые переданы как шаблонные параметры.
       Не должно быть одновременно вызвано блокировок больше чем std::numeric_limits<Count>::max
     * Вложенное открытие критической секции приводит к UB. Независимо от типа критической секции(read/write).
     * 
     * \tparam Mutex Класс соответвующий ограничениям Lockable, по умолчанию std::mutex.
     * \tparam CV Класс соответвующий интерфейсу std::condition_variable.
     * \tparam Count Интегральный тип для подсчёта активных и ждущих потоков.
     *  
     */
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

        /** Блокировка секции на чтение */
        void read_lock() {
            std::unique_lock ulock(locker);

            while (wait_writers || active_writers) {
                read_queue.wait(ulock);
            }

            active_readers++;
        }
        
        /** Разблокировка секции на чтение */
        void read_unlock() {
            std::unique_lock ulock(locker);

            write_queue.notify_one();

            active_readers--;
        }

        /** Блокирока секции на запись */
        void write_lock() {
            std::unique_lock ulock(locker);

            wait_writers++;
            while (active_writers || active_readers) {
                write_queue.wait(ulock);
            }
            wait_writers--;

            active_writers++;
        }

        /* Разблокировка секции на запись */
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

    /** RAII для класса rwlocker. Вызывает блокироку на чтение */
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

    /** RAII для класса rwlocker. Вызывает блокировку на запись */
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