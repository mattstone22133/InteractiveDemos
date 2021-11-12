#include "Tools/EngineLog.h"

#include <iostream>
#include <string>

#include "Engine.h"

namespace Engine
{
	namespace EngineLogging
	{
		char formatBuffer[10240];
	}

	void log(const char* logName, LogLevel level, const char* msg)
	{
		static EngineBase& engine = EngineBase::get();
		std::string frame = "[" + std::to_string(engine.getFrameNumber()) + "]";

		std::ostream& output = (level == LogLevel::LOG_WARNING || level == LogLevel::LOG_ERROR) ? std::cerr : std::cout;
		output << logName << " " << frame << " : " << msg << std::endl;
	}





}

