
#include "HelloWorld.h"
#include "Goodbye.h"
#include "test/engine_linker_test.h"

int main()
{
    HelloWorld();
	Goodbye();

    engine_linker_test();

    return 0;
}
