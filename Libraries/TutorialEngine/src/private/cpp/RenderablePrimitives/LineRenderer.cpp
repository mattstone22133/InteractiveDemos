
#include "RenderablePrimitives/LineRenderer.h"
#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"
#include "Utils/Shader.h"
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"



namespace TutorialEngine
{
	LineRenderer::LineRenderer()
	{

	}

	void LineRenderer::onAcquireGPUResources()
	{
		using namespace glm;

		const char* const line_vert_src = R"(
//#version 330 core
//layout (location = 0) in vec3 basis_vector; gl3.3
//out vec4 color;

attribute vec3 basis_vector;
varying mediump vec4 color; 
uniform highp mat4 shearMat;			
uniform highp mat4 projection_view;

void main()
{
	gl_Position = projection_view * shearMat * vec4(basis_vector, 1);
	color = shearMat[2]; //3rd col is packed color
}
		)";
		const char* const line_frag_src = R"(
//#version 330 core
//in vec4 color;
//out vec4 fragColor;

varying mediump vec4 color; 

void main()
{
	gl_FragColor = color;
}
		)";

		Engine::Shader::ShaderParams in;
		in.vertex_src = line_vert_src;
		in.fragment_src = line_frag_src;

		shader = new_sp<Engine::Shader>(in);
		//must get this before attempting to configure buffer, for it to work correctly.
		vboPos_AttribLoc = glGetAttribLocation(shader->getProgramId(), "basis_vector");

		//glBindVertexArray(0);
		//glGenVertexArrays(1, &vao);
		//glBindVertexArray(vao);

		std::vector<vec3> vertPositions =
		{
			vec3(1,0,0),
			vec3(0,1,0)
		};
		ec(glGenBuffers(1, &vboPositions));
		ec(glBindBuffer(GL_ARRAY_BUFFER, vboPositions));
		ec(glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertPositions.size(), vertPositions.data(), GL_STATIC_DRAW));
		configureVBOs();


		//prevent other calls from corrupting this VAO state
		//glBindVertexArray(0); //#vao

		//todo shader needs to be converted to gpu resource so it can properly cleanup, for now make a new one
		
	}

	void LineRenderer::onReleaseGPUResources()
	{
		//todo shader needs to be converted to gpu resource so it can properly cleanup, for now -- delete it.
		shader = nullptr;

		if (glIsBuffer(vboPositions))
		{
			ec(glDeleteBuffers(1, &vboPositions));
		}
	}

	void LineRenderer::bindVBOs()
	{
		ec(glBindBuffer(GL_ARRAY_BUFFER, vboPositions));
	}

	void LineRenderer::configureVBOs()
	{
		ec(glVertexAttribPointer(vboPos_AttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast<void*>(0)));
		ec(glEnableVertexAttribArray(vboPos_AttribLoc));
	}

	void LineRenderer::renderLine(const glm::vec3& pntA, const glm::vec3& pntB, const glm::vec3& color, const glm::mat4& projection_view)
	{
		using namespace glm;
		if (hasAcquiredResources())
		{
			//use a shear matrix trick to package all this data into a single mat4.
			// shear matrix trick transforms basis vectors into the columns of the provided matrix.
			mat4 shearMatrix = mat4(vec4(pntA, 0), vec4(pntB, 0), vec4(color, 1), vec4(0.f, 0.f, 0.f, 1.f));

			shader->use();
			shader->setMat4("shearMat", shearMatrix);
			shader->setMat4("projection_view", projection_view);

			//glBindVertexArray(vao); //#vao
			bindVBOs();
			configureVBOs();
			ec(glDrawArrays(GL_LINES, 0, 2));
		}
	}
}

