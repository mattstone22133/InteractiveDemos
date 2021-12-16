#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	struct ICamera
	{
		virtual ~ICamera() {}

		virtual glm::mat4 getView() const = 0;
		virtual glm::vec3 getFront() const = 0;
		virtual glm::vec3 getRight() const = 0;
		virtual glm::vec3 getUp() const = 0;
		virtual glm::vec3 getPosition() const = 0;
		virtual float getFOVy_rad() const = 0;
		virtual float getNear() const = 0;
		virtual float getFar() const = 0;
	};
}

