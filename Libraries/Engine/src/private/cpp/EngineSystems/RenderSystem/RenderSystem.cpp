#include <algorithm>

#include "Engine.h"
#include "EngineSystems/RenderSystem/RenderSystem.h"
#include "EngineSystems/WindowSystem/WindowSystem.h"
#include "Framework/Cameras/QuaternionCamera.h"
#include "Utils/DataStructureUtils.h"

#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"

namespace Engine
{
	void RenderSystem::initSystem()
	{
		createRenderData();

		sp<ICamera> defaultCamera = createDefaultRenderCamera();
		if (defaultCamera)
		{
			renderCameras.resize(1);
			renderCameras[0] = defaultCamera;
		}
	}

	void RenderSystem::createRenderData()
	{
		//override this to instantiate specific subclass of render data.
		frameRenderData = new_sp<FrameRenderData>();
	}

	sp<ICamera> RenderSystem::createDefaultRenderCamera()
	{
		return new_sp<QuaternionCamera>();
	}

	void RenderSystem::tick(float /*dt_sec*/)
	{
		//SystemBase::tick(dt_sec);

	}

	void RenderSystem::clearScreen()
	{
		ec(glEnable(GL_DEPTH_TEST));
		ec(glEnable(GL_STENCIL_TEST)); //enabling to ensure that we clear stencil every frame (may not be necessary)
		ec(glStencilMask(0xff)); //enable complete stencil writing so that clear will clear stencil buffer (also, not tested if necessary)

		ec(glClearColor(0.f, 0.f, 0.f, 1.f));
		ec(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		ec(glStencilMask(0x0)); //we cleared stencil buffer, stop writing to stencil buffer.
		ec(glDisable(GL_STENCIL_TEST)); //only enable stencil test on demand
	}

	void RenderSystem::prepareRenderDataForFrame(std::size_t renderCameraIndex/*= 0*/)
	{
		frameRenderData->window = WindowSystem::get().getPrimaryWindow();
		if (frameRenderData->window)
		{
			std::pair<int, int> fbDimension = frameRenderData->window->getFramebufferSize();
			frameRenderData->fbWidth = fbDimension.first;
			frameRenderData->fbHeight = fbDimension.second;

			frameRenderData->camera = Engine::isValidIndex(renderCameraIndex, renderCameras) ? renderCameras[renderCameraIndex] : nullptr;
			//frameRenderData->camera = Engine::getAtIndexSafe(renderCameraIndex, renderCameras);
 
			if (frameRenderData->camera)
			{
				const float aspect = frameRenderData->window->getAspect();
				const float near = frameRenderData->camera->getNear();
				const float far = frameRenderData->camera->getFar();

				frameRenderData->view = frameRenderData->camera->getView();
				frameRenderData->projection = glm::perspective(frameRenderData->camera->getFOVy_rad(), aspect, near, far);
				frameRenderData->projection_view = frameRenderData->projection * frameRenderData->view;

			}
		}
	}

	bool RenderSystem::setRenderCamera(const sp<ICamera>& newCamera, const std::size_t cameraIndex/*=0*/)
	{
		if (cameraIndex < MAX_NUM_RENDER_CAMERAS)
		{
			if (renderCameras.size() < cameraIndex)
			{
				renderCameras.resize(cameraIndex + 1);
			}
			onPrimaryViewCameraChanging.broadcast(renderCameras[cameraIndex], newCamera, cameraIndex);
			renderCameras[cameraIndex] = newCamera;
			return true;
		}
		else
		{
			return false;
		}
	}

	const sp<Engine::ICamera>& RenderSystem::getRenderCamera(const size_t index /*= 0*/) const
	{
		if (index < renderCameras.size())
		{
			return renderCameras[index];
		}

		static sp<ICamera> nullCamera = nullptr;
		return nullCamera;
	}

}
