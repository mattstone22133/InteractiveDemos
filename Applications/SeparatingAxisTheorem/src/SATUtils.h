#pragma once

///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////
#ifdef WITH_SAT_DEMO_BUILD
#include <Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h>
#include <iostream>

#include <PortedOldOpenGL3/Deprecated_Shader.h>
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"
#include "RenderablePrimitives/LineRenderer.h"
#include "GameObjectBase.h"

namespace SAT
{
	//There was a LOT of duplication happening in my original demo. Here I try to remove some of that to make porting easier.


	const char* const litObjectShaderVertSrc = R"(
				#version 330 core
				layout (location = 0) in vec3 position;			
				layout (location = 1) in vec3 normal;	
				
				uniform mat4 model;
				uniform mat4 view;
				uniform mat4 projection;

				out vec3 fragNormal;
				out vec3 fragPosition;

				void main(){
					gl_Position = projection * view * model * vec4(position, 1);
					fragPosition = vec3(model * vec4(position, 1));

					//calculate the inverse_tranpose matrix on CPU in real applications; it's a very costly operation
					fragNormal = normalize(mat3(transpose(inverse(model))) * normal);
				}
			)";
	const char* const litObjectShaderFragSrc = R"(
				#version 330 core
				out vec4 fragmentColor;

				uniform vec3 lightColor;
				uniform vec3 objectColor;
				uniform vec3 lightPosition;
				uniform vec3 cameraPosition;

				in vec3 fragNormal;
				in vec3 fragPosition;

				/* uniforms can have initial value in GLSL 1.20 and up */
				uniform float ambientStrength = 0.1f; 
				uniform vec3 objColor = vec3(1,1,1);

				void main(){
					vec3 ambientLight = ambientStrength * lightColor;					

					vec3 toLight = normalize(lightPosition - fragPosition);
					vec3 normal = normalize(fragNormal);									//interpolation of different normalize will cause loss of unit length
					vec3 diffuseLight = max(dot(toLight, fragNormal), 0) * lightColor;

					float specularStrength = 0.5f;
					vec3 toView = normalize(cameraPosition - fragPosition);
					vec3 toReflection = reflect(-toView, normal);							//reflect expects vector from light position
					float specularAmount = pow(max(dot(toReflection, toLight), 0), 32);
					vec3 specularLight = specularStrength * lightColor * specularAmount;

					vec3 lightContribution = (ambientLight + diffuseLight + specularLight) * objectColor;
					
					fragmentColor = vec4(lightContribution, 1.0f);
				}
			)";
	const char* const lightLocationShaderVertSrc = R"(
				#version 330 core
				layout (location = 0) in vec3 position;				
				
				uniform mat4 model;
				uniform mat4 view;
				uniform mat4 projection;

				void main(){
					gl_Position = projection * view * model * vec4(position, 1);
				}
			)";
	const char* const lightLocationShaderFragSrc = R"(
				#version 330 core
				out vec4 fragmentColor;

				uniform vec3 lightColor;
				
				void main(){
					fragmentColor = vec4(lightColor, 1.f);
				}
			)";

	const char* const Dim3DebugShaderVertSrc = R"(
				#version 330 core
				layout (location = 0) in vec4 position;				
				
				uniform mat4 model;
				uniform mat4 view;
				uniform mat4 projection;

				void main(){
					gl_Position = projection * view * model * position;
				}
			)";
	const char* const Dim3DebugShaderFragSrc = R"(
				#version 330 core
				out vec4 fragmentColor;
				uniform vec3 color = vec3(1.f,1.f,1.f);
				void main(){
					fragmentColor = vec4(color, 1.f);
				}
			)";
	void printVec3(glm::vec3 v)
	{
		std::cout << "[" << v.x << ", " << v.y << ", " << v.z << "]";
	}

	void drawDebugLine(
		const glm::vec3& pntA, const glm::vec3& pntB, const glm::vec3& color,
		const glm::mat4& /*model*/, const glm::mat4& view, const glm::mat4 projection
	)
	{
		/* This method is extremely slow and non-performant; use only for debugging purposes */
		//Engine::Shader::ShaderParams shaderInit;
		//shaderInit.vertex_src = Dim3DebugShaderVertSrc;
		//shaderInit.fragment_src = Dim3DebugShaderFragSrc;
		//static sp<Engine::Shader> shader = new_sp<Engine::Shader>(shaderInit);

		//if (shader && shader->hasAcquiredResources())
		//{
		//	shader->use();
		//	shader->setUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		//	shader->setUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		//	shader->setUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
		//	shader->setUniform3f("color", color);

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

		//	glDrawArrays(GL_LINES, 0, 2);

		//	glDeleteVertexArrays(1, &tmpVAO); //this is so gross, why did I do this? I should fix it...
		//	glDeleteBuffers(1, &tmpVBO);
		//}


		//--------------------------------------------------------------------------------------------------

		/* This method is extremely slow and non-performant; use only for debugging purposes */
		//static Deprecated_Shader shader(Dim3DebugShaderVertSrc, Dim3DebugShaderFragSrc, false);

		//shader.use();
		//shader.setUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		//shader.setUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		//shader.setUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
		//shader.setUniform3f("color", color);

		////basically immediate mode, should be very bad performance
		////GLuint tmpVAO;
		//GLuint tmpVBO;
		////glGenVertexArrays(1, &tmpVAO);
		////glBindVertexArray(tmpVAO);
		//float verts[] = {
		//	pntA.x,  pntA.y, pntA.z, 1.0f,
		//	pntB.x, pntB.y, pntB.z, 1.0f
		//};
		//ec(glGenBuffers(1, &tmpVBO));
		//ec(glBindBuffer(GL_ARRAY_BUFFER, tmpVBO));
		//ec(glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW));
		//ec(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0)));
		//ec(glEnableVertexAttribArray(0));

		//ec(glDrawArrays(GL_LINES, 0, 2));

		//ec(glDisableVertexAttribArray(0)); //this is so BAD... but porting code as close to possible as the original though..
		////glDeleteVertexArrays(1, &tmpVAO);
		//ec(glDeleteBuffers(1, &tmpVBO));

		//----------------------------------------------------------------------------------
		
		//just use tutorial engines line renderer
		static sp<TutorialEngine::LineRenderer> LineRenderer = new_sp<TutorialEngine::LineRenderer>();
		if (LineRenderer->hasAcquiredResources())
		{
			LineRenderer->renderLine(pntA, pntB, color, projection * view);
		}
	}

}
#endif #WITH_SAT_DEMO_BUILD
