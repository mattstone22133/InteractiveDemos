#pragma once
#include "GameObjectBase.h"
#include "Tools/RemoveSpecialMemberFunctionUtils.h"

namespace Engine
{
	class SystemBase : public GameObjectBase
		, public RemoveCopies
		, public RemoveMoves
	{
		friend class EngineBase; //used to call initialize system.

	private:

		virtual void tick(float deltaSec) {};

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
	};

}
