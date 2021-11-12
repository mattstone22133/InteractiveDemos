#include<iostream>
#include <TutorialEngine.h>
#include "GameObjectBase.h"

using namespace Engine;

int main()
{
	std::cout << "Barycentric Coordinate Started" << std::endl;

	sp<TutorialUtils::TutorialEngine> engine = new_sp<TutorialUtils::TutorialEngine>();
	engine->start();

}