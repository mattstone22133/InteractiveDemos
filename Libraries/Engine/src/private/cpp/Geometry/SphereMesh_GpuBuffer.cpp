#include "Geometry/SphereMesh_GpuBuffer.h"
#include "Geometry/SphereGenerator.h"

namespace Engine
{
	SphereMesh_GpuBuffer::SphereMesh_GpuBuffer()
	{
		//higher tolerance has less verts
		SphereGenerator sphereRawData{0.02f};

		setPositions(sphereRawData.getPositions());
		setNormals(sphereRawData.getNormals());
		setElementIndices(sphereRawData.getTriangleElementIndices());
	}
}
