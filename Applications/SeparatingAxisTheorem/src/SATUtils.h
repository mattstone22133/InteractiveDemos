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
					//#version 330 core
					//layout (location = 0) in vec3 position;			
					//layout (location = 1) in vec3 normal;	
				attribute vec3 position;			
				attribute vec3 normal;	
				
				uniform highp mat4 model;
				uniform highp mat4 view;
				uniform highp mat4 projection;
				uniform highp mat4 transposeInverseModel;

				varying vec3 fragNormal;
				varying vec3 fragPosition;

				void main(){
					gl_Position = projection * view * model * vec4(position, 1);
					fragPosition = vec3(model * vec4(position, 1));

					//calculate the inverse_tranpose matrix on CPU in real applications; it's a very costly operation
					fragNormal = normalize(mat3(transposeInverseModel) * normal);
				}
			)";
	const char* const litObjectShaderFragSrc = R"(
				//#version 330 core
				//out vec4 fragmentColor;

				uniform highp vec3 lightColor;
				uniform highp vec3 objectColor;
				uniform highp vec3 lightPosition;
				uniform highp vec3 cameraPosition;

				varying highp vec3 fragNormal;
				varying highp vec3 fragPosition;

				/* uniforms can have initial value in GLSL 1.20 and up */
				//uniform highp float ambientStrength = 0.1; 
				const highp float ambientStrength = 0.1; 
				uniform highp vec3 objColor;// = vec3(1,1,1);

				void main(){
					highp vec3 ambientLight = ambientStrength * lightColor;					

					highp vec3 toLight = normalize(lightPosition - fragPosition);
					highp vec3 normal = normalize(fragNormal);									//interpolation of different normalize will cause loss of unit length
					highp vec3 diffuseLight = max(dot(toLight, fragNormal), 0.0) * lightColor;

					highp float specularStrength = 0.5;
					highp vec3 toView = normalize(cameraPosition - fragPosition);
					highp vec3 toReflection = reflect(-toView, normal);							//reflect expects vector from light position
					highp float specularAmount = pow(max(dot(toReflection, toLight), 0.0), 32.0);
					highp vec3 specularLight = specularStrength * lightColor * specularAmount;

					highp vec3 lightContribution = (ambientLight + diffuseLight + specularLight) * objectColor;
					
					gl_FragColor = vec4(lightContribution, 1.0);
				}
			)";
	const char* const lightLocationShaderVertSrc = R"(
				//#version 330 core
				//layout (location = 0) in vec3 position;				
				attribute vec3 position;				
				
				uniform highp mat4 model;
				uniform highp mat4 view;
				uniform highp mat4 projection;

				void main(){
					gl_Position = projection * view * model * vec4(position, 1);
				}
			)";
	const char* const lightLocationShaderFragSrc = R"(
				//#version 330 core
				//out vec4 fragmentColor;

				uniform highp vec3 lightColor;
				
				void main(){
					gl_FragColor = vec4(lightColor, 1.0);
				}
			)";

	const char* const Dim3DebugShaderVertSrc = R"(
				//#version 330 core
				//layout (location = 0) in vec4 position;				
				attribute vec4 position;
				
				uniform highp mat4 model;
				uniform highp mat4 view;
				uniform highp mat4 projection;

				void main(){
					gl_Position = projection * view * model * position;
				}
			)";
	const char* const Dim3DebugShaderFragSrc = R"(
				//#version 330 core
				//out vec4 fragmentColor;
				uniform highp vec3 color; // = vec3(1.f,1.f,1.f);
				void main(){
					gl_FragColor = vec4(color, 1.0);
				}
			)";
	void printVec3(glm::vec3 v);

	void drawDebugLine(
		const glm::vec3& pntA, const glm::vec3& pntB, const glm::vec3& color,
		const glm::mat4& /*model*/, const glm::mat4& view, const glm::mat4 projection
	);

}
#endif #WITH_SAT_DEMO_BUILD
