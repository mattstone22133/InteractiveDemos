
#include "HelloWorld.h"
#include "Goodbye.h"
//#include "engine_linker_test.h"
#include "test/engine_linker_test.h"
//#include <src/test/engine_linker_test.h>
//#include"src/test/engine_linker_test.h"

int main()
{
    HelloWorld();
	Goodbye();

    engine_linker_test();

    return 0;
}
