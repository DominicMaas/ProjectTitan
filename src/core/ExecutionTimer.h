#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <string>

class ExecutionTimer {
public:
	using Clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
		std::chrono::high_resolution_clock,
		std::chrono::steady_clock>;
private:
	const Clock::time_point _start = Clock::now();
	std::string _message;

public:
	ExecutionTimer(std::string message)
	{
		_message = message;
		//Logger::LogMessage(_message + "...");
	}

	inline void stop() {
		const auto end = Clock::now();

		long time = std::chrono::duration_cast<std::chrono::milliseconds>(end - _start).count();
		//Logger::LogMessage(_message + " took: " + std::to_string(time) + "ms");
	}
};
