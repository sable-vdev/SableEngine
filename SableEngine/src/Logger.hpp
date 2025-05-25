#pragma once
#include <iostream>
#include <string>
#include <chrono>

#ifdef ERROR
#undef ERROR
#endif

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
	static void Log(LogLevel logLevel, const std::string& message);
private:
	static std::string LogLevelToString(LogLevel logLevel);
	Logger() = delete;
	~Logger() = delete;
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
};
