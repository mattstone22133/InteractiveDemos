#pragma once

#pragma once

#include "Utils/MathUtils.h"
#include <vector>
#include <cstdint>

namespace Engine
{
	class SphereGenerator final
	{
	public:
		SphereGenerator(float tolerance = 0.014125375f);

		static int calculateNumFacets(float tolerance);
		static void generateUnitSphere(float tolerance, std::vector<glm::vec3>& vertList, std::vector<glm::vec3>& normalList, std::vector<size_t>& rowIterations_numFacets);
		static glm::vec3& rotatePointZAxis(glm::vec3& point, float rotationDegrees);
		static glm::vec3 copyAndRotatePointZAxis(const glm::vec3 toCopy, float rotationDegrees);
		static glm::vec3 copyAndRotatePointXAxis(const glm::vec3& toCopy, float rotationDegrees);

		const std::vector<glm::vec3>& getPositions() const { return vertPositions; }
		const std::vector<glm::vec3>& getNormals() const { return vertNormals; }
		const std::vector<uint32_t>& getTriangleElementIndices() const{ return triangleElementIndices; }
	private:
		std::vector<glm::vec3> vertPositions;
		std::vector<glm::vec3> vertNormals;
		std::vector<uint32_t> triangleElementIndices;
	};



}