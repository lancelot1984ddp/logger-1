# logger
Simple C++ logger for embedded solutions.

### Description
* support `std::ostream` streams: `logger().add(std::cout)`;
* support 4 levels: Debug, Info, Warning, Error;
* autoflush into streams when buffer is full;
* customizable buffer and message sizes by #define constatns
* message formats

### Example
Full example see in `test/test.cpp`. Short example:

```C++
logger().add(std::cout);

logger().log(Log::Level::Debug, "Some message #%d", 10);
logger().log(Log::Level::Info, "Some message with %.3f", 245.2835);
logger().log(Log::Level::Warning, "Some message with %.3f", 245.2835);
logger().log(Log::Level::Error, "Some message with error \"%s\"", "Error message");
```
