

#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include "spdlog/spdlog.h"

class ExecutionTimer {
public:
    using Clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
            std::chrono::high_resolution_clock,
            std::chrono::steady_clock>;
private:
    const Clock::time_point _start = Clock::now();
    std::string _message;

public:
    ExecutionTimer(std::string message) {
        _message = message;
    }

    inline void stop() {
        const auto end = Clock::now();

        long time = std::chrono::duration_cast<std::chrono::milliseconds>(end - _start).count();
        spdlog::info(_message + " took: " + std::to_string(time) + "ms");
    }
};