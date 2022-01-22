///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////

#ifdef WITH_SAT_DEMO_BUILD
#include <Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h>

#include "SATRenderDebugUtils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"


//void SAT::drawDebugLine(Shader& shader,
//	const glm::vec3& pntA, const glm::vec3& pntB, const glm::vec3& color,
//	const glm::mat4& model, const glm::mat4& view, const glm::mat4 projection)
//{
//	/* This method is extremely slow and non-performant; use only for debugging purposes */
//	shader.use();
//	shader.setUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
//	shader.setUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
//	shader.setUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
//	shader.setUniform3f("color", color);
//
//	//basically immediate mode, should be very bad performance
//	GLuint tmpVAO, tmpVBO;
//	glGenVertexArrays(1, &tmpVAO);
//	glBindVertexArray(tmpVAO);
//	float verts[] = {
//		pntA.x,  pntA.y, pntA.z, 1.0f,
//		pntB.x, pntB.y, pntB.z, 1.0f
//	};
//	glGenBuffers(1, &tmpVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, tmpVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
//	glEnableVertexAttribArray(0);
//
//	glDrawArrays(GL_LINES, 0, 2);
//
//	glDeleteVertexArrays(1, &tmpVAO);
//	glDeleteBuffers(1, &tmpVBO);
//}
//
//void SAT::drawDebugTriangleFace(Shader& shader, const glm::vec3& pntA, const glm::vec3& pntB, const glm::vec3& pntC,
//	const glm::vec3& color, 
//	const glm::mat4& model, const glm::mat4& view, const glm::mat4 projection)
//{
//	/* This method is extremely slow and non-performant; use only for debugging purposes */
//	shader.use();
//	shader.setUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
//	shader.setUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
//	shader.setUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
//	shader.setUniform3f("color", color);
//
//	//basically immediate mode, should be very bad performance
//	GLuint tmpVAO, tmpVBO;
//	glGenVertexArrays(1, &tmpVAO);
//	glBindVertexArray(tmpVAO);
//	float verts[] = {
//		pntA.x,  pntA.y, pntA.z, 1.0f,
//		pntB.x, pntB.y, pntB.z, 1.0f,
//		pntC.x, pntC.y, pntC.z, 1.0f
//	};
//	glGenBuffers(1, &tmpVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, tmpVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
//	glEnableVertexAttribArray(0);
//
//	glDrawArrays(GL_TRIANGLES, 0, 3);
//
//	glDeleteVertexArrays(1, &tmpVAO);
//	glDeleteBuffers(1, &tmpVBO);
//}
//
//void SAT::drawDebugCollisionShape(Shader& shader, const Shape& shape, const glm::vec3& color, float debugPointSize,
//									bool bRenderEdges, bool bRenderFaces,
//									const glm::mat4& view, const glm::mat4 projection)
//{
//	const std::vector<glm::vec4>& transformedPoints = shape.getTransformedPoints();
//
//	shader.use();
//	shader.setUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
//	shader.setUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
//	shader.setUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
//	shader.setUniform3f("color", color);
//
//	bool pointSizeEnabled = glIsEnabled(GL_POINT_SIZE);
//	float originalPointSize = 1.0f;
//	glGetFloatv(GL_POINT_SIZE, &originalPointSize);
//	if (!pointSizeEnabled)
//	{
//		glEnable(GL_POINT_SIZE);
//		glPointSize(debugPointSize);
//	}
//
//	//basically immediate mode, should be very bad performance
//	GLuint tmpVAO, tmpVBO;
//	glGenVertexArrays(1, &tmpVAO);
//	glBindVertexArray(tmpVAO);
//	glGenBuffers(1, &tmpVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, tmpVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transformedPoints.size(), &transformedPoints[0], GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
//	glEnableVertexAttribArray(0);
//
//	glDrawArrays(GL_POINTS, 0, transformedPoints.size());
//
//	glDeleteVertexArrays(1, &tmpVAO);
//	glDeleteBuffers(1, &tmpVBO);
//
//	if (!pointSizeEnabled)
//	{
//		glDisable(GL_POINT_SIZE);
//	}
//	else
//	{
//		glPointSize(originalPointSize);
//	}
//
//	if (bRenderEdges)
//	{
//		for (const Shape::EdgePointIndices& edge : shape.getDebugEdgeIdxs())
//		{
//			drawDebugLine(shader, transformedPoints[edge.indexA], transformedPoints[edge.indexB], color,
//				glm::mat4(1.0f), view, projection);
//		}
//	}
//	if (bRenderFaces)
//	{
//		for (const Shape::FacePointIndices& face : shape.getDebugFaceIdxs())
//		{
//			uint32_t aIdx = face.edge1.indexA;
//			uint32_t bIdx = face.edge1.indexB;
//			uint32_t cIdx = face.edge2.indexA;
//			cIdx = (cIdx == aIdx || cIdx == bIdx) ? face.edge2.indexB : cIdx;
//
//			drawDebugTriangleFace(shader, transformedPoints[aIdx], transformedPoints[bIdx], transformedPoints[cIdx], color/2.0f,
//				glm::mat4(1.0f), view, projection);
//		}
//	}
//
//
//}

namespace SAT
{
	ShapeRender::ShapeRender(const Shape& shape)
	{
		using glm::vec3; using glm::vec4; using glm::mat4;

		const std::vector<glm::vec4>& localPoints = shape.getLocalPoints();
		std::vector<glm::vec4> triPnts;
		std::vector<glm::vec3> triNorms;

		//generate triangles for every face
		for (const Shape::FacePointIndices& face : shape.getDebugFaceIdxs())
		{
			//not sure, but this may not respect CCW ordering for some shapes
			uint32_t aIdx = face.edge1.indexA;
			uint32_t bIdx = face.edge1.indexB;
			uint32_t cIdx = face.edge2.indexA;
			cIdx = (cIdx == aIdx || cIdx == bIdx) ? face.edge2.indexB : cIdx;

			triPnts.push_back(localPoints[aIdx]);
			triPnts.push_back(localPoints[bIdx]);
			triPnts.push_back(localPoints[cIdx]);

			//normal generation is not well tested, and if triPnts are not in CCW ordering, the normals will point internally
			glm::vec3 first = glm::vec3(localPoints[bIdx] - localPoints[aIdx]);
			glm::vec3 second = glm::vec3(localPoints[cIdx] - localPoints[aIdx]);
			glm::vec4 normal = glm::vec4(glm::normalize(glm::cross(first, second)), 0.f);
			triNorms.push_back(normal);
			triNorms.push_back(normal);
			triNorms.push_back(normal);
		}
		vertCount = triPnts.size();

		//glGenVertexArrays(1, &VAO_tris);
		//glBindVertexArray(VAO_tris);

		ec(glGenBuffers(1, &VBO_tris));
		ec(glBindBuffer(GL_ARRAY_BUFFER, VBO_tris));
		ec(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * triPnts.size(), &triPnts[0], GL_STATIC_DRAW));
		//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
		//glEnableVertexAttribArray(0);

		ec(glGenBuffers(1, &VBO_normals));
		ec(glBindBuffer(GL_ARRAY_BUFFER, VBO_normals));
		ec(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * triNorms.size(), &triNorms[0], GL_STATIC_DRAW));
		//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(1));
		//glEnableVertexAttribArray(1);
		configureAttributes_Triangles();
		//glBindVertexArray(0);

		//POINTS
		//glGenVertexArrays(1, &VAO_pnts);
		//glBindVertexArray(VAO_pnts);
		ec(glGenBuffers(1, &VBO_pnts));
		ec(glBindBuffer(GL_ARRAY_BUFFER, VBO_pnts));
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * localPoints.size(), &localPoints[0], GL_STATIC_DRAW);
		//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
		//glEnableVertexAttribArray(0);
		configurateAttributes_Points();
		pntCount = localPoints.size();
		//glBindVertexArray(0);
	}

	void ShapeRender::configureAttributes_Triangles()
	{
		ec(glBindBuffer(GL_ARRAY_BUFFER, VBO_tris));
		ec(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0)));
		ec(glEnableVertexAttribArray(0));

		ec(glBindBuffer(GL_ARRAY_BUFFER, VBO_normals));
		ec(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(1)));
		ec(glEnableVertexAttribArray(1));
	}

	void ShapeRender::configurateAttributes_Points()
	{
		ec(glBindBuffer(GL_ARRAY_BUFFER, VBO_pnts));
		ec(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0)));
		ec(glEnableVertexAttribArray(0));
	}

	void ShapeRender::disableAttributes_Triangles()
	{
		ec(glDisableVertexAttribArray(0));
		ec(glDisableVertexAttribArray(1));
	}

	void ShapeRender::disableAttributes_Points()
	{
		ec(glDisableVertexAttribArray(0));
	}

	ShapeRender::~ShapeRender()
	{
		//glDeleteVertexArrays(1, &VAO_tris);
		//glDeleteVertexArrays(1, &VAO_pnts);

		ec(glDeleteBuffers(1, &VBO_tris));
		ec(glDeleteBuffers(1, &VBO_normals));
		ec(glDeleteBuffers(1, &VBO_pnts));
	}

	void ShapeRender::render(bool bRenderPnts /*= true*/, bool bRenderUniqueTris /*= true*/)
	{
		if (bRenderUniqueTris)
		{
			//glBindVertexArray(VAO_tris);
			configureAttributes_Triangles();
			glDrawArrays(GL_TRIANGLES, 0, vertCount);
			//glBindVertexArray(0);
			disableAttributes_Triangles();
		}

		if (bRenderPnts)
		{
			//bool pointSizeEnabled = glIsEnabled(GL_POINT_SIZE);
			//float originalPointSize = 1.0f;
			//glGetFloatv(GL_POINT_SIZE, &originalPointSize);
			//if (!pointSizeEnabled)
			//{
			//	glEnable(GL_POINT_SIZE);
			//	glPointSize(5.f);
			//}
			
			//GLES2 appears to use line width for point size, according to https://gamedev.stackexchange.com/questions/11095/opengl-es-2-0-point-sprites-size
			float originalPointSize = 1.0f;
			ec(glGetFloatv(GL_LINE_WIDTH, &originalPointSize)); 
			ec(glLineWidth(5.f));

			//glBindVertexArray(VAO_pnts);
			configurateAttributes_Points();

			//GLES2 work around to change point size. not sure if this works as there is a lot of compile errors to fix before can test.
			ec(glDrawArrays(GL_POINTS, 0, pntCount));
			ec(glLineWidth(originalPointSize));
			
			//glPointSize(originalPointSize);
			//if (!pointSizeEnabled){ glDisable(GL_POINT_SIZE);}
			//glBindVertexArray(0);
			disableAttributes_Points();
		}
	}

	const float capsuleVertices[] = {
		//positions                 normals

		//bottom cone face with x and z
		0.0f, -1.5f, 0.0f,		0.57735f,-0.57735f,0.57735f,//0.0f, -1.0f, 0.0f,
		1.0f, -0.5f, 0.0f,		0.57735f,-0.57735f,0.57735f,//1.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 1.0f,		0.57735f,-0.57735f,0.57735f,//0.0f, 0.0f, 1.0f,

		//bottom cone face with z and -x
		0.0f, -1.5f, 0.0f,		-0.57735f,-0.57735f,0.57735f,//0.0f, -1.0f, 0.0f,
		0.0f, -0.5f, 1.0f,		-0.57735f,-0.57735f,0.57735f,//0.0f, 0.0f, 1.0f,
		-1.0f, -0.5f, 0.0f,		-0.57735f,-0.57735f,0.57735f,//-1.0f, 0.0f, 0.0f,

		//bottom cone with face -x and -z
		0.0f, -1.5f, 0.0f,		-0.57735f,-0.57735f,-0.57735f,//0.0f, -1.0f, 0.0f,
		-1.0f, -0.5f, 0.0f,		-0.57735f,-0.57735f,-0.57735f,//-1.0f, 0.0f, 0.0f,
		0.0f, -0.5f, -1.0f,		-0.57735f,-0.57735f,-0.57735f,//0.0f, 0.0f, -1.0f,

		//bottom cone with face -z and x
		0.0f, -1.5f, 0.0f,		0.57735f,-0.57735f, -0.57735f,//0.0f, -1.0f, 0.0f,
		0.0f, -0.5f, -1.0f,		0.57735f,-0.57735f, -0.57735f,//0.0f, 0.0f, -1.0f,
		1.0f, -0.5f, 0.0f,		0.57735f,-0.57735f, -0.57735f,//1.0f, 0.0f, 0.0f,





		//side faces in x and z 
		1.0f, -0.5f, 0.0f,		0.707107f, -0.0f, 0.707107f,//1.0f, 0.0f, 0.0f,
		0.0f,  0.5f, 1.0f,		0.707107f, -0.0f, 0.707107f,//0.0f, 0.0f, 1.0f,
		0.0f, -0.5f, 1.0f,		0.707107f, -0.0f, 0.707107f,//0.0f, 0.0f, 1.0f,
		//----				 							   
		1.0f, -0.5f, 0.0f,		0.707107f, -0.0f, 0.707107f,//1.0f, 0.0f, 0.0f,
		1.0f, 0.5f, 0.0f,		0.707107f, -0.0f, 0.707107f,//1.0f, 0.0f, 0.0f,
		0.0f,  0.5f, 1.0f,		0.707107f, -0.0f, 0.707107f,//0.0f, 0.0f, 1.0f,

		//side faces in z and -x
		0.0f, -0.5f, 1.0f,		-0.707107f, 0.0f, 0.707107f,//0.0f, 0.0f, 1.0f,
		0.0f, 0.5f, 1.0f,		-0.707107f, 0.0f, 0.707107f,//0.0f, 0.0f, 1.0f,
		-1.0f, 0.5f, 0.0f,		-0.707107f, 0.0f, 0.707107f,//-1.0f, 0.0f, 0.0f,
		//----										  
		0.0f, -0.5f, 1.0f,		-0.707107f, 0.0f, 0.707107f,//0.0f, 0.0f, 1.0f,
		-1.0f, 0.5f, 0.0f,		-0.707107f, 0.0f, 0.707107f,//-1.0f, 0.0f, 0.0f,
		-1.0f, -0.5f, 0.0f,		-0.707107f, 0.0f, 0.707107f,//-1.0f, 0.0f, 0.0f,

		//side faces in -x and -z
		0.0f, -0.5f, -1.0f,		-0.707107f, -0.0f, -0.707107f,//0.0f, 0.0f, -1.0f,
		-1.0f, -0.5f, 0.0f,		-0.707107f, -0.0f, -0.707107f,//-1.0f, 0.0f, 0.0f,
		-1.0f, 0.5f, 0.0f,		-0.707107f, -0.0f, -0.707107f,//-1.0f, 0.0f, 0.0f,
		//----											   
		0.0f, -0.5f, -1.0f,		-0.707107f, 0.0f, -0.707107f,//0.0f, 0.0f, -1.0f,
		-1.0f, 0.5f, 0.0f,		-0.707107f, 0.0f, -0.707107f,//-1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -1.0f,		-0.707107f, 0.0f, -0.707107f,//0.0f, 0.0f, -1.0f,

		//side faces in -z and x
		0.0f,  -0.5f, -1.0f,	0.707107f, 0.0f, -0.707107f,//0.0f, 0.0f, -1.0f,
		0.0f,  0.5f, -1.0f,		0.707107f, 0.0f, -0.707107f,//0.0f, 0.0f, -1.0f,
		1.0f, 0.5f, 0.0f,		0.707107f, 0.0f, -0.707107f,//1.0f, 0.0f, 0.0f,
		//----										   
		0.0f,  -0.5f, -1.0f,	0.707107f, 0.0f, -0.707107f,//0.0f, 0.0f, -1.0f,
		1.0f, 0.5f, 0.0f,		0.707107f, 0.0f, -0.707107f,//1.0f, 0.0f, 0.0f,
		1.0f, -0.5f, 0.0f,		0.707107f, 0.0f, -0.707107f,//1.0f, 0.0f, 0.0f, 



		//top cone face with z and x
		0.0f, 1.5f, 0.0f,		0.57735f,0.57735f,0.57735f,//0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 1.0f,		0.57735f,0.57735f,0.57735f,//0.0f, 0.0f, 1.0f,
		1.0f, 0.5f, 0.0f,		0.57735f,0.57735f,0.57735f,//1.0f, 0.0f, 0.0f,

		//top cone face with -x and z
		0.0f, 1.5f, 0.0f,		-0.57735f,0.57735f,0.57735f,//0.0f, 1.0f, 0.0f,
		-1.0f, 0.5f, 0.0f,		-0.57735f,0.57735f,0.57735f,//-1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 1.0f,		-0.57735f,0.57735f,0.57735f,//0.0f, 0.0f, 1.0f,

		//top cone with face -z and -x
		0.0f, 1.5f, 0.0f,		-0.57735f,0.57735f,-0.57735f,//0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, -1.0f,		-0.57735f,0.57735f,-0.57735f,//0.0f, 0.0f, -1.0f,
		-1.0f, 0.5f, 0.0f,		-0.57735f,0.57735f,-0.57735f,//-1.0f, 0.0f, 0.0f,

		//top cone with face x and -z 
		0.0f, 1.5f, 0.0f,		0.57735f,0.57735f,-0.57735f,//0.0f, 1.0f, 0.0f,
		1.0f, 0.5f, 0.0f,		0.57735f,0.57735f,-0.57735f,//1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -1.0f,		0.57735f,0.57735f,-0.57735f//0.0f, 0.0f, -1.0f
	};


	CapsuleRenderer::CapsuleRenderer()
	{
		//glGenVertexArrays(1, &capsuleVAO);
		//glBindVertexArray(capsuleVAO);

		ec(glGenBuffers(1, &capsuleVBO));
		ec(glBindBuffer(GL_ARRAY_BUFFER, capsuleVBO));
		ec(glBufferData(GL_ARRAY_BUFFER, sizeof(capsuleVertices), capsuleVertices, GL_STATIC_DRAW));
		configureAttributes();
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);
		//glBindVertexArray(0);
	}

	CapsuleRenderer::~CapsuleRenderer()
	{
		//glDeleteVertexArrays(1, &capsuleVAO);
		ec(glDeleteBuffers(1, &capsuleVBO));
	}

	void CapsuleRenderer::configureAttributes()
	{
		ec(glBindBuffer(GL_ARRAY_BUFFER, capsuleVBO));
		ec(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0)));
		ec(glEnableVertexAttribArray(0));
		ec(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float))));
		ec(glEnableVertexAttribArray(1));
	}

	void CapsuleRenderer::disableAttributes()
	{
		ec(glDisableVertexAttribArray(0));
		ec(glDisableVertexAttribArray(1));
	}

	void CapsuleRenderer::render()
	{
		configureAttributes();//gles2 vao simulation
		//glBindVertexArray(capsuleVAO);
		constexpr GLuint numCapsuleVerts = sizeof(capsuleVertices) / (sizeof(float) * 6);
		ec(glDrawArrays(GL_TRIANGLES, 0, numCapsuleVerts));
		//glBindVertexArray(0);
		disableAttributes();
	}


}


#endif //WITH_SAT_DEMO_BUILD
