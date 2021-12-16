#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"
#include "Utils/Shader.h"
#include "RenderablePrimitives/ImmediateTriangle.h"
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"

namespace TutorialEngine
{
	using namespace glm;


	void ImmediateTriangle::renderTriangle(const glm::vec3& pntA, const glm::vec3& pntB, const glm::vec3& pntC, const glm::vec3& color, const glm::mat4& projection_view)
	{
		using namespace glm;

		if (hasAcquiredResources())
		{
			//use a shear matrix trick to package all this data into a single mat4.
			// shear matrix trick transforms basis vectors into the columns of the provided matrix.
			mat4 shearMatrix = mat4(vec4(pntA, 0), vec4(pntB, 0), vec4(pntC, 0), vec4(0.f, 0.f, 0.f, 1.f));

			shader->use();
			shader->setMat4("shearMat", shearMatrix);
			shader->setMat4("projection_view", projection_view);
			shader->setUniform3f("u_color", color);

			bindVBO();
			configureVBO();
			//glBindVertexArray(vao);
			ec(glDrawArrays(bWireFrame ? GL_LINE_LOOP : GL_TRIANGLES, 0, 3));
		}

	}

	void ImmediateTriangle::onAcquireGPUResources()
	{
		const char* const triShader_vs_src = R"(
				//#version 330 core
				//layout (location = 0) in vec3 basis_vector;
				//out vec4 color;

				attribute vec3 basis_vector;
				varying vec4 color;

				uniform mat4 shearMat;			
				uniform mat4 projection_view;
				uniform vec3 u_color; // = vec3(1,1,1);

				void main()
				{
					gl_Position = projection_view * shearMat * vec4(basis_vector, 1);
					color = vec4(u_color,1.0);
				}
			)";
		const char* const triShader_fs_src = R"(
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
		in.vertex_src = triShader_vs_src;
		in.fragment_src = triShader_fs_src;

		shader = new_sp<Engine::Shader>(in);
		vboPos_attribLoc = glGetAttribLocation(shader->getProgramId(), "basis_vector");

		//glBindVertexArray(0);
		//glGenVertexArrays(1, &vao);
		//glBindVertexArray(vao);

		std::vector<vec3> vertPositions =
		{
			vec3(1,0,0),
			vec3(0,1,0),
			vec3(0,0,1)
		};
		ec(glGenBuffers(1, &vboPositions));
		bindVBO();
		ec(glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertPositions.size(), vertPositions.data(), GL_STATIC_DRAW));
		configureVBO();

		//prevent other calls from corrupting this VAO state
		//glBindVertexArray(0);
	}

	void ImmediateTriangle::onReleaseGPUResources()
	{
		if (glIsBuffer(vboPositions))
		{
			ec(glDeleteBuffers(1, &vboPositions));
		}
		shader = nullptr;
	}

	void ImmediateTriangle::bindVBO()
	{
		ec(glBindBuffer(GL_ARRAY_BUFFER, vboPositions));
	}

	void ImmediateTriangle::configureVBO()
	{
		ec(glVertexAttribPointer(vboPos_attribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), reinterpret_cast<void*>(0)));
		ec(glEnableVertexAttribArray(vboPos_attribLoc));
	}

}
