#pragma once

#include<iostream>
#include<string>
#include<optional>
#include<cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "EngineSystems/RenderSystem/Rendering/GPUResource.h"
#include "Tools/RemoveSpecialMemberFunctionUtils.h"

#define ENABLE_MODERN_OPENGL_SHADERS 0

namespace Engine
{
	class Shader final : public GPUResource
		, public IRemoveCopies
		, public IRemoveMoves
	{
	public:
		struct ShaderParams
		{
			std::optional<std::string> vertex_src;
#if ENABLE_MODERN_OPENGL_SHADERS
			std::optional<std::string> tessellation_control_src;
			std::optional<std::string> tessellation_evaluation_src;
			std::optional<std::string> geometry_src;
			std::optional<std::string> compute_src;
#endif //ENABLE_MODERN_OPENGL_SHADERS
			std::optional<std::string> fragment_src;
		};

	public:
		Shader(ShaderParams& in);
		~Shader();
		void shaderCompileCheck(const char* debugShaderTypeName, /*GLuint*/uint32_t shader);
		void shaderLinkCheck();

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// API
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void use();
		uint32_t getProgramId() const { return shaderProgram; }

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Uniform Functions
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void setMat4(const char* uniformName, const glm::mat4& matrix4);
		void setInt(const char* uniformName, int value);
		void setFloat(const char* uniformName, float value);
		void setUniform4f(const char* uniform, float red, float green, float blue, float alpha);
		void setUniform4f(const char* uniform, const glm::vec4& values);
		void setUniform3f(const char* uniform, float red, float green, float blue);
		void setUniform3f(const char* uniform, const glm::vec3& vals);
		void setUniform1i(const char* uniform, int newValue);
		void setUniformMatrix4fv(const char* uniform, int numberMatrices, /*GLuint*/uint8_t transpose, const float* data);
		void setUniform1f(const char* uniformName, float value);

	private:
		virtual void onAcquireGPUResources() override;
		virtual void onReleaseGPUResources() override;
	private:
		ShaderParams initParams;
		uint32_t shaderProgram = 0;
	};

}