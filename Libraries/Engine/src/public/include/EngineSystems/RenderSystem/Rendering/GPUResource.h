#pragma once

#include "GameObjectBase.h"
#include "Event.h"

namespace Engine
{
	class Window;

	//#TODO create system that detects post construct was not called and alert user; this could be mitigated with smarter events

	class GPUResource : public GameObjectBase
		, public IEventSubscriber
	{
	public:
		inline bool hasAcquiredResources() const { return bAcquiredGPUResource; }
		bool isGPUContextAvailable() const;
		//virtual ~GPUResource(); //thinking about adding this? cleaning up via virtual functions in dtor is probably not what you want
		void cleanup();
	protected:
		virtual void postConstruct() override;
	private:
		void handleWindowLosingGPUContext(const sp<Window>& window);
		void handleWindowAcquiredGPUContext(const sp<Window>& window);
		void handlePostEngineShutdownTicksOver();
	private: //subclass api provided
		virtual void onAcquireGPUResources() = 0;
		virtual void onReleaseGPUResources() = 0;
	private:
		bool bAcquiredGPUResource = false;
	};
}