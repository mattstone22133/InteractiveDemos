#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>
#include <cstdint>
#include "Tools/RemoveSpecialMemberFunctionUtils.h"

#include "GPUResource.h"

namespace Engine
{
	struct TextureWrapper : public GPUResource
		, public Engine::IRemoveMoves
		, public Engine::IRemoveCopies
	{
		struct TextureInitParams
		{
			const char* exe_relative_filepath = "";
			int texture_unit = -1;
#ifdef MODERN_OPENGL
			bool useGammaCorrection = false;
#endif //MODERN_OPENGL
			bool bGenerateMips = true;
		};

		TextureWrapper(const TextureInitParams& init);
		~TextureWrapper();

	protected:
		virtual void onAcquireGPUResources() override;
		virtual void onReleaseGPUResources() override;

	public:
		uint32_t /*GLuint*/ glTextureId = 0;
		TextureInitParams params;
	};
}