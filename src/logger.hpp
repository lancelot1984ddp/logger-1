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

#if defined(_DEBUG)
#define LOG_DEFAULT_MIN_LEVEL   Log::Level::Debug
#else
#define LOG_DEFAULT_MIN_LEVEL   Log::Level::Info
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
    void    setMin      (Level value = LOG_DEFAULT_MIN_LEVEL)   { if(value >= LOG_DEFAULT_MIN_LEVEL) this->minLevel = value; }
    void    setBufferized(bool isBufferized = true)             { this->_isBufferized = isBufferized; }
    void    addMessage  (const Level level, const char* function, const size_t line, const char* message, ...);
    void    flush       ();

private:
    friend Log& logger  ();

    const char* getLevelString(const Level value);
    void    performPushMessage(const size_t printedSize);

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
    : minLevel(LOG_DEFAULT_MIN_LEVEL), startIndex(0), endIndex(0), count(0),
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
    if(level < minLevel) {
        return;
    }

    memset(buffer[endIndex], 0, LOG_MESSAGE_SIZE);

    size_t printedSize = snprintf(buffer[endIndex], LOG_MESSAGE_SIZE, "[%s|%s:%llu]:", getLevelString(level), function, line);

    va_list args;
    va_start(args, message);
    printedSize += vsnprintf(buffer[endIndex] + printedSize, LOG_MESSAGE_SIZE - printedSize, message, args);
    va_end(args);

    performPushMessage(printedSize);
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

// ----------------------------------------------------------------------------

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

void Log::performPushMessage(const size_t printedSize) {
    if(printedSize <= LOG_MESSAGE_SIZE - 3) {
        buffer[endIndex][printedSize + 0] = '\r';
        buffer[endIndex][printedSize + 1] = '\n';
        buffer[endIndex][printedSize + 2] = '\0';
    } else {
        buffer[endIndex][LOG_MESSAGE_SIZE - 3] = '\r';
        buffer[endIndex][LOG_MESSAGE_SIZE - 2] = '\n';
        buffer[endIndex][LOG_MESSAGE_SIZE - 1] = '\0';
    }

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

Log& logger() {
    static Log __emdc_logger;
    return __emdc_logger;
}