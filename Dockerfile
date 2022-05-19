FROM debian:11-slim

RUN apt update && \
    apt install -y pip cmake build-essential && \
    pip install conan && \
    conan profile new --detect --force default

WORKDIR /build

CMD cmake -S . -B out -DCMAKE_TESTING_ENABLED=on -DEXAMPLE=on && \
    cmake --build out && \
    cd out && ctest --verbose; \
    ./bin/rwlocker_example