#include<EngineSystems/SystemBase.h>

void Engine::SystemBase::registerGetter(SingletonOperation Op)
{
	if (SingletonOperation::REGISTER == Op)
	{
		bRegisteredStaticGet = true;
	}
	else if (SingletonOperation::REMOVE == Op)
	{
		bRegisteredStaticGet = false;
	}
}
