#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>

#include <stdarg.h>

#include <ostream>

#ifndef LOG_MESSAGE_SIZE
#define LOG_MESSAGE_SIZE        80
#endif

#ifndef LOG_MESSAGES_BUFFER
#define LOG_MESSAGES_BUFFER     5
#endif

#ifndef LOG_PIPES_COUNT
#define LOG_PIPES_COUNT         5
#endif

class Log {
public:
    enum class Level {
        Debug   = 0,
        Info    = 1,
        Warning = 2,
        Error   = 3,
    };

    Log();
    ~Log();

    void    add         (std::ostream& os);
    void    setMin      (Level value = Level::Debug)    { this->minLevel = value; }
    void    setBufferized(bool isBufferized = true)     { this->_isBufferized = isBufferized; }
    void    addMessage  (const Level level, const char* function, const size_t line, const char* message, ...);
    void    flush       ();

private:
    friend Log& logger();

    const char* getLevelString(const Level value);

    Level           minLevel;

    char            buffer[LOG_MESSAGES_BUFFER][LOG_MESSAGE_SIZE];
    size_t          startIndex;
    size_t          endIndex;
    size_t          count;
    bool            _isBufferized;

    std::ostream*   pipes[LOG_PIPES_COUNT];
    size_t          pipesCount;
};

#define log(level, message, ...)    addMessage(level, __FUNCTION__, __LINE__, message, __VA_ARGS__)

// ============================================================================

Log::Log()
    : minLevel(Level::Debug), startIndex(0), endIndex(0), count(0),
    _isBufferized(true), pipesCount(0)
{   }

Log::~Log() {
    this->flush();
}

void Log::add(std::ostream& os) {
    if(this->pipesCount < LOG_PIPES_COUNT) {
        this->pipes[this->pipesCount++] = &os;
    }
}

void Log::addMessage(const Level level, const char* function, const size_t line, const char* message, ...) {
    static char messageBuffer[LOG_MESSAGE_SIZE - 10];
    static char lineBuffer[LOG_MESSAGE_SIZE];

    if(level < minLevel) {
        return;
    }

    memset(messageBuffer, 0, LOG_MESSAGE_SIZE - 10);
    memset(buffer[endIndex], 0, LOG_MESSAGE_SIZE);

    va_list args;
    va_start(args, message);
    vsnprintf(messageBuffer, LOG_MESSAGE_SIZE - 10, message, args);
    va_end(args);

    size_t printedSize = snprintf(buffer[endIndex], LOG_MESSAGE_SIZE, "[%s|%s:%llu]:%s\r\n", getLevelString(level), function, line, messageBuffer);

    endIndex++;
    if(endIndex >= LOG_MESSAGES_BUFFER) {
        endIndex = 0;
    }
    this->count++;

    if(this->_isBufferized) {
        if(this->count >= LOG_MESSAGES_BUFFER) {
            this->flush();
        }
    } else {
        this->flush();
    }
}

const char* Log::getLevelString(const Level value) {
    switch(value) {
    case Level::Debug:
        return "Dbg ";
    case Level::Info:
        return "Info";
    case Level::Warning:
        return "Warn";
    case Level::Error:
        return "Err ";
    }
}

void Log::flush() {
    while(this->count) {
        for(size_t i = 0; i < this->pipesCount; ++i) {
            (*this->pipes[i]) << buffer[startIndex];
        }
        startIndex++;
        if(startIndex >= LOG_MESSAGES_BUFFER) {
            startIndex = 0;
        }
        this->count--;
    }
}

Log& logger() {
    static Log __ace_logger;
    return __ace_logger;
}