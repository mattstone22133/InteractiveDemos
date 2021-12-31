#pragma once

#include "TriangleMesh_GpuBuffer.h"
#include "Geometry/ConeGenerator.h"

namespace Engine
{
	class ConeMesh_GpuBuffer : public TriangleMesh_GpuBuffer
	{
	public:
		ConeMesh_GpuBuffer(uint32_t numFaces = 5, float height = 1.0f, float radius = 1.0f, ConeGenerator::ConeOriginGeneration OriginGenPolicy = ConeGenerator::ConeOriginGeneration::BASE, const bool bSealCone = true);
	};


}