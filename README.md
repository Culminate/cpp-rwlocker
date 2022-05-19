# cpp-rwlocker

Only-header библиотека реализующая примитив синхронизации один писатель, много читателей.
Документация к классу описана в rwlocker.hpp

Копиляция с примером и тестами.

```
cd <папка с проектом>
cmake -S . -B out -DCMAKE_TESTING_ENABLED=on -DEXAMPLE=on
cmake --build out
ctest --test-dir out --verbose

```