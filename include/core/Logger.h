#pragma once

#include <string>
#include <iostream>
#include <ctime>

class Logger
{
public:
	static void LogMessage(std::string message)
	{
		// Get current time
		std::time_t t = std::time(0);
		std::tm* now = std::localtime(&t);

		std::cout << "[" << std::to_string(now->tm_hour) << ":" << std::to_string(now->tm_min) << ":" << std::to_string(now->tm_sec) << "] " << message << std::endl;
	}
};