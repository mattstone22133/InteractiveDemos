#pragma once

#include<cstdint>
#include<cstdio>

namespace Engine
{
enum class LogLevel : uint8_t
{
	LOG,
	LOG_WARNING,
	LOG_ERROR
};

void log(const char* logName, LogLevel level, const char* msg);

namespace EngineLogging
{
	extern char formatBuffer[10240];
}

#define Logf(logName, logLevel, msg, ...)\
	{\
		snprintf(EngineLogging::formatBuffer, sizeof(EngineLogging::formatBuffer), msg, __VA_ARGS__); /*note: currently must provide argument to prevent compile error*/ \
		log(logName, logLevel, EngineLogging::formatBuffer);\
	}
}

