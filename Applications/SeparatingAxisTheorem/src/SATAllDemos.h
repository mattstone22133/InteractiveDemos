#pragma once

#include "Framework/InteractableDemoBase.h"
#include "SATDemoInterface.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////

#if WITH_SAT_DEMO_BUILD
namespace SAT
{
	class SATDemoApplication : public TutorialEngine::InteractableDemoBase
	{
		using Super = TutorialEngine::InteractableDemoBase;

	protected:
		virtual void init() override;
		virtual void render_game(float dt_sec) override;
		virtual void render_UI() override;
		virtual void inputPoll(float dt_sec) override;
		virtual void tick(float dt_sec) override;

		virtual void gatherInteractableCubeObjects(std::vector<const TutorialEngine::TriangleList_SNO*>& objectList) override {}

		void handleEngineShuttingDown();

	private:
		Deprecated_InputTracker input;
		sp<ISATDemo> activeDemo = nullptr;
		std::vector<sp<ISATDemo>> demos;
	};
}
#endif //WITH_SAT_DEMO_BUILD