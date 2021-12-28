
#include <iostream>
#include <chrono>
#include <thread>
#include <Engine.h>
#include <Tools/EngineLog.h>

#include "EngineSystems/SystemBase.h"
#include "EngineSystems/WindowSystem/WindowSystem.h"
#include "EngineSystems/RenderSystem/RenderSystem.h"
#include "Utils/Platform/PlatformUtils.h"
#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"
#include "EngineSystems/TimeSystem/TickGroupManager.h"
#include "EngineSystems/EditorUISystem/EditorUISystem.h"

namespace Engine
{
	//globally available check for systems that may require engine services but will not in the event the engine has been destroyed.
	static bool bIsEngineShutdown = false;

	EngineBase::EngineBase()
	{
		bIsEngineShutdown = false;

		//allows subclasses to have local-static singleton getters
		if (!RegisteredSingleton)
		{
			RegisteredSingleton = this;
		}
		else
		{
			throw std::runtime_error("Only a single instance of the game can be created.");
		}

		//tick group manager comes before time system so that time system can set up tick groups
		tickGroupManager = new_sp<TickGroupManager>();
	}

	EngineBase::~EngineBase()
	{
		bIsEngineShutdown = true;
	}

	EngineBase* EngineBase::RegisteredSingleton = nullptr;
	Engine::EngineBase& EngineBase::get()
	{
		if (!RegisteredSingleton)
		{
			throw std::runtime_error("GAME BASE NOT CREATED");
		}
		return *RegisteredSingleton;
	}

	void EngineBase::addToRoot(const sp<SceneNode>& child)
	{
		if (bStarted && rootNode && child)
		{
			child->setParent(rootNode);
		}
	}

	void EngineBase::start(const std::function<void()>& initializationFunc)
	{
		//WARNING: any local objects (eg smart pointers) in this function will have lifetime of game!
		//DEV-NOTE: this method should be kept simple, as it provides a high level overview of the engine.
		if (!bStarted)
		{
			//onInitEngineConstants(configuredConstants);	//this should happen before the subclass game has started. this means systems can read it.
			registerTickGroups();						//tick groups created very early, these are effectively static and not intended to be initialized with dnyamic logic from systems. Thus these are created before systems.
			createEngineSystems();
			//systems are initialized after all systems have been created; this way cross-system interaction can be achieved during initailization (ie subscribing to events, etc.)
			for (const sp<SystemBase>& system : systems) { system->initSystem(); }

			windowSystem->makeWindowPrimary(makeInitialWindow());
			startUp();
			bStarted = true;

			//game loop processes
			onGameloopBeginning.broadcast();

			initializationFunc(); //do user initialize just before first game loop.
#if HTML_BUILD
			emscripten_set_main_loop(&EngineBase::htmlTickMain, 0, /*simulate_infinite_loop*/true);
#else
			while (!bExitGame)
			{
				framerateSleep();
				tickGameloop_EngineBase();
			}
			end();
#endif
		}
	}

	void EngineBase::end()
	{
		//begin shutdown process
		onShutDown();

		//tick a few more times for any frame deferred processes
		for (size_t shutdownTick = 0; shutdownTick < 3; ++shutdownTick) { tickGameloop_EngineBase(); }

		//shutdown systems after game client has been shutdown
		for (const sp<SystemBase>& system : systems) { if (system) { system->shutdown(); } }

		onShutdownGameloopTicksOver.broadcast();

		//disassociate static getters after shutdowns are complete. Doing this after allows systems to cross-reference while shutting down
		for (const sp<SystemBase>& system : systems) { if (system) { system->registerGetter(SingletonOperation::REMOVE); } }
	}

	bool EngineBase::isEngineShutdown()
	{
		return bIsEngineShutdown;
	}

	sp<Engine::Window> EngineBase::makeInitialWindow()
	{
		int width = 1440, height = 810;
		sp<Window> window = new_sp<Window>(width, height);
		ec(glViewport(0, 0, width, height)); //#TODO, should we do this in the EngineBase level on "glfwSetFramebufferSizeCallback" changed?
		return window;
	}

	void EngineBase::startShutdown()
	{
		log("GameFramework", LogLevel::LOG, "Shutdown Initiated");
		onShutdownInitiated.broadcast();
		onShutDown();
		bExitGame = true;
	}

	void EngineBase::tickGameloop_EngineBase()
	{
		timeSystem.updateTime(TimeSystem::PrivateKey{});
		float deltaTimeSecs = systemTimeManager->getDeltaTimeSecs();

		GameObjectBase::cleanupPendingDestroy(GameObjectBase::CleanKey{});

		//the engine will tick a few times after shutdown to clean up deferred tasks.
		if (!bExitGame)
		{
			//#consider having system pass a reference to the system time manager, rather than a float; That way critical systems can ignore manipulation time effects or choose to use time affects. Passing raw time means systems will be forced to use time effects (such as dilation)
			for (const sp<SystemBase>& system : systems) { system->tick(deltaTimeSecs); }

			///////////////////////
			//UPDATE GAME LOGIC
			///////////////////////
			//NOTE: there probably needs to be a priority based pre/post loop; but not needed yet so it is not implemented (priorities should probably be defined in a single file via template specliazations)
			onPreGameloopTick.broadcast(deltaTimeSecs);
			tickGameLoop(deltaTimeSecs);
			onPostGameloopTick.broadcast(deltaTimeSecs);

			///////////////////////
			//RENDER
			///////////////////////
			//cacheRenderDataForCurrentFrame(*renderSystem->getFrameRenderData_Write(frameNumber, identityKey));
			renderSystem->clearScreen();
			renderSystem->prepareRenderDataForFrame();
			renderLoop_begin(deltaTimeSecs);
			onRenderDispatch.broadcast(deltaTimeSecs); //LEAVING this to make porting easy - perhaps this needs to be a sorted structure with prioritizes; but that may get hard to maintain. Needs to be a systematic way for UI to come after other rendering.
			renderSystem->onRenderDispatch.broadcast(deltaTimeSecs); //todo - make a more roboust render set up 
			renderLoop_end(deltaTimeSecs);
			onRenderDispatchEnded.broadcast(deltaTimeSecs);

			//perhaps this should be a subscription service since few systems care about post render //TODO this system should probably be removed and instead just subscribe to delegate
			for (const sp<SystemBase>& system : postRenderNotifys) { system->handlePostRender(); }
		}

		//broadcast current frame and increment the frame number.
		onFrameOver.broadcast(frameNumber++);
#if HTML_BUILD
		lastFrameTime = glfwGetTime(); //html does not do framerate throttling because that is handled by browser emscripten set main function

		if (bExitGame)
		{
			//html needs to exit here, since the main loop is not nested in a function like desktop.
			end();
		}
#endif
	}

	/*static*/ void EngineBase::htmlTickMain()
	{
		//log(__FUNCTION__, LogLevel::LOG, "htmlMainTickTest");
		EngineBase::get().tickGameloop_EngineBase();
	}

	void EngineBase::createEngineSystems()
	{
		// !!! REFACTOR WARNING !!  do not place this within the ctor; polymorphic systems are designed to be instantiated via virtual functions; virutal functions shouldn't be called within a ctor!
		//this is provided outside of ctor so that virtual functions may be called to define systems that are polymorphic based on the game

		//initialize time management systems; this is done after tick groups are created so that timeManager can set up tick groups at construction
		systemTimeManager = timeSystem.createManager();
		gameTimeManager = timeSystem.createManager();
		timeSystem.markManagerCritical(TimeSystem::PrivateKey{}, systemTimeManager);

		//create and register systems
		windowSystem = new_sp<WindowSystem>();
		validateSystemSpawned(windowSystem);
		systems.insert(windowSystem);

		renderSystem = createRenderSystemSubclass();
		validateSystemSpawned(renderSystem);
		systems.insert(renderSystem);

		sp<SystemBase> editorUISystem = new_sp<UISystem_Editor>();
		validateSystemSpawned(editorUISystem);
		systems.insert(editorUISystem);

		//assetSystem = new_sp<AssetSystem>();
		//systems.insert(assetSystem);

		//levelSystem = new_sp<LevelSystem>();
		//systems.insert(levelSystem);

		//playerSystem = new_sp<PlayerSystem>();
		//systems.insert(playerSystem);

		//particleSystem = new_sp<ParticleSystem>();
		//systems.insert(particleSystem);

		//systemRNG = new_sp<RNGSystem>();
		//systems.insert(systemRNG);

		//debugRenderSystem = new_sp<DebugRenderSystem>();
		//systems.insert(debugRenderSystem);

		//automatedTestSystem = new_sp<AutomatedTestSystem>();
		//systems.insert(automatedTestSystem);

		//cheatSystem = createCheatSystemSubclass();
		//cheatSystem = cheatSystem ? cheatSystem : new_sp<CheatSystemBase>();
		//systems.insert(cheatSystem);

		//curveSystem = createCurveSystemSubclass();
		//curveSystem = curveSystem ? curveSystem : new_sp<CurveSystem>();
		//systems.insert(curveSystem);

		//audioSystem = createAudioSystemSubclass();
		//audioSystem = audioSystem ? audioSystem : new_sp<AudioSystem>();
		//systems.insert(audioSystem);

		//initialize custom subclass systems; 
		//ctor warning: this is not done in EngineBase ctor because systems may call EngineBase virtuals
		bCustomSystemRegistrationAllowedTimeWindow = true;
		onRegisterCustomSystem();
		bCustomSystemRegistrationAllowedTimeWindow = false;

		for (sp<SystemBase> system : systems)
		{
			if (system)
			{
				system->registerGetter(SingletonOperation::REGISTER);

				if (!system->hasRegisteredStaticGet())
				{
					throw std::runtime_error("system failed to set up static getter; did you forget to define a corret parent in the implement get macro?");
				}
			}
		}
	}

	sp<Engine::RenderSystem> EngineBase::createRenderSystemSubclass()
	{
		return new_sp<RenderSystem>();
	}

	void EngineBase::validateSystemSpawned(const sp<SystemBase>& system)
	{
		if (system == nullptr)
		{
			throw std::runtime_error("failed to spawn required system! did you override a method but return null?");
		}
	}

	//sp<Engine::CurveSystem> EngineBase::createCurveSystemSubclass()
	//{
	//	return new_sp<CurveSystem>();
	//}

	//sp<Engine::AudioSystem> EngineBase::createAudioSystemSubclass()
	//{
	//	return new_sp<AudioSystem>();
	//}

	void EngineBase::subscribePostRender(const sp<SystemBase>& system)
	{
		postRenderNotifys.insert(system);
	}

	void EngineBase::framerateSleep()
	{
		if (!bEnableFramerateLimit) { return; }

		double currentTimeBeforeSleep = glfwGetTime();
		double deltaSec = currentTimeBeforeSleep - lastFrameTime;

		double frameTargetDeltaSec = 1.0 / double(targetFramesPerSecond);

		bool bShouldSleep = frameTargetDeltaSec > deltaSec;
		double waitTimeSec = frameTargetDeltaSec - deltaSec;
		long long waitTimeMiliSec = static_cast<long long>(waitTimeSec * 1000.f);
		if (bShouldSleep)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMiliSec));
		}

		lastFrameTime = glfwGetTime();
		if (bShouldSleep && lastFrameTime == currentTimeBeforeSleep)
		{
			STOP_DEBUGGER_HERE(); // helps catch if on other drivers glfw isn't updating time after the sleep, if this is happening I may need to rething this framerate cap method
		}
	}

	void EngineBase::registerTickGroups()
	{
		tickGroupManager->start_TickGroupRegistration(TickGroupManager::EngineBaseKey{});

		//give users change to create special subclass with extra tick groups.
		tickGroupData = onRegisterTickGroups();
		if (!tickGroupData)
		{
			STOP_DEBUGGER_HERE(); //subclass did not provide a subclass, providing default one. This is not expected behavior.
			tickGroupData = new_sp<TickGroups>();
		}

		tickGroupManager->stop_TickGroupRegistration(TickGroupManager::EngineBaseKey{});
	}

	sp<TickGroups> EngineBase::onRegisterTickGroups()
	{
		return new_sp<TickGroups>();
	}

	void EngineBase::RegisterCustomSystem(const sp<SystemBase>& system)
	{
		if (bCustomSystemRegistrationAllowedTimeWindow)
		{
			systems.insert(system);
		}
		else
		{
			std::cerr << "FATAL: attempting to register a custom system outside of start up window; use appropraite virtual function to register these systems" << std::endl;
		}
	}




}


