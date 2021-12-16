#pragma once

#include <memory>
#include "EngineSmartPointers.h"


#define GENERATE_CLASS_BOILERPLATE(ClassName, BaseName)\
using Super = BaseName;\
using ThisClass = ClassName;



///////////////////////////////////////////////////////////////////////////////////////////////////
//These forward declarations are carefully placed here to avoid circle includes.
//new_sp is desired to be responsible for calling post construct, and therefore 
//needs to know of the type "GameEntity". So that file will include "GameEntity"
//However, game entity needs to know the smart pointer aliases to friend them, 
//hence the forward declarations
//
//Note, Some of below are the real type aliases (eg see sp)

	//shared pointer
template<typename T>
using sp = std::shared_ptr<T>;

//weak pointer
template<typename T>
using wp = std::weak_ptr<T>;

//unique pointer
template<typename T>
using up = std::unique_ptr<T>;


template<typename T, typename... Args>
sp<T> new_sp(Args&&... args);
template<typename T, typename... Args>
up<T> new_up(Args&&... args);


struct GameObjectConstructorFriendHelper;

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{

	// forward declare event so that it does not need to be included creating circular include.
	template<typename... Args>
	class Event;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Base class for most heap based objects
	// 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class GameObjectBase : public std::enable_shared_from_this<GameObjectBase>
	{
		#define sp_this() Engine::GameObjectBase::sp_this_impl<std::remove_reference<decltype(*this)>::type>()

	public:
		/** Game Objects will all have virtual destructors to avoid easy-to-miss mistakes*/
		GameObjectBase();
		virtual ~GameObjectBase() {}

	private:
		/** lifetime pointers get a special event that fires before the destroyed event; this prevents race conditions that may be rely on lifetime pointer features; this must remain private. See notes at broadcast. */
		const sp< Event<> > onLifetimeOverEvent;
	public:
		const sp< Event<const sp<GameObjectBase>&> > onDestroyedEvent;//pointer because this will create circular include if we define type here; Events operator on game entities
		bool isPendingDestroy() const { return bPendingDestroy; }

		/** WARNING: think twice before using this; if you're given a ref/rawptr then the API may be trying to prevent you from holding a reference
		 * subclasses can deny this request by overriding the virtual method to return nullptr.*/
		wp<GameObjectBase> requestReference() { return sp_this(); }
		/** WARNING: raw pointers may be dangling. this is only safe in that it does a dynamic cast; but that will not catch dangling pointers.
		   the functions should only be used when raw pointer was immediately obtained -- and not if raw pointer was set at some previous point in history*/
		template<typename T> wp<T> requestTypedReference_Safe() { return std::dynamic_pointer_cast<T>(sp_this()); }
		template<typename T> wp<T> requestTypedReference_Nonsafe() { return std::static_pointer_cast<T>(sp_this()); }

		/* Marks an entity for pending destroy */
		void destroy();

	protected:
		/* new_sp will call this function after the object has been created, allowing GameEntities
		   to subscribe to delegates immediately after construction*/
		virtual void postConstruct() {};

		virtual void onDestroyed();

		/** Not intended to be called directly; please use macro "sp_this()" to avoid specifying template types*/
		template<typename T>
		sp<T> sp_this_impl()
		{
			//static cast safe because this must be called from derived classes 
			//static cast for speed; does not inccur RTTI overhead of dynamic cast
			return std::static_pointer_cast<T>(shared_from_this());
		}
	private:
		bool bPendingDestroy = false;

	private:
		friend struct ::GameObjectConstructorFriendHelper;

		template<typename T>
		friend class LifetimePointer; //TODO perhaps remove this if it isn't ported it over. Not sure I like the implementation of it.

	public:
		struct CleanKey { friend class EngineBase;  private: CleanKey() {} };
		static void cleanupPendingDestroy(CleanKey);
	};

}

struct GameObjectConstructorFriendHelper
{
	/** Since friending a template function (new_sp) with global namespacee, within a class in engine namespace (GameObjectBase) causes compilation issues, this class is a work around.
		This class is not at template, and therefore can be friend'd in GameObjectBase.
		Since this is in the same namespace (global) as new_sp, it can itself friend new_sp without compile errors.
		This means that ONLY new_sp<>() can do postConstruct on the gameobjectbase. 
		Which properly encapsulates postConstruct so that it can only be called when the new_sp factor function is used. (or subclass calls parent)
		*/
private:
	//allow new_sp to call into this. this gets around issues with friending new_sp outside of engine namespace when gameobjectbase is in engine namespace
	template<typename T, typename... Args>
	friend sp<T> new_sp(Args&&... args);

	static inline void doPostConstruct(Engine::GameObjectBase& Obj)
	{
		Obj.postConstruct();
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper methods for constructing shared pointers and game object subclasses.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T, typename... Args>
sp<T> new_sp(Args&&... args)
{
	if constexpr (std::is_base_of<Engine::GameObjectBase, T>::value)
	{
		sp<T> newObj = std::make_shared<T>(std::forward<Args>(args)...);

		//safe cast because of type-trait
		Engine::GameObjectBase* newEntity = static_cast<Engine::GameObjectBase*>(newObj.get());
		GameObjectConstructorFriendHelper::doPostConstruct(*newEntity); //work around since new_sp is out of engine namespace
		return newObj;
	}
	else
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	//return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
up<T> new_up(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}
