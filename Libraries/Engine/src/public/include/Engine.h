#pragma once

#include <vector>
#include <functional>

#include "Event.h"
#include "SceneNode.h"
#include "GameObjectBase.h"
#include "Tools/RemoveSpecialMemberFunctionUtils.h"
#include "EngineSystems/TimeSystem/TimeSystem.h"

namespace Engine
{
	//forward declarations
	class SystemBase;
	class WindowSystem;
	class AssetSystem;
	class LevelSystem;
	class PlayerSystem;
	class ParticleSystem;
	class RNGSystem;
	class AutomatedTestSystem;
	class DebugRenderSystem;
	class RenderSystem;
	class CurveManager;
	class AudioSystem;
	class CheatSystemBase;

	class Window;

	struct TickGroups;
	class TickGroupManager;

	//////////////////////////////////////////////////////////////////////////////////////
	//struct EngineConstants
	//{
	//	int8_t RENDER_DELAY_FRAMES = 0;
	//	uint32_t MAX_DIR_LIGHTS = 4;
	//};
	//////////////////////////////////////////////////////////////////////////////////////
	struct EngineBaseIdentityKey : public RemoveCopies, public RemoveMoves
	{
		friend class EngineBase; //only the game base can construct this.
	private:
		EngineBaseIdentityKey() {}
	};
	//////////////////////////////////////////////////////////////////////////////////////

	/**
		The Game Base class; this is the root to the game systems and has a static getter.
		Understanding this class should be an early step in understanding the systems of
		this engine.
	*/
	class EngineBase : public GameObjectBase
		, public IRemoveCopies
		, public IRemoveMoves
	{
		/////////////////////////////////////////////////////////////////////////////////////
		//  Construction
		/////////////////////////////////////////////////////////////////////////////////////
	public:
		EngineBase();
		~EngineBase();

		/////////////////////////////////////////////////////////////////////////////////////
		//  Base Class Singleton
		/////////////////////////////////////////////////////////////////////////////////////
	public:
		static EngineBase& get();

	private:
		/** It is preferred to make a static getter in the implemented subclass, but this
			exists to avoid having systems know about concrete class types*/
		static EngineBase* RegisteredSingleton;

		/////////////////////////////////////////////////////////////////////////////////////
		//  Root
		/////////////////////////////////////////////////////////////////////////////////////
	private:
		sp<SceneNode> rootNode = new_sp<SceneNode>();
	public:
		inline const sp<SceneNode>& getRoot() const { return rootNode; }
		void addToRoot(const sp<SceneNode>& child);


		/////////////////////////////////////////////////////////////////////////////////////
		//  START UP / SHUT DOWN
		/////////////////////////////////////////////////////////////////////////////////////
	public: //starting system
		/** Starts up systems and begins game loop */
		void start(const std::function<void()>& initializationFunc = []() {});
	private:
		void end(); //end will happy automatically
	public:
		void startShutdown();
		static bool isEngineShutdown();
		bool isExiting() { return bExitGame; };
		Event<> onShutdownInitiated;
	protected:
		/** Child game classes should set up pre-gameloop state here.
			#return value Provide an initial primary window on startup.	*/
		virtual sp<Window> makeInitialWindow();
		virtual void startUp() {};
		virtual void onShutDown() {};
	private: //starting systems
		bool bStarted = false;
		bool bExitGame = false;


		//////////////////////////////////////////////////////////////////////////////////////
		//  GAME LOOP
		//////////////////////////////////////////////////////////////////////////////////////
	public:
		Event<> onGameloopBeginning;
		Event<> onShutdownGameloopTicksOver;
		Event<float /*deltaSec*/> onPreGameloopTick;
		Event<float /*deltaSec*/> onPostGameloopTick;
		Event<float /*deltaSec*/> onRenderDispatch;
		Event<float /*deltaSec*/> onRenderDispatchEnded;
		Event<uint64_t /*endingFrameNumber*/> onFrameOver;

	private:
		void tickGameloop_EngineBase();
		static void htmlTickMain();
	protected:
		virtual void tickGameLoop(float /*deltaTimeSecs*/){};
		//virtual void cacheRenderDataForCurrentFrame(struct RenderData& frameRenderData){};
		virtual void renderLoop_begin(float /*deltaTimeSecs*/){};
		virtual void renderLoop_end(float /*deltaTimeSecs*/){};

		//////////////////////////////////////////////////////////////////////////////////////
		//  SYSTEMS 
		//		Each system has an explicit getter to for a reason. The core of this engine 
		//		and the systems at play can be understood by simply reading this header. 
		//		if making a templated getter (like with game components) will make it harder
		//		to get a bigger picture of the interplay of systems, in my opinion.
		//////////////////////////////////////////////////////////////////////////////////////
	public:
		//System getters (to prevent circular dependencies, be sure to use forward declared references)
		//todo perhaps remove these accessors in favor of system static getters (see window and render system headers)
		inline WindowSystem& getWindowSystem() noexcept { return *windowSystem; }
		inline RenderSystem& getRenderSystem() noexcept { return *renderSystem; }
		//inline AssetSystem& getAssetSystem() noexcept { return *assetSystem; }
		//inline LevelSystem& getLevelSystem() noexcept { return *levelSystem; }
		//inline PlayerSystem& getPlayerSystem() noexcept { return *playerSystem; }
		//inline ParticleSystem& getParticleSystem() noexcept { return *particleSystem; }
		//inline RNGSystem& getRNGSystem() noexcept { return *systemRNG; }
		//inline DebugRenderSystem& getDebugRenderSystem() noexcept { return *debugRenderSystem; }
		//inline AutomatedTestSystem& getAutomatedTestSystem() noexcept { return *automatedTestSystem; };
		//inline CheatSystemBase& getCheatSystem() { return *cheatSystem; }
		//inline CurveSystem& getCurveSystem() { return *curveSystem; }
		//inline AudioSystem& getAudioSystem() { return *audioSystem; }
	private:
		void createEngineSystems();
		/**polymorphic systems require virtual override to define class. If nullptr detected these systems should create a default instance.*/
		virtual sp<RenderSystem> createRenderSystemSubclass();
		void validateSystemSpawned(const sp<SystemBase>& system);
		//virtual sp<CheatSystemBase> createCheatSystemSubclass() { return nullptr; }
		//virtual sp<CurveSystem> createCurveSystemSubclass();
		//virtual sp<AudioSystem> createAudioSystemSubclass();
	protected:
		virtual void onRegisterCustomSystem() {};
		void RegisterCustomSystem(const sp<SystemBase>& system);
	public:
		void subscribePostRender(const sp<SystemBase>& system); // this isn't as encapsulated as I'd like, but will not likely be an issue 
	private:
		bool bCustomSystemRegistrationAllowedTimeWindow = false;

	private: //systems
		sp<WindowSystem> windowSystem;
		sp<RenderSystem> renderSystem;
		//sp<AudioSystem> audioSystem;
		//sp<AssetSystem> assetSystem;
		//sp<LevelSystem> levelSystem;
		//sp<PlayerSystem> playerSystem;
		//sp<ParticleSystem> particleSystem;
		//sp<RNGSystem> systemRNG;
		//sp<AutomatedTestSystem> automatedTestSystem;
		//sp<DebugRenderSystem> debugRenderSystem;
		//sp<CheatSystemBase> cheatSystem;
		//sp<CurveSystem> curveSystem;

		std::set< sp<SystemBase> > systems;
		std::set< sp<SystemBase> > postRenderNotifys;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Constants
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//public:
	//	static inline const EngineConstants& getConstants() { return EngineBase::get().configuredConstants; }
	//	/** Game subclasses can only configure game constants at this point early in initialization
	//		@Warning No systems will be initialized or available when this virtual is called.
	//	*/
	//	virtual void onInitEngineConstants(EngineConstants& config) {};
	//private:
	//	EngineConstants configuredConstants;

		//////////////////////////////////////////////////////////////////////////////////////
		// frame id
		//////////////////////////////////////////////////////////////////////////////////////
	public:
		uint64_t getFrameNumber() const noexcept { return frameNumber; }
	private:
		uint64_t frameNumber = 0;

		/////////////////////////////////////////////////////////////////////////////////////
		// framerate limiting
		/////////////////////////////////////////////////////////////////////////////////////
	private:
		double lastFrameTime = 0.f;
		void framerateSleep(); //AMD drivers do not appear to limit framerate like nvidia. 
	protected:
		bool bEnableFramerateLimit = true;
		size_t targetFramesPerSecond = 61;


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Identity Key
		//		Systems that want to restrict function calls to EngineBase without friending can require this object as a key
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	protected: //grant subclasses access to this key. This prevents anyone from passing the EngineBase object as a key.
		EngineBaseIdentityKey identityKey;

		//////////////////////////////////////////////////////////////////////////////////////
		//  Time
		//////////////////////////////////////////////////////////////////////////////////////
	public:
		TimeSystem& getTimeSystem() { return timeSystem; }
		TimeManager& getSystemTimeManager() { return *systemTimeManager; }
		TimeManager& getGameTimeManager() { return *gameTimeManager; }
		TickGroups& tickGroups() { return *tickGroupData; }
		TickGroupManager& getTickGroupManager() { return *tickGroupManager; }
	private:
		void registerTickGroups();
		virtual sp<TickGroups> onRegisterTickGroups();
	private: //time management 
		/** Time management needs to be separate from systems since their tick relies on its results. */
		TimeSystem timeSystem;
		sp<TimeManager> systemTimeManager;
		sp<TimeManager> gameTimeManager;
		sp<TickGroups> tickGroupData = nullptr;
		sp<TickGroupManager> tickGroupManager = nullptr;
		bool bTickGoupsInitialized = false;
	};


}
