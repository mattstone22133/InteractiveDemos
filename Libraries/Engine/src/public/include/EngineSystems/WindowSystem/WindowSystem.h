#pragma once

#include <cstdint>
#include <map>
#include <set>

#include "EngineSystems/SystemBase.h"
#include "Event.h"
#include "WindowWrapper.h"


namespace Engine
{
	class WindowSystem : public SystemBase
	{
	public: //events
		/*This event should not be used to determine when OpenGL contexts change */
		Event<const sp<Window>& /*old_window*/, const sp<Window>& /*new_window*/> onPrimaryWindowChangingEvent;
		Event<const sp<Window>&> onWindowLosingOpenglContext;
		Event<const sp<Window>&> onWindowAcquiredOpenglContext;
		Event<const sp<Window>&> onFocusedWindowTryingToClose;

	public:
		const sp<Window>& getPrimaryWindow() { return focusedWindow; }
		void makeWindowPrimary(const sp<Window>& window);

	private:
		virtual void initSystem() override;
		virtual void tick(float deltaSec) override;
		virtual void handlePostRender() override;

	private:
		sp<Window> focusedWindow = nullptr;
	};
}