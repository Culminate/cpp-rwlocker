# cpp-rwlocker

Only-header библиотека реализующая примитив синхронизации "один писатель, много читателей".
Документация к классу описана в `rwlocker.hpp`

Зависимости:
- GCC(GNU Compiler Collection) - коллекция компиляторов GNU включая с++;
- cmake - автоматизация сборки;
- conan - менеджер зависимостей для с++.

## Копиляция с примером и тестами на debian 11:

```
cd <папка с проектом>
# конфигурация проекта со сборкой тестов и примера
cmake -S . -B out -DCMAKE_TESTING_ENABLED=on -DEXAMPLE=on
# сборка проекта
cmake --build out
# запуск тестов`
cd out
ctest --verbose
# запуск примера
./bin/rwlocker_example
```

## Cборка и запуск в docker:

```
cd <папка с проектом>
# сборка образа
docker build -t rwlocker .
# запуск сборки, исполнения тестов и примера
docker run -it --rm -v $PWD:/build rwlocker
```

## Пример

Чтобы собрать пример нужно в cmake включить опцию `Example`.
Будет собран исполняемый файл `rwlocker_example`.
Описание примера находится в исходном коде `rwlocker_example.cpp`.

Можно отредактировать некоторые параметры компиляции примера. Описание в cmake файле.

Параметры:
- EXAMPLE_RWLOCKER_DISABLE
- EXAMPLE_SEED
- EXAMPLE_THREAD_COUNT
- EXAMPLE_LIST_SIZE
- EXAMPLE_STRING_MULT

## Unit-tests

Тесты выполнены с применением фреймворка [doctest](https://github.com/doctest/doctest).
Для включения тестов в сборку необходимо включить в cmake опцию `CMAKE_TESTING_ENABLED`.
Будет собран исполняемый файл `test_rwlocker.cpp`.

Тесты на i5-8600 проходят примерно 24 секунды.
