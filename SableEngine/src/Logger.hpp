#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <source_location>
#undef ERROR


enum LogLevel
{
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

class Logger
{
public:
	static void Log(LogLevel logLevel, const std::string& message, const std::source_location& location = std::source_location::current());
private:
	static std::string LogLevelToString(LogLevel logLevel);
	Logger() = delete;
	~Logger() = delete;
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
};
