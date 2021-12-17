
#include "Geometry/TriangleMesh_GpuBuffer.h"
#include "Utils/Platform//OpenGLES2/PlatformOpenGLESInclude.h"
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"
#include "Utils/Shader.h"

namespace Engine
{
	TriangleMesh_GpuBuffer::TriangleMesh_GpuBuffer()
	{
		//do not acquire gpu buffers on construction, subclasses expected to populate positions (and potentially normals) and then subclasses will manually acquire GPU buffers when appropriate.

	}

	TriangleMesh_GpuBuffer::~TriangleMesh_GpuBuffer()
	{

	}

	void TriangleMesh_GpuBuffer::setPositions(const std::vector<glm::vec3>& inPositions)
	{
		const bool bGpuHasOldData = hasAcquiredResources();
		if (bGpuHasOldData)
		{
			//ideally this doesn't happen, but handle it gracefully; albeit expensively
			onReleaseGPUResources();
		}

		positions = inPositions;

		if (bGpuHasOldData)
		{
			// since gpu already had resoures, context must be available already; so go ahead and reallocate
			onAcquireGPUResources();
		}
	}

	void TriangleMesh_GpuBuffer::setNormals(const std::vector<glm::vec3>& inNormals)
	{
		const bool bGpuHasOldData = hasAcquiredResources();
		if (bGpuHasOldData)
		{
			//ideally this doesn't happen, but handle it gracefully; albeit expensively
			onReleaseGPUResources();
		}

		normals = inNormals;

		if (bGpuHasOldData)
		{
			// since gpu already had resoures, context must be available already; so go ahead and reallocate
			onAcquireGPUResources();
		}
	}

	void TriangleMesh_GpuBuffer::setElementIndices(const std::vector<uint32_t>& inElementIndices)
	{
		const bool bGpuHasOldData = hasAcquiredResources();
		if (bGpuHasOldData)
		{
			//ideally this doesn't happen, but handle it gracefully; albeit expensively
			onReleaseGPUResources();
		}

		elementIndices = inElementIndices;

		if (bGpuHasOldData)
		{
			// since gpu already had resoures, context must be available already; so go ahead and reallocate
			onAcquireGPUResources();
		}
	}

	void TriangleMesh_GpuBuffer::cacheAttribLocations(class Shader& shader)
	{
		if (positions.size() > 0)
		{
			attributeLoc_vboPositions = glGetAttribLocation(shader.getProgramId(), attributeShaderName_Positions);
		}

		if (normals.size() > 0)
		{
			attributeLoc_vboNormals = glGetAttribLocation(shader.getProgramId(), attributeShaderName_Normals);
		}
	}

	void TriangleMesh_GpuBuffer::render(class Shader& shader, const bool callUseShader)
	{
		if (callUseShader)
		{
			shader.use();
		}

		cacheAttribLocations(shader);
		renderCached();
	}

	void TriangleMesh_GpuBuffer::onAcquireGPUResources()
	{
		using namespace glm;

		if (normals.size() > 0 && normals.size() != positions.size())
		{
			std::cerr << __FUNCTION__ << " normals provided to mesh, but did not match number of positions" << std::endl;
			return;
		}

		if (positions.size() > 0)
		{
			//plane in x-plane so normal matches x-axis -- this lets us easily create rotation matrix that moves plane to match requested normal
			ec(glGenBuffers(1, &vboPositions));
			ec(glBindBuffer(GL_ARRAY_BUFFER, vboPositions));
			ec(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW));
			ec(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), reinterpret_cast<void*>(0)));
			ec(glEnableVertexAttribArray(0));
		}

		if (normals.size() > 0)
		{
			ec(glGenBuffers(1, &vboNormals));
			ec(glBindBuffer(GL_ARRAY_BUFFER, vboNormals));
			ec(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW));
			ec(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), reinterpret_cast<void*>(0)));
			ec(glEnableVertexAttribArray(1));
		}

		if (elementIndices.size() > 0)
		{
			ec(glGenBuffers(1, &ebo));
			ec(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
			ec(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * elementIndices.size(), elementIndices.data(), GL_STATIC_DRAW));
		}
	}

	void TriangleMesh_GpuBuffer::onReleaseGPUResources()
	{
		if (hasAcquiredResources() 
			&& isGPUContextAvailable())
		{
			if (glIsBuffer(vboPositions))
			{
				ec(glDeleteBuffers(1, &vboPositions));
			}
			if (glIsBuffer(vboNormals))
			{
				ec(glDeleteBuffers(1, &vboNormals));
			}
			if (glIsBuffer(ebo))
			{
				glDeleteBuffers(1, &ebo);
			}
		}
	}

	void TriangleMesh_GpuBuffer::renderCached()
	{
		using namespace glm;


		if (positions.size() > 0)
		{
			ec(glBindBuffer(GL_ARRAY_BUFFER, vboPositions));
			ec(glVertexAttribPointer(attributeLoc_vboPositions, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), reinterpret_cast<void*>(0)));
			ec(glEnableVertexAttribArray(attributeLoc_vboPositions));
		}

		if (normals.size() > 0)
		{
			ec(glBindBuffer(GL_ARRAY_BUFFER, vboNormals));
			ec(glVertexAttribPointer(attributeLoc_vboNormals, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), reinterpret_cast<void*>(0)));
			ec(glEnableVertexAttribArray(attributeLoc_vboNormals));
		}

		uint32_t TriangleRenderMode = bRenderWireframe ? GL_LINE_LOOP : GL_TRIANGLES;
		if (elementIndices.size() > 0)
		{
			// todo -- debug overfow check for very large models converting from size_t to GLsizei

			//draw with elements if we have element indices.
			ec(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
			ec(glDrawElements(TriangleRenderMode, GLsizei(elementIndices.size()), GL_UNSIGNED_INT, reinterpret_cast<void*>(0)));
		}
		else
		{
			// todo -- debug overfow check for very large models converting from size_t to GLsizei

			//draw triangles if not using elements
			ec(glDrawArrays(TriangleRenderMode, 0, GLsizei(positions.size())));
		}

	}

}