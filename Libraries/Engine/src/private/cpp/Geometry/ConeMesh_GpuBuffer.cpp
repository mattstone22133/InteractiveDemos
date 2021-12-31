#include "Geometry/ConeMesh_GpuBuffer.h"

namespace Engine
{
	ConeMesh_GpuBuffer::ConeMesh_GpuBuffer(uint32_t numFaces, float height, float radius, ConeGenerator::ConeOriginGeneration OriginGenPolicy, const bool bSealCone)
	{
		ConeGenerator coneRawData(
			numFaces
			, height
			, radius
			, OriginGenPolicy
			, bSealCone);

		setPositions(coneRawData.getPositions());
		setNormals(coneRawData.getNormals());
	}

}

