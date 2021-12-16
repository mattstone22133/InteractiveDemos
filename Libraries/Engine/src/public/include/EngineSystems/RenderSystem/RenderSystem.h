#pragma once
#include <vector>
#include <cstddef> //size_t
#include "EngineSystems/SystemBase.h"

//#include "Tools/Algorithms/AmortizeLoopTool.h"
#include "Transform.h"
#include "Event.h"
#include "GameObjectBase.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTE: this class was ported but seemed largely unused and specific to opengl3.3. Perhaps it should just be deleted.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
	struct RenderData;
	class EngineBase;

	/** Represents a cached state of all rendering variables for the given frame. */
	class FrameRenderData : public GameObjectBase
	{
	public:
		sp<class Window> window = nullptr;
		sp<struct ICamera> camera = nullptr;
		glm::mat4 view{ 1.f };
		glm::mat4 projection{ 1.f };
		glm::mat4 projection_view{ 1.f };
		int fbHeight = 1;
		int fbWidth = 1;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// The encapsulated render system
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class RenderSystem : public SystemBase
	{
		IMPLEMENT_PUBLIC_STATIC_GET(RenderSystem, SystemBase);
	public:
		Event<float /*deltaSec*/> onRenderDispatch;
	private:
		sp<FrameRenderData> frameRenderData = nullptr;
		std::vector<sp<ICamera>> renderCameras;
	protected:
		std::size_t MAX_NUM_RENDER_CAMERAS = 16;
	public:
		virtual void clearScreen();
		virtual void prepareRenderDataForFrame(std::size_t renderCameraIndex = 0);
		const sp<FrameRenderData>& getFrameRenderData() const { return frameRenderData; }

		bool setRenderCamera(const sp<ICamera>& newCamera, const std::size_t cameraIndex=0);
		const sp<ICamera>& getRenderCamera(const size_t index = 0) const;
		Event<const sp<ICamera>& /*oldCamera*/, const sp<ICamera>& /*newCamera*/, const std::size_t& /*cameraIndex*/> onPrimaryViewCameraChanging;

	protected:
		virtual void tick(float dt_sec) override;;
	private:
		/** Private to allow deciding whether to return this as constant data or mutable data.*/
		//RenderData* getFrameRenderData(uint64_t frameNumber);
	private:
		virtual void initSystem() override;
	private:
		virtual void createRenderData();
		virtual sp<ICamera> createDefaultRenderCamera();

	};
}