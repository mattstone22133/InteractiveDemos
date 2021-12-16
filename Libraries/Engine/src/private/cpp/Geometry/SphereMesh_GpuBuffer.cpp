#include "Geometry/SphereMesh_GpuBuffer.h"
#include "Geometry/SphereGenerator.h"

namespace Engine
{
	SphereMesh_GpuBuffer::SphereMesh_GpuBuffer()
	{
		SphereGenerator sphereRawData{};

		setPositions(sphereRawData.getPositions());
		setNormals(sphereRawData.getNormals());

	}
}

