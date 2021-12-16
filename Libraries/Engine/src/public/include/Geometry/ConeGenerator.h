#pragma once

#include "Utils/MathUtils.h"
#include <vector>

namespace Engine
{
	class ConeGenerator
	{
	public:
		enum class ConeOriginGeneration
		{
			BASE, CENTER, TIP
		};

	public:
		ConeGenerator(uint32_t numFaces = 5, float height = 1.0f, float radius = 1.0f, ConeOriginGeneration OriginGenPolicy = ConeOriginGeneration::BASE)
		{
			using namespace glm;

			numFaces = glm::max<uint32_t>(numFaces, 3); //clamp it so it isn't a plane

			const vec3 pointingAxis = glm::vec3(1.f, 0.f, 0.f);

			const vec3 origin = [&]() {
				switch (OriginGenPolicy)
				{
				case ConeOriginGeneration::BASE:
					return glm::vec3(0.f);
				case ConeOriginGeneration::CENTER:
					return pointingAxis * -height * 0.5f;
				case ConeOriginGeneration::TIP:
				default:
					return pointingAxis * -height;
				}
			}();

			const float faceRotation = 360.0f / float(numFaces);
			const vec3 vecToRotate = origin + vec3(0.f, radius, 0.f);
			const vec3 tipPoint = origin + pointingAxis * height;

			for (size_t face = 0; face < numFaces; ++face)
			{
				const float startRotation = faceRotation * face;
				const float endRotation = faceRotation * (face + 1);

				const vec3 startPoint = vecToRotate * glm::angleAxis(startRotation, pointingAxis);
				const vec3 endPoint = vecToRotate * glm::angleAxis(endRotation, pointingAxis);
				const vec3 normal = glm::normalize(glm::cross(endPoint - startPoint, tipPoint - startPoint));

				positions.push_back(startPoint);
				normals.push_back(normal);

				positions.push_back(endPoint);
				normals.push_back(normal);

				positions.push_back(tipPoint);
				normals.push_back(normal);
			}
		}

		const std::vector<glm::vec3>& getPositions() const { return positions; }
		const std::vector<glm::vec3>& getNormals() const { return normals; }
	private:
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
	};



}