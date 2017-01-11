#include <iostream>

#define _DEBUG

#include <logger.hpp>

int main() {
	logger().add(std::cout);
    logger().add(std::cerr);
    logger().add(std::clog);

    const char* errorMessage = "Some long long error from main function";

    logger().log(Log::Level::Debug, "Some debug message #%d", 10);
    logger().log(Log::Level::Info, "Some info with %.3f", 245.2835);
    logger().log(Log::Level::Warning, "Some warning with %.3f", 245.2835);
    logger().log(Log::Level::Error, "Some error with message \"%s\"", errorMessage);
    logger().log(Log::Level::Debug, "Some debug message #%d", 10);
    logger().log(Log::Level::Info, "Some info with %.3f", 245234.2835);
    logger().log(Log::Level::Warning, "Some warning with %.3f", 5745.2835);
    logger().log(Log::Level::Error, "Some error with message \"%s\"", errorMessage);

    logger().flush();

	return 0;
}