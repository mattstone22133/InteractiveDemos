#pragma once
#include "GameObjectBase.h"
#include "Tools/RemoveSpecialMemberFunctionUtils.h"

namespace Engine
{
	enum class SingletonOperation : uint8_t
	{
		REGISTER,
		REMOVE,
		GET
	};

	/** Uses local statics in templates to define a single place to store a singleton.*/
	template<typename System>
	void singletoneManager(SingletonOperation op, System*& inOutSystem)
	{
		static System* staticSystem = nullptr;
		if (SingletonOperation::GET == op)
		{
			inOutSystem = staticSystem;
		}
		else if (SingletonOperation::REMOVE == op)
		{
			staticSystem = nullptr;
		}
		else if (SingletonOperation::REGISTER == op)
		{
			staticSystem = inOutSystem;
		}
	}

/** Defines the boiler plate for easily creating a static getter in a single location.
* It is recommended to put this immediately under the class name declaration so that if a base class changes, it can be easily updated.
*/
#ifndef IMPLEMENT_PUBLIC_STATIC_GET
#define IMPLEMENT_PUBLIC_STATIC_GET(ThisClass, ParentClass)\
public:\
    virtual void registerGetter(SingletonOperation Op) override\
    {\
        ParentClass::registerGetter(Op);\
        ThisClass* OutPtr = this;\
        Engine::singletoneManager(Op, OutPtr);\
    }\
public:\
    static ThisClass& get()\
	{\
		/*with proper engine initialization, this should never be null except before start up and after shutdown.*/\
		/*If you get a null crash here, perhaps a parent class isn't properly set up or the system was never initialized.*/\
		ThisClass* OutPtr = nullptr; \
		Engine::singletoneManager<ThisClass>(Engine::SingletonOperation::GET, OutPtr); \
		return *OutPtr; \
	}
#endif // IMPLEMENT_PUBLIC_STATIC_GET

	class SystemBase : public GameObjectBase
		, public IRemoveCopies
		, public IRemoveMoves
	{
		friend class EngineBase; //used to call initialize system.
	public:
		inline bool hasRegisteredStaticGet() const { return bRegisteredStaticGet; }

	protected:
		virtual void registerGetter(SingletonOperation Op);
	private:

		virtual void tick(float /*deltaSec*/) {};

		/** Called when main game systems can safely be accessed; though not all may be initialized */
		virtual void initSystem() {};

		/** Called when game is shuting down for resource releasing; resources that require other systems should prefer releasing here rather than the dtor of the system */
		virtual void shutdown() {}

		virtual void handlePostRender() {}

		/** Called after all systems have been created. 
			It is safe to cross reference to other systems.
			But those systems may not be initialized.
			Events/Delegates should be bound in order to "wait" for systems,
			rather than attempting to predetermine order of initialization.*/
		virtual void InitializeSystem() {}
	private:
		bool bRegisteredStaticGet = false;
	};

}
