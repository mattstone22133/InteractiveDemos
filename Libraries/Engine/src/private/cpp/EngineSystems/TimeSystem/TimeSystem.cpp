#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"

#include <algorithm>
#include <assert.h>

#include "EngineSystems/TimeSystem/TimeSystem.h"
#include "Tools/EngineLog.h"
#include "Tools/DataStructures/IterableHashSet.h"
#include "Tools/DataStructures/ObjectPools.h"
#include "EngineSystems/TimeSystem/TickGroupManager.h"
#include "Utils/Platform/PlatformUtils.h"
#include "Engine.h"

namespace
{
	Engine::SP_SimpleObjectPool<Engine::Timer> timerPool;

}

namespace Engine
{

	void Timer::reset()
	{
		durationSecs = 0.f;
		currentTime = 0.f;
		bLoop = false;
		userCallback = nullptr;
	}

	void Timer::set(const sp<Event<>>& inCallbackDelegate, float inDurationSecs, bool inbLoop, float delaySecs)
	{
		reset();
		userCallback = inCallbackDelegate;
		durationSecs = inDurationSecs;
		bLoop = inbLoop;

		//set the timer into negative region for the delay
		currentTime = -delaySecs;
	}

	bool Timer::update(float dt_dilatedSecs)
	{
		if (!userCallback) { return true; }
		if (userCallback->numBound() == 0) { return true; }

		currentTime += dt_dilatedSecs;

		//if timer should have ticked twice in time-frame, then tick it twice.
		//the alternative may cause unexpected behavior for user
		while (currentTime > durationSecs)
		{
			userCallback->broadcast();
			currentTime -= durationSecs;

			if (!bLoop)
			{
				return true;
			}
		}

		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Time Manager
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TimeManager::update(PrivateKey /*key*/, TimeSystem& timeSystem)
	{
		////////////////////////////////////////////////////////
		// updating state
		////////////////////////////////////////////////////////
		framesToStep = framesToStep > 0 ? framesToStep - 1 : framesToStep;
		if (newFramesToStep > 0)
		{
			framesToStep = newFramesToStep;
			newFramesToStep = 0;
		}

		//prevents time dilation from happening mid frame
		timeDilationFactor = DilationFactor_nextFrame;

		dt_undilatedSecs = timeSystem.getDeltaTimeSecs();
		dt_dilatedSecs = dt_undilatedSecs * timeDilationFactor;
		timeSinceStartSecs_Dilated += dt_dilatedSecs;

		////////////////////////////////////////////////////////
		//tick timers
		////////////////////////////////////////////////////////
		{
			bTickingTimers = true;
			for (const sp<Timer>& timer : timers)
			{
				if (timer->update(dt_dilatedSecs))
				{
					timersToRemoveWhenTickingOver.emplace_back(timer);
				}
			}
			bTickingTimers = false;
		}

		////////////////////////////////////////////////////////
		//clear stale timers before adding deferred timers
		////////////////////////////////////////////////////////
		if (timersToRemoveWhenTickingOver.size() > removeTimerReservationSpace)
		{
			removeTimerReservationSpace = std::clamp(removeTimerReservationSpace, 0U, 10000U);
			timersToRemoveWhenTickingOver.reserve(removeTimerReservationSpace);
		}
		for (const sp<Timer> timer : timersToRemoveWhenTickingOver)
		{
			//ticking cannot be happening since we're below it -- this should remove the timer (and avoid code duplication)
			if (removeTimer(timer->getUserCallback()) != ETimerOperationResult::SUCCESS)
			{
				//log failure when there exists a more performant logging system
				log("TimeManagementSystem", LogLevel::LOG_ERROR, "Failed to remove timer; this should not happen.");
			}
		}
		timersToRemoveWhenTickingOver.clear();

		////////////////////////////////////////////////////////
		//add deferred timers
		////////////////////////////////////////////////////////
		for (const sp<Timer>& timer : deferredTimersToAdd)
		{
			if (Event<>* delegateHandle = timer->getUserCallback().get())
			{
				timers.insert(timer);
				delegateToTimerMap.insert({ delegateHandle , timer });
			}
			else
			{
				//#TODO log when logging has been refactored and isn't so expensive; actually this really should be an assertion failure
			}
		}
		deferredTimersToAdd.clear();
		deferredTimerDelegatesToAdd.clear();

		////////////////////////////////////////////////////////
		// Tick Tickables
		////////////////////////////////////////////////////////
		bIsTickingTickables = true;
		for (const sp<ITickable>& tickable : tickables)
		{
			bool bKeepTicking = tickable->tick(dt_dilatedSecs);
			if (!bKeepTicking) { pendingRemovalTickables.insert(tickable); }
		}
		bIsTickingTickables = false;
		for (const sp<ITickable>& tickable : pendingAddTickables)
		{
			tickables.insert(tickable);
		}
		for (const sp<ITickable>& tickable : pendingRemovalTickables)
		{
			tickables.remove(tickable);
		}
		pendingAddTickables.clear();
		pendingRemovalTickables.clear();

		////////////////////////////////////////////////////////
		// Tick Groups
		////////////////////////////////////////////////////////
		for (TickGroupEntry& tickGroup : tickGroups)
		{
			//delegate already cover subscription/removal edge cases, they do not need to be covered here. Just let someone attempt to register to event and it will be applied after broadcast.
			tickGroup.onTick->broadcast(dt_dilatedSecs);
		}
	}

	TimeManager::TimeManager()
	{
		TickGroupManager& tickGroupManager = EngineBase::get().getTickGroupManager();

		bool bTickGroupsInitialized = tickGroupManager.areTickGroupsInitialized();
		if (!bTickGroupsInitialized)
		{
			STOP_DEBUGGER_HERE();	//a refactor happened so that tickgroup constant data was not initialized before time manager data,
									//tick groups need to be initialized first so that time managers can configure sorted tick groups.
		}
		assert(tickGroupManager.areTickGroupsInitialized());
		for (const TickGroupDefinition& tgDef : tickGroupManager.getSortedTickGroups())
		{
			tickGroups.emplace_back();
			TickGroupEntry& tickGroup = tickGroups.back();
			tickGroup.name = tgDef.name;
			tickGroup.priority = tgDef.priority;
			tickGroup.sortIdx = tgDef.sortIdx();
			tickGroup.onTick = new_sp<Event<float /*dt_sec*/>>(); //this makes copies shallow, which is what we want. Currently no copies should be possible.
		}


		timersToRemoveWhenTickingOver.reserve(removeTimerReservationSpace);
		//pendingAddTickables.reserve(tickerDeferredRegistrationMinBufferSize);
		//pendingRemovalTickables.reserve(tickerDeferredRegistrationMinBufferSize);
	}

	bool TimeManager::hasTimerForDelegate(const sp<Event<>>& boundDelegate)
	{
		if (boundDelegate)
		{
			return delegateToTimerMap.find(boundDelegate.get()) != delegateToTimerMap.end();
		}
		return false;
	}

	Engine::ETimerOperationResult TimeManager::createTimer(const sp<Event<>>& callbackDelegate, float durationSec, bool bLoop /* = false*/, float delaySecs /*= 0.f*/)
	{
		if (durationSec < 0) //setting duration equal to 0 is like a "next tick" timer
		{
			return ETimerOperationResult::FAILURE_NEGATIVE_DURATION;
		}

		if (bTickingTimers)
		{
			/*! timers can be deferred until timer ticking is over because adding during ticking will cause premature addition to the timer's clock */
			if (delegateToTimerMap.find(callbackDelegate.get()) != delegateToTimerMap.end())
			{
				//technically this timer could be pending remove after its tick, but if user wants that then they should be looping
				return ETimerOperationResult::DEFER_FAILURE_TIMER_FOR_DELEGATE_EXISTS;
			}
			if (deferredTimerDelegatesToAdd.find(callbackDelegate.get()) != deferredTimerDelegatesToAdd.end())
			{
				return ETimerOperationResult::DEFER_FAILURE_DELEGATE_ALREADY_PENDING_ADD;
			}

			sp<Engine::Timer> timerInstance = timerPool.getInstance();
			timerInstance->set(callbackDelegate, durationSec, bLoop, delaySecs);

			deferredTimerDelegatesToAdd.insert({ callbackDelegate.get(), timerInstance });
			deferredTimersToAdd.insert(timerInstance);
		}
		else
		{
			if (delegateToTimerMap.find(callbackDelegate.get()) != delegateToTimerMap.end())
			{
				return ETimerOperationResult::FAILURE_TIMER_FOR_DELEGATE_EXISTS;
			}

			sp<Engine::Timer> timerInstance = timerPool.getInstance();
			timerInstance->set(callbackDelegate, durationSec, bLoop, delaySecs);

			delegateToTimerMap.insert({ callbackDelegate.get(), timerInstance });
			timers.insert(timerInstance);
		}

		return ETimerOperationResult::SUCCESS;
	}

	ETimerOperationResult TimeManager::removeTimer(const sp<Event<>>& callbackDelegate)
	{
		auto findResult = delegateToTimerMap.find(callbackDelegate.get());
		if (findResult != delegateToTimerMap.end())
		{
			if (!bTickingTimers)
			{
				timers.remove(findResult->second);
				timerPool.releaseInstance(findResult->second);

				findResult->second->reset();

				delegateToTimerMap.erase(findResult);
				return ETimerOperationResult::SUCCESS;
			}
			else
			{
				timersToRemoveWhenTickingOver.emplace_back(findResult->second);
				return ETimerOperationResult::DEFERRED;
			}
		}
		return ETimerOperationResult::FAILURE_TIMER_NOT_FOUND;

	}

	void TimeManager::registerTicker(const sp<ITickable>& tickable)
	{
		//WARNING: don't check tickables contains the new tickable and early out if it does; if you do removing then adding in same tick frame will break.
		//^^Since we're dealing with sets, it isn't really necessary to do that check anyways. This has a test case in the unit tests "readdition test".
		if (bIsTickingTickables)
		{
			pendingAddTickables.insert(tickable);
			pendingRemovalTickables.remove(tickable); //remove previous attempt to clear! Last operation will be the valid one.
		}
		else
		{
			tickables.insert(tickable);
		}
	}

	void TimeManager::removeTicker(const sp<ITickable>& tickable)
	{
		if (bIsTickingTickables)
		{
			pendingRemovalTickables.insert(tickable);
			pendingAddTickables.remove(tickable); //remove previous attempt to add this frame! Last operation will be the valid one.
		}
		else
		{
			tickables.remove(tickable);
		}
	}

	bool TimeManager::hasRegisteredTicker(const sp<ITickable>& tickable)
	{
		//return tickables.contains(tickable);
		return (tickables.contains(tickable) || pendingAddTickables.contains(tickable))
			&& !pendingRemovalTickables.contains(tickable);
	}

	Engine::Event<float /*dt_sec*/>& TimeManager::getEvent(const TickGroupDefinition& tickGroupData)
	{
#ifdef DEBUG_BUILD
		assert(tickGroups.size() > tickGroupData.sortIdx() && tickGroupData.isRegistered());
#endif 
		//use sorted index to bypass any slow lookups from string comparisons. sortIdx is set an engine start up.
		return *tickGroups[tickGroupData.sortIdx()].onTick;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Time System
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TimeSystem::updateTime(PrivateKey /*key*/)
	{
		bUpdatingTime = true;

		currentTime = static_cast<float>(glfwGetTime()); //note: porting code and this was a local variable, don't have unit tests set up to test change. but it was shadowing class var.
		rawDeltaTimeSecs = currentTime - lastFrameTime;
		rawDeltaTimeSecs = rawDeltaTimeSecs > MAX_DELTA_TIME_SECS ? MAX_DELTA_TIME_SECS : rawDeltaTimeSecs;
		deltaTimeSecs = rawDeltaTimeSecs;
		lastFrameTime = currentTime;

		for (const sp<TimeManager>& manager : managers)
		{
			manager->update(TimeManager::PrivateKey{}, *this);
		}

		bUpdatingTime = false;
	}

	void TimeSystem::markManagerCritical(PrivateKey, sp<TimeManager>& manager)
	{
		criticalManagers.insert(manager);
	}

	sp<TimeManager> TimeSystem::createManager()
	{
		sp<TimeManager> newManager = new_sp<TimeManager>();

		managers.insert(newManager);

		return newManager;
	}

	void TimeSystem::destroyManager(sp<TimeManager>& manager)
	{
		if (criticalManagers.find(manager) == criticalManagers.end())
		{
			managers.erase(manager);
			manager = nullptr;
		}
		else
		{
			log("TimeSystem", LogLevel::LOG_WARNING, "Attempting to destroy a system critical time manager");
		}
	}


}

