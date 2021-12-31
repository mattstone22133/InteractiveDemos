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
		ConeGenerator(uint32_t numFaces = 5, float height = 1.0f, float radius = 1.0f, ConeOriginGeneration OriginGenPolicy = ConeOriginGeneration::BASE, const bool bSealConeEnd = true)
		{
			using namespace glm;

			numFaces = glm::max<uint32_t>(numFaces, 3); //clamp it so it isn't a plane

			//const vec3 pointingAxis = glm::vec3(1.f, 0.f, 0.f);
			//const vec3 vecToRotate = vec3(0.f, radius, 0.f);

			//const vec3 pointingAxis = glm::vec3(0.f, 1.f, 0.f);
			//const vec3 vecToRotate = vec3(radius, 0.f, 0.f);

			const vec3 pointingAxis = glm::vec3(0.f, 0.f, 1.f);
			const vec3 vecToRotate = vec3(0.f, radius, 0.f);

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

			const float faceRotation = glm::radians( 360.0f / float(numFaces) );
			const vec3 scaledPointingAxis = pointingAxis * height;
			const vec3 tipPoint = origin + scaledPointingAxis;
			const vec3 basePoint = tipPoint - scaledPointingAxis; //makes math simpler for all origin configs to use relative to tip
			const vec3 normalConeEnd = -pointingAxis;

			for (size_t face = 0; face < numFaces; ++face)
			{
				const float startRotation = faceRotation * face;
				const float endRotation = faceRotation * (face + 1);

				const vec3 startPoint = origin + vecToRotate * glm::angleAxis(startRotation, pointingAxis);
				const vec3 endPoint = origin + vecToRotate * glm::angleAxis(endRotation, pointingAxis);
				const vec3 normalSideFace = glm::normalize(glm::cross(tipPoint - startPoint, endPoint - startPoint));

				// build the side face
				positions.push_back(startPoint);
				normals.push_back(normalSideFace);

				positions.push_back(endPoint);
				normals.push_back(normalSideFace);

				positions.push_back(tipPoint);
				normals.push_back(normalSideFace);

				if (bSealConeEnd)
				{
					positions.push_back(startPoint);
					normals.push_back(normalConeEnd);

					positions.push_back(endPoint);
					normals.push_back(normalConeEnd);

					positions.push_back(basePoint);
					normals.push_back(normalConeEnd);
				}
			}
		}

		const std::vector<glm::vec3>& getPositions() const { return positions; }
		const std::vector<glm::vec3>& getNormals() const { return normals; }
	private:
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
	};



}