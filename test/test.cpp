#include <iostream>

#include <logger.hpp>

int main() {
	logger().add(std::cout);
    logger().add(std::cerr);
    logger().add(std::clog);

    const char* errorMessage = "Some error";

    logger().log(Log::Level::Debug, "Some message #%d", 10);
    logger().log(Log::Level::Info, "Some message with %.3f", 245.2835);
    logger().log(Log::Level::Warning, "Some message with %.3f", 245.2835);
    logger().log(Log::Level::Error, "Some message with error \"%s\"", errorMessage);
    logger().log(Log::Level::Debug, "Some message #%d", 10);
    logger().log(Log::Level::Info, "Some message with %.3f", 245.2835);
    logger().log(Log::Level::Warning, "Some message with %.3f", 245.2835);
    logger().log(Log::Level::Error, "Some message with error \"%s\"", errorMessage);

    logger().flush();

	return 0;
}