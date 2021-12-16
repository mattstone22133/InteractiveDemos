#include "Geometry/SphereGenerator.h"
#include <stdexcept>

namespace Engine
{
	SphereGenerator::SphereGenerator(float tolerance /*= 0.014125375f*/)
	{
		using namespace glm;

		std::vector<vec3> sphereVerts;
		std::vector<vec3> sphereNormals;

		std::vector<size_t> rowIterations_numFacets;
		generateUnitSphere(tolerance, sphereVerts, sphereNormals, rowIterations_numFacets);

		size_t latitudeNum = rowIterations_numFacets[0];
		size_t numFacetsInCircle = rowIterations_numFacets[1];
		//numFacetsInCircle = Math.max(numFacetsInCircle, 4); 

		// for every facet in circle, we have a strip of the same number of facets.
		// Since we have triangles for every two facet layers, there are numFacetsInCircle - 1 triangle strips.
		// Every facet contributes to 2 triangles.
		size_t numTriangles = numFacetsInCircle * (latitudeNum - 1) * 2;

		std::vector<float> verts(sphereVerts.size() * 3);
		std::vector<float> normals(sphereNormals.size() * 3);
		std::vector<uint32_t> triangles(numTriangles * 3);

		//this time I will be doing a whole vertex offset to avoid multiplications within the indices of the array. 
		size_t vertOffset = 0;
		size_t triangleElementOffset = 0;

		for (size_t verticalIdx = 0; verticalIdx < latitudeNum; ++verticalIdx)
		{
			size_t rowOffset = verticalIdx * numFacetsInCircle;
			for (size_t horriIdx = 0; horriIdx < numFacetsInCircle; ++horriIdx)
			{
				size_t eleIdx = rowOffset + horriIdx;
				vec3 vertex = sphereVerts[eleIdx];
				vec3 normal = sphereNormals[eleIdx];

				//vertex and normals share same offset, so it is updated after.
				verts[vertOffset] = vertex.x;
				verts[vertOffset + 1] = vertex.y;
				verts[vertOffset + 2] = vertex.z;
				normals[vertOffset] = normal.x;
				normals[vertOffset + 1] = normal.y;
				normals[vertOffset + 2] = normal.z;
				vertOffset += 3;

				//connect layer's triangles
				if (verticalIdx != 0)
				{ //first layer has nothing to connect to.
				  // How faces between rings are set up as triangles.
				  // r1v1 --- r1v2   triangle1 = r2v1, r1v2, r2v2
				  //   |   /    |    triangle2 = r2v1, r1v1, r1v2
				  // r2v1 --- r2v2   point rotation matters for OpenGL
					size_t r2v1 = horriIdx + rowOffset;
					size_t r2v2 = (horriIdx + 1) % numFacetsInCircle + rowOffset;
					size_t r1v1 = horriIdx + (rowOffset - numFacetsInCircle);
					size_t r1v2 = (horriIdx + 1) % numFacetsInCircle + (rowOffset - numFacetsInCircle);

					// triangle 1
					triangles[triangleElementOffset]	 = uint32_t(r2v1);
					triangles[triangleElementOffset + 1] = uint32_t(r2v2);
					triangles[triangleElementOffset + 2] = uint32_t(r1v2);

					// triangle 2
					triangles[triangleElementOffset + 3] = uint32_t(r1v1);
					triangles[triangleElementOffset + 4] = uint32_t(r2v1);
					triangles[triangleElementOffset + 5] = uint32_t(r1v2);
					triangleElementOffset += 6;
				}
			}
		}

		this->vertPositions.clear();
		this->vertNormals.clear();
		for (size_t x = 0; x < verts.size(); x += 3)
		{
			size_t y = x + 1;
			size_t z = x + 2;
			if (z < normals.size() && z < verts.size())
			{
				vertPositions.emplace_back(verts[x], verts[y], verts[z]);
				vertNormals.emplace_back(normals[x], normals[y], normals[z]);
			}
			else
			{
				throw std::runtime_error("logic error in sphere generation, out of bounds");
			}
		}
		this->triangleElementIndices = triangles;
	}

	int SphereGenerator::calculateNumFacets(float tolerance)
	{
		return static_cast<int>(round((glm::pi<float>() / (4 * tolerance))));
	}

	void SphereGenerator::generateUnitSphere(float tolerance, std::vector<glm::vec3>& vertList, std::vector<glm::vec3>& normalList, std::vector<size_t>& rowIterations_numFacets)
	{
		using namespace glm;

		tolerance = clamp(tolerance, 0.01f, 0.2f);
		int numFacets = calculateNumFacets(tolerance);
		float rotationDegrees = 360.0f / (numFacets);
		vec3 srcPoint(1, 0, 0);


		//generate a 2D grid of vertices that will be mapped to a sphere.
		int numRows = (numFacets / 2) + 1;
		float rowRotationDegrees = 180.0f / (numRows - 1);

		for (int row = 0; row < numRows; ++row)
		{
			vec3 latitudePnt = copyAndRotatePointZAxis(srcPoint, row * rowRotationDegrees);
			for (int col = 0; col < numFacets; ++col)
			{
				//for a unit sphere the normals match the vertex locations.
				vec3 finalPnt = copyAndRotatePointXAxis(latitudePnt, col * rotationDegrees);
				vertList.push_back(finalPnt);
				normalList.push_back(finalPnt);
			}
		}

		//re-orient sphere so poles are at top and bottom
		for (unsigned int i = 0; i < vertList.size(); ++i)
		{
			vec3& vertex = vertList[i];
			vec3& normal = normalList[i];

			rotatePointZAxis(vertex, 90);
			normal = vertex;
		}

		rowIterations_numFacets.clear();
		rowIterations_numFacets.push_back(numRows);
		rowIterations_numFacets.push_back(numFacets);
	}

	glm::vec3& SphereGenerator::rotatePointZAxis(glm::vec3& point, float rotationDegrees)
	{
		using namespace glm;

		glm::mat4 rotationMatrix(1.f);
		rotationMatrix = rotate(rotationMatrix, glm::radians(rotationDegrees), vec3(0, 0, 1.f));

		//since we're rotating a point, we take on a 1 in the w component for the transformation
		point = vec3(rotationMatrix * vec4(point, 1.f));

		return point;
	}

	glm::vec3 SphereGenerator::copyAndRotatePointZAxis(const glm::vec3 toCopy, float rotationDegrees)
	{
		using namespace glm;

		vec3 copy(toCopy);
		//expecting RVO to occur
		return rotatePointZAxis(copy, rotationDegrees);
	}

	glm::vec3 SphereGenerator::copyAndRotatePointXAxis(const glm::vec3& toCopy, float rotationDegrees)
	{
		using namespace glm;

		vec3 result(toCopy);

		glm::mat4 rotationMatrix(1.f);
		rotationMatrix = rotate(rotationMatrix, glm::radians(rotationDegrees), vec3(1.f, 0, 0));

		result = vec3(rotationMatrix * vec4(result, 1.f));

		//expecting RVO
		return result;
	}

}
