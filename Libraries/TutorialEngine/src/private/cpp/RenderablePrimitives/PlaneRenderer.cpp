
#include "GameObjectBase.h"
#include "RenderablePrimitives/PlaneRenderer.h"
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"
#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"
#include "Utils/Shader.h"

namespace TutorialEngine
{
	PlaneRenderer::PlaneRenderer()
	{

	}

	void PlaneRenderer::onAcquireGPUResources()
	{
		using namespace glm;

		const char* const plane_vert_src = R"(
			//#version 330 core
			//layout (location = 0) in vec3 inPositions;
			//out vec4 color;

			attribute vec3 inPositions;
			varying vec4 color;

			uniform vec4 uColor;// = vec4(1,1,1,1);
			uniform mat4 model;			
			uniform mat4 projection_view;

			void main()
			{
				gl_Position = projection_view * model * vec4(inPositions, 1);
				color = uColor;
			}
		)";
		const char* const plane_frag_src = R"(
			varying highp vec4 color;
			uniform bool bScreenDoorEffect; // = false;

			void main()
			{
				gl_FragColor = color;

				if(bScreenDoorEffect)
				{
					//GLES2 port failures below, leaving for posterity as to what this was before it was before converting to GLES2 as my solution may not work
					//ivec4 clamped = ivec4(gl_FragCoord);
					//if( clamped.x % 2 != 0 && clamped.y % 2 != 0 ) //ERROR: 0:13: '%' : integer modulus operator supported in GLSL ES 3.00 and above only
					//if( clamped.x & 1 != 0 && clamped.y & 1 != 0 )	//ERROR: 0:14: '&' : bit-wise operator supported in GLSL ES 3.00 and above only

					mediump float xMod = mod(gl_FragCoord.x, 2.0);
					mediump float yMod = mod(gl_FragCoord.y, 2.0);
					if( abs(xMod) < 0.01 && abs(yMod) < 0.01)  //not sure if this comparison will work, porting lot of code cannot test
					{
						discard;
					} 
				}
			}
		)";

		Engine::Shader::ShaderParams in;
		in.vertex_src = plane_vert_src;
		in.fragment_src = plane_frag_src;

		shader = new_sp<Engine::Shader>(in);
		shader->setUniform4f("uColor", glm::vec4(1,1,1,1));
		shader->setInt("bScreenDoorEffect",0);
		

		//glBindVertexArray(0);
		//glGenVertexArrays(1, &vao);
		//glBindVertexArray(vao);

		//plane in x-plane so normal matches x-axis -- this lets us easily create rotation matrix that moves plane to match requested normal
		std::vector<vec3> vertPositions =
		{
			vec3(0, -0.5f, -0.5f),
			vec3(0, 0.5f, -0.5f),
			vec3(0, -0.5f, 0.5f),

			vec3(0, 0.5f, -0.5f),
			vec3(0, 0.5f, 0.5f),
			vec3(0, -0.5f, 0.5f)
		};

		attribLoc_vboPositions = glGetAttribLocation(shader->getProgramId(), "inPositions");

		ec(glGenBuffers(1, &vboPositions));
		bindVBO();
		ec(glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertPositions.size(), vertPositions.data(), GL_STATIC_DRAW));
		configureVBO();

		//prevent other calls from corrupting this VAO state
		//glBindVertexArray(0); //#vao
	}

	void PlaneRenderer::bindVBO()
	{
		ec(glBindBuffer(GL_ARRAY_BUFFER, vboPositions));
	}

	void PlaneRenderer::configureVBO()
	{
		ec(glVertexAttribPointer(attribLoc_vboPositions, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast<void*>(0)));
		ec(glEnableVertexAttribArray(attribLoc_vboPositions));
	}


	void PlaneRenderer::onReleaseGPUResources()
	{
		if (glIsBuffer(vboPositions))
		{
			ec(glDeleteBuffers(1, &vboPositions));
		}

		shader = nullptr;
	}

	void PlaneRenderer::renderPlane(const glm::vec3& centerPnt, const glm::vec3& normal, const glm::vec3& scale, const glm::vec4& color, const glm::mat4& projection_view)
	{
		using namespace glm;

		vec3 tempUp = vec3(0, 1, 0);
		vec3 u = glm::normalize(normal);
		vec3 tempV = MathUtils::vectorsAreSame(normal, tempUp) ? vec3(-1, 0, 0) : tempUp;
		vec3 w = glm::normalize(glm::cross(u, tempV));
		vec3 v = glm::normalize(glm::cross(w, u));
		mat4 rot{ vec4(u,0), vec4(v,0), vec4(w,0),vec4(0,0,0,1) };

		mat4 model{ 1.f };
		model = glm::translate(model, centerPnt);
		model = model * rot;
		model = glm::scale(model, scale);

		shader->use();
		shader->setUniform4f("uColor", color);
		shader->setMat4("model", model);
		shader->setMat4("projection_view", projection_view);
		shader->setInt("bScreenDoorEffect", bScreenDoorEffect);

		bindVBO();
		configureVBO();

		//glBindVertexArray(vao);
		ec(glDrawArrays(GL_TRIANGLES, 0, 6));
	}
}

