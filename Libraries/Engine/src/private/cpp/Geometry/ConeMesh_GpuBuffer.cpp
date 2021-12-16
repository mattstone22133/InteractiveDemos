#include "Geometry/ConeMesh_GpuBuffer.h"

namespace Engine
{
	ConeMesh_GpuBuffer::ConeMesh_GpuBuffer(uint32_t numFaces, float height, float radius, ConeGenerator::ConeOriginGeneration OriginGenPolicy)
	{
		ConeGenerator coneRawData(numFaces, height, radius, OriginGenPolicy);

		setPositions(coneRawData.getPositions());
		setNormals(coneRawData.getNormals());
	}

}

