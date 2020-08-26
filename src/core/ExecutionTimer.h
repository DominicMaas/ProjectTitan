#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include "spdlog/spdlog.h"

class ExecutionTimer {
private:
    std::string _message;
    double _start;

public:
    ExecutionTimer(std::string message) {
        _message = message;
        _start = glfwGetTime();
    }

    inline void stop() {
        double end = glfwGetTime();
        double delta = (end - _start) * 60;

        spdlog::info(_message + " took: " + std::to_string(delta) + "ms");
    }
};
