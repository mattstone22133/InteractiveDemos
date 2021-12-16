#include <vector>

#include "Utils/TriangleListDebugger.h"
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"
#include "Utils/TutorialRayUtils.h"
#include "Transform.h"
#include "Utils/Shader.h"
#include "GameObjectBase.h"

namespace TutorialEngine
{

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	//
	//
	//
	//
	// Triangle List Debugger
	//
	//
	//
	//
	//
	//
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	TriangleListDebugger::TriangleListDebugger(const TriangleList& inList)
	{
		triList = inList;
	}

	void TriangleListDebugger::onAcquireGPUResources()
	{
		Engine::Shader::ShaderParams shaderInit;
		shaderInit.vertex_src = R"(
					//#version 410 core
					//layout (location = 0) in vec3 position;				

					attribute vec3 position;
					uniform mat4 model;// = mat4(1.0);
					uniform mat4 projection_view;// = mat4(1.0);
					void main(){
						gl_Position = projection_view * model * vec4(position,1.0);
					}
				)";
		shaderInit.fragment_src = R"(
					//#version 410 core
					//out vec4 fragmentColor;

					uniform mediump vec3 solidColor;// = vec3(0,1,0);
					void main()
					{
						//fragmentColor = vec4(solidColor.rgb, 1.0);
						gl_FragColor = vec4(solidColor.rgb, 1.0);
					}
				)";

		shader = new_sp<Engine::Shader>(shaderInit);
		shader->setMat4("model", glm::mat4(1.0f));
		shader->setUniform3f("solidColor", glm::vec3(0.f, 1.f, 0.f));
		attribLoc_vboPositions = glGetAttribLocation(shader->getProgramId(), "position");

		//ec(glGenVertexArrays(1, &vao));
		//ec(glBindVertexArray(vao)); //gles2 no vao

		//convert the triangle list into a series of vertices
		std::vector<glm::vec3> verts;
		for (Triangle tri : triList.getLocalTriangles())
		{
			verts.push_back(tri.pntA);
			verts.push_back(tri.pntB);
			verts.push_back(tri.pntC);
		}
		numVerts = int(verts.size()); //3 elements in a vec3

		ec(glGenBuffers(1, &vbo));
		bindVBO();
		ec(glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), &verts[0], GL_STATIC_DRAW));
		configureVBOAttributes();

		//ec(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0)));
		//glEnableVertexAttribArray(0);//gles2 no vao
	}

	void TriangleListDebugger::onReleaseGPUResources()
	{
		//ec(glDeleteVertexArrays(1, &vao));
		ec(glDeleteBuffers(1, &vbo));
		shader = nullptr;
	}

	void TriangleListDebugger::render(const glm::mat4& projection_view, const glm::mat4& model)
	{
		/** PSA:: if you get nullptr here, make sure you called Super::init.*/

		shader->use();
		shader->setMat4("projection_view", projection_view);
		shader->setMat4("model", model);

		//glBindVertexArray(vao); 
		bindVBO();
		configureVBOAttributes();
		//ec(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
		//ec(glDrawArrays(GL_TRIANGLES, 0, numVerts));
		//ec(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		ec(glDrawArrays(GL_LINE_LOOP, 0, numVerts));

		//glBindVertexArray(0);
	}

	void TriangleListDebugger::bindVBO()
	{
		ec(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	}

	void TriangleListDebugger::configureVBOAttributes()
	{
		ec(glVertexAttribPointer(attribLoc_vboPositions, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0)));
		ec(glEnableVertexAttribArray(attribLoc_vboPositions));
	}

	TriangleListDebugger::~TriangleListDebugger()
	{

	}

}
