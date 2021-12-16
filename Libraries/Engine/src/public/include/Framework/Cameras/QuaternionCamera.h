#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assert.h>

#include "CameraInterface.h"

namespace Engine
{
	class QuaternionCamera : public ICamera
	{
	public:
		enum class LookMode { Free, Orbit };
		QuaternionCamera();

	public: //static helpers
		static inline bool anyValueNAN(glm::vec3 vec) { return glm::isnan(vec.x) || glm::isnan(vec.y) || glm::isnan(vec.z); }
		static inline bool anyValueNAN(glm::vec4 vec) { return glm::isnan(vec.x) || glm::isnan(vec.y) || glm::isnan(vec.z) || glm::isnan(vec.w); };
		static inline bool anyValueNAN(glm::quat quat)
		{
			glm::bvec4 vec = glm::isnan(quat);
			return vec.x || vec.y || vec.z || vec.w;
		}
	public:
		void tick(float dt_sec);

		void handleMouseMoved(float dt_sec, const glm::vec2& deltaMouse);

		void updateBasisVectors();

		virtual glm::mat4 getView() const override
		{
			return glm::lookAt(pos, pos + -w_axis, v_axis);
		}
		virtual glm::vec3 getFront() const override { return -w_axis; }
		virtual glm::vec3 getRight() const override { return u_axis; }
		virtual glm::vec3 getUp() const override { return v_axis; }
		virtual glm::vec3 getPosition() const override { return pos; }
		virtual float getFOVy_rad() const override { return fovY_rad; }
		virtual glm::quat getRotation() const { return rotation; }
		virtual float getNear() const override { return near; }
		virtual float getFar() const override { return far; }
	public: //public so demostrations can easily tweak; tick will correct
		float cameraSpeed = 10.0f; //NDCs per second
		float rollSpeed = glm::radians<float>(180.f);
		float fovY_rad = glm::radians<float>(45.f);
		glm::vec3 pos{ 0.f, 0.f, 0.f };
		glm::quat rotation{ 1.f,0,0,0 };
		uint32_t cursorModeKey = 256; /*= GLFW_KEY_ESCAPE;*/
		float near = 0.1f;
		float far = 100.f;
	private: //tick state
		double lastX = 0, lastY = 0;
	private:
		glm::vec3 u_axis{ 1.f,0.f,0.f };
		glm::vec3 v_axis{ 0.f,1.f,0.f };
		glm::vec3 w_axis{ 0.f,0.f,1.f };
		float mouseSensitivity = 0.125f;
		float orbitDistance = 5.f;
		bool bCursorMode = true;
		bool bEnableOrbitModeSwitch = true;
		LookMode lookMode = LookMode::Free;
	};
}