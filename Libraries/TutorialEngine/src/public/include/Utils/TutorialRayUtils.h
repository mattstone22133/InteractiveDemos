#pragma once

#include <vector>
#include <optional>
#include <iostream>

#include "Utils/MathUtils.h"
#include "GameObjectBase.h"

struct Triangle
{
	glm::vec3 pntA;
	glm::vec3 pntB;
	glm::vec3 pntC;
};

struct Ray
{
	float T = 1.0f;
	glm::vec3 start = glm::vec3(0, 0, 0);
	glm::vec3 dir = glm::vec3(1, 0, 0);
};

namespace RayTests
{
	float initForIntersectionTests();

	bool triangleIntersect(const Ray& ray, const Triangle& tri, glm::vec3& intersectPnt, float& outT);

	std::optional<glm::vec3> rayPlaneIntersection(const Ray& ray, const glm::vec3& planeNormal_n, const glm::vec3& planePoint);
};

class ObjectPicker
{
public:
	static Ray generateRayFromCamera(
		const glm::vec2& screenResolution, const glm::vec2& clickPoint,
		const glm::vec3& cameraPos_w, const glm::vec3& cameraUp_n, const glm::vec3& cameraRight_n, const glm::vec3& cameraFront_n,
		float FOVy_deg, float aspectRatio
	);
};

struct TriangleList
{
	TriangleList() {}
	TriangleList(const std::vector<Triangle>& inLocalTriangles)
		: localTriangles(inLocalTriangles)
	{}
	virtual ~TriangleList() {}
	virtual void transform(const glm::mat4& model);
	std::vector<Triangle> worldTriangles;
	const std::vector<Triangle>& getLocalTriangles() const { return localTriangles; }
private: //private because const will prevent default copy
	std::vector<Triangle> localTriangles;
};

//#TODO remove this if possible
template<typename Owner>
struct OwnedTriangleList : public TriangleList
{
	OwnedTriangleList(const std::vector<Triangle>& inLocalTriangles) : TriangleList(inLocalTriangles) {}
	virtual ~OwnedTriangleList() {}
	virtual void onUpdated() {}
	Owner* owner = nullptr;
};

/**
	Note this is not necessarily intended to be efficient. It is more of a debug utility.
	For example, creating/destorying std::vectors on fly has overhead.
	You can optimize this by reserving the vector and reusing the query struct.
*/
struct CameraRayCastData
{
	std::optional<glm::vec3> camPos = std::nullopt;
	std::optional<glm::vec3> camRight_n = std::nullopt;
	std::optional<glm::vec3> camUp_n = std::nullopt;
	std::optional<glm::vec3> camFront_n = std::nullopt;
	std::optional<float> fovY_deg;
	struct GLFWwindow* window = nullptr; //do not include platform types here; forward declare or move.

	bool validate() const;
};

struct CameraRayCastData_Triangles : public CameraRayCastData
{
	std::vector<const TriangleList*> objectList;
	bool validate() const;
};

struct TriangleObjectRaycastResult
{
	float hitT = -1.f;
	glm::vec3 hitPoint{0.f};
	const TriangleList* hitObject = nullptr;
	Ray castRay{};
};

std::optional<Ray> rayCast(const CameraRayCastData& rd);
std::optional<TriangleObjectRaycastResult> rayCast_TriangleObjects(const CameraRayCastData_Triangles& rd);