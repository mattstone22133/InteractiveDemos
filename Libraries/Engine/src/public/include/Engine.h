#pragma once
#include "Event.h"
#include "SceneNode.h"

namespace Engine
{

	class EngineBase
	{
	public:
		Event<> TestEvent;
	private:
		sp<SceneNode> WorldRoot = nullptr;
	public:
		EngineBase();


	};

}
