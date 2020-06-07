#pragma once

#include <spdlog/spdlog.h>

#include <spdlog/sinks/stdout_color_sinks.h>

#include <iostream>

class Logger
{
public:
	Logger();
	~Logger();

private:
	std::shared_ptr<spdlog::logger> logger;
};

Logger::Logger()
{
	logger = spdlog::stdout_color_mt("console");

	for (int i = 0; i < 10; i++) {
		logger->info("Hello world");
		logger->flush();
	}
	std::cout << "Hello" << std::endl;
	std::cout << "Good bye!" << std::endl;
}

Logger::~Logger() {}
