///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////

#ifdef WITH_SAT_DEMO_BUILD

#include<iostream>

#include <Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h>
//#include <glad/glad.h> //include opengl headers, so should be before anything that uses those headers (such as GLFW)
//#include <GLFW/glfw3.h>

#include <string>
#include <cmath>
#include <glm/gtx/quaternion.hpp>
#include <tuple>
#include <array>

#include "SATRenderDebugUtils.h"
#include "SATDemoInterface.h"

#include "Utils/Shader.h"
#include "Framework/Cameras/QuaternionCamera.h"
//#include "../../GettingStarted/Camera/CameraFPS.h"
//#include "../../../InputTracker.h"
//#include "../../nu_utils.h"
//#include "../../../Shader.h"

#include "SATUtils.h"

#include "SATComponent.h"
#include "SATUnitTestUtils.h"

#include "PortedOldOpenGL3/Deprecated_InputTracker.h"
#include "PortedOldOpenGL3/Deprecated_CameraFPS.h"
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"

#if WITH_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif //WITH_IMGUI

using namespace SAT;
namespace
{
	using SAT::ColumnBasedTransform;

	//const char* litObjectShaderVertSrc = R"(
	//			#version 330 core
	//			layout (location = 0) in vec3 position;			
	//			layout (location = 1) in vec3 normal;	
	//			
	//			uniform mat4 model;
	//			uniform mat4 view;
	//			uniform mat4 projection;

	//			out vec3 fragNormal;
	//			out vec3 fragPosition;

	//			void main(){
	//				gl_Position = projection * view * model * vec4(position, 1);
	//				fragPosition = vec3(model * vec4(position, 1));

	//				//calculate the inverse_tranpose matrix on CPU in real applications; it's a very costly operation
	//				fragNormal = normalize(mat3(transpose(inverse(model))) * normal);
	//			}
	//		)";
	//const char* litObjectShaderFragSrc = R"(
	//			#version 330 core
	//			out vec4 fragmentColor;

	//			uniform vec3 lightColor;
	//			uniform vec3 objectColor;
	//			uniform vec3 lightPosition;
	//			uniform vec3 cameraPosition;

	//			in vec3 fragNormal;
	//			in vec3 fragPosition;

	//			/* uniforms can have initial value in GLSL 1.20 and up */
	//			uniform float ambientStrength = 0.1f; 
	//			uniform vec3 objColor = vec3(1,1,1);

	//			void main(){
	//				vec3 ambientLight = ambientStrength * lightColor;					

	//				vec3 toLight = normalize(lightPosition - fragPosition);
	//				vec3 normal = normalize(fragNormal);									//interpolation of different normalize will cause loss of unit length
	//				vec3 diffuseLight = max(dot(toLight, fragNormal), 0) * lightColor;

	//				float specularStrength = 0.5f;
	//				vec3 toView = normalize(cameraPosition - fragPosition);
	//				vec3 toReflection = reflect(-toView, normal);							//reflect expects vector from light position
	//				float specularAmount = pow(max(dot(toReflection, toLight), 0), 32);
	//				vec3 specularLight = specularStrength * lightColor * specularAmount;

	//				vec3 lightContribution = (ambientLight + diffuseLight + specularLight) * objectColor;
	//				
	//				fragmentColor = vec4(lightContribution, 1.0f);
	//			}
	//		)";
	//const char* lightLocationShaderVertSrc = R"(
	//			#version 330 core
	//			layout (location = 0) in vec3 position;				
	//			
	//			uniform mat4 model;
	//			uniform mat4 view;
	//			uniform mat4 projection;

	//			void main(){
	//				gl_Position = projection * view * model * vec4(position, 1);
	//			}
	//		)";
	//const char* lightLocationShaderFragSrc = R"(
	//			#version 330 core
	//			out vec4 fragmentColor;

	//			uniform vec3 lightColor;
	//			
	//			void main(){
	//				fragmentColor = vec4(lightColor, 1.f);
	//			}
	//		)";

	//const char* Dim3DebugShaderVertSrc = R"(
	//			#version 330 core
	//			layout (location = 0) in vec4 position;				
	//			
	//			uniform mat4 model;
	//			uniform mat4 view;
	//			uniform mat4 projection;

	//			void main(){
	//				gl_Position = projection * view * model * position;
	//			}
	//		)";
	//const char* Dim3DebugShaderFragSrc = R"(
	//			#version 330 core
	//			out vec4 fragmentColor;
	//			uniform vec3 color = vec3(1.f,1.f,1.f);
	//			void main(){
	//				fragmentColor = vec4(color, 1.f);
	//			}
	//		)";
	//void printVec3(glm::vec3 v)
	//{
	//	std::cout << "[" << v.x << ", " << v.y << ", " << v.z << "]";
	//}

	//void drawDebugLine(
	//	const glm::vec3& pntA, const glm::vec3& pntB, const glm::vec3& color,
	//	const glm::mat4& model, const glm::mat4& view, const glm::mat4 projection
	//)
	//{
	//	/* This method is extremely slow and non-performant; use only for debugging purposes */
	//	static Shader shader(Dim3DebugShaderVertSrc, Dim3DebugShaderFragSrc, false);

	//	shader.use();
	//	shader.setUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
	//	shader.setUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
	//	shader.setUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
	//	shader.setUniform3f("color", color);

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

	//	glDeleteVertexArrays(1, &tmpVAO);
	//	glDeleteBuffers(1, &tmpVBO);
	//}

	//class ISATDemo
	//{
	//public:
	//	ISATDemo(int width, int height) {}
	//	virtual void tickGameLoop(GLFWwindow* window) = 0;
	//	virtual void handleModuleFocused(GLFWwindow* window) = 0;
	//};

	class CapsuleDemo final : public ISATDemo
	{
		//Cached Window Data
		int height;
		int width;
		float lastFrameTime = 0.f;
		float deltaTime = 0.0f;

		//OpenGL data
		GLuint /*cubeVAO,*/ cubeVBO, /*capsuleVAO,*/ capsuleVBO;
		std::size_t capsuleVertSize = 0;

		//Shape Data
		ColumnBasedTransform redCapsuleTransform;
		ColumnBasedTransform blueCapsuleTransform;
		const ColumnBasedTransform defaultBlueCapsuleTransform = { { 0,0,0 }, {1,0,0,0},  { 1,1,1 } };
		const ColumnBasedTransform defaultRedCapsuleTransform = { { 5,0,5 }, {1,0,0,0},  { 1,1,1 } };
		const glm::vec3 blueCapsuleColor{ 0, 0, 1 };
		const glm::vec3 redCapsuleColor{ 1, 0, 0 };
		SAT::PolygonCapsuleShape blueCapsuleCollision;
		SAT::PolygonCapsuleShape redCapsuleCollision;

		//State
		Deprecated_CameraFPS camera{ 45.0f, -90.f, 0.f };
		ColumnBasedTransform lightTransform;
		glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };
		Deprecated_Shader objShader{ litObjectShaderVertSrc , litObjectShaderFragSrc, false };
		Deprecated_Shader lampShader{ lightLocationShaderVertSrc, lightLocationShaderFragSrc, false };
		Deprecated_Shader debugShapeShader{ Dim3DebugShaderVertSrc, Dim3DebugShaderFragSrc, false };

		//Modifiable State
		ColumnBasedTransform* transformTarget = &redCapsuleTransform;
		float moveSpeed = 3;
		float rotationSpeed = 45.0f;
		bool bEnableCollision = true;
		bool bBlockCollisionFailures = true;
		bool bTickUnitTests = false;
		bool bUseCameraAxesForObjectMovement = true;
		bool bEnableAxisOffset = false;
		bool bDrawDebugSATInfo = false;
		glm::vec3 axisOffset{ 0, 0.005f, 0 };
		glm::vec3 cachedVelocity;

		//UnitTests
		std::shared_ptr<SAT::TestSuite> UnitTests;

		void enableAttributes_cubeVBO()
		{
			ec(glBindBuffer(GL_ARRAY_BUFFER, cubeVBO));

			ec(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0)));
			ec(glEnableVertexAttribArray(0));
			ec(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float))));
			ec(glEnableVertexAttribArray(1));
		}

		void disableAttributes_cubeVBO()
		{
			ec(glDisableVertexAttribArray(0));
			ec(glDisableVertexAttribArray(1));
		}

		void enableAttributes_capsuleVBO()
		{
			ec(glBindBuffer(GL_ARRAY_BUFFER, capsuleVBO));

			ec(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0)));
			ec(glEnableVertexAttribArray(0));
			ec(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float))));
			ec(glEnableVertexAttribArray(1));
		}
		void disableAttributes_capsuleVBO()
		{
			ec(glDisableVertexAttribArray(0));
			ec(glDisableVertexAttribArray(1));
		}

		virtual void populateUI() override
		{
#if WITH_IMGUI
			ImGui::Text("Hold RIGHT CLICK on canvas - to turn camera");
			ImGui::Text("W/A/S/D to move camera position");
			ImGui::Text("");
			ImGui::Text("Hold ALT to move objects with WASD-EQ");
			ImGui::Text("Hold ALT + CONTROL to rotate objects with WASD-EQ");
			ImGui::Text("");
			ImGui::Text("Press T to toggle which the object is moved ");
			ImGui::Text("Press V to toggle locking translations(and rotations) to camera (easier movements) ");
			ImGui::Text("Press M to toggle slightly displacing axes so that parallel axes are visible simultaneously ");
			ImGui::Text("Press P to print debug information");
			ImGui::Text("Press R to reset object positions to default.");
			ImGui::Text("Press C to toggle collision detection");
			ImGui::Text("Press 9/0 to decrease/increase scale");
			ImGui::Text("Press U to start unit tests; press left/right to skip through unit tests");
#endif //WITH_IMGUI
		}

	public:
		CapsuleDemo(int width, int height)
			: ISATDemo(width, height)
		{
			using glm::vec2;
			using glm::vec3;
			using glm::mat4;

			camera.setPosition(0.0f, 0.0f, 3.0f);
			this->width = width;
			this->height = height;

			camera.setPosition(-7.29687f, 3.22111f, 5.0681f);
			camera.setYaw(-22.65f);
			camera.setPitch(-15.15f);

			/////////////////////////////////////////////////////////////////////
			// OpenGL/Collision Object Creation
			/////////////////////////////////////////////////////////////////////

			//unit create cube for light
			float cubeVertices[] = {
				//x     y       z      _____normal_xyz___
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
				0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
				-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
				-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

				0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
				0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
				0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
				0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
				0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
				0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
				0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
				0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
				-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
				0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
			};
			//ec(glGenVertexArrays(1, &cubeVAO));
			//ec(glBindVertexArray(cubeVAO));
			ec(glGenBuffers(1, &cubeVBO));
			ec(glBindBuffer(GL_ARRAY_BUFFER, cubeVBO));
			ec(glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW));
			enableAttributes_cubeVBO();
			//ec(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0)));
			//ec(glEnableVertexAttribArray(0));
			//ec(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float))));
			//ec(glEnableVertexAttribArray(1));
			//ec(glBindVertexArray(0));
			disableAttributes_cubeVBO();

			float capsuleVertices[] = {
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
			capsuleVertSize = sizeof(capsuleVertices);
			//ec(glGenVertexArrays(1, &capsuleVAO));
			//ec(glBindVertexArray(capsuleVAO));
			ec(glGenBuffers(1, &capsuleVBO));
			ec(glBindBuffer(GL_ARRAY_BUFFER, capsuleVBO));
			ec(glBufferData(GL_ARRAY_BUFFER, sizeof(capsuleVertices), capsuleVertices, GL_STATIC_DRAW));
			//ec(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0)));
			//ec(glEnableVertexAttribArray(0));
			//ec(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float))));
			//ec(glEnableVertexAttribArray(1));
			//ec(glBindVertexArray(0));
			enableAttributes_capsuleVBO();
			disableAttributes_capsuleVBO();

			redCapsuleTransform = defaultRedCapsuleTransform;
			blueCapsuleTransform = defaultBlueCapsuleTransform;

			lightTransform.position = glm::vec3(5, 3, 0);
			lightTransform.scale = glm::vec3(0.1f);

			/////////////////////////////////////////////////////////////////////
			// Unit Test Creation
			/////////////////////////////////////////////////////////////////////

			UnitTests = std::make_shared<SAT::TestSuite>();
			
			auto UnitTest_ScaledAngledPuncture = std::make_shared< SAT::ApplyVelocityTest>();
				auto KF_DownAngledPuncture = std::make_shared<SAT::ApplyVelocityKeyFrame>(1.0f /*secs*/);
				auto blueAgent_AngledPuncture = std::make_shared<SAT::ApplyVelocityFrameAgent>(
					blueCapsuleCollision,
					vec3(0, 0, 0),
					blueCapsuleTransform,
					blueCapsuleTransform,
					[](SAT::ApplyVelocityFrameAgent&) {return true; }
				);
				auto redAgent_AngledPuncture = std::make_shared< SAT::ApplyVelocityFrameAgent >
					(
						redCapsuleCollision,
						vec3(0, -moveSpeed, 0),
						redCapsuleTransform,
						SAT::ColumnBasedTransform{ {0, 3.0f, 0 }, {glm::angleAxis(glm::radians(33.0f), glm::normalize(glm::vec3(1.0f,1.0f,0.0f)))}, {0.5f,0.5f,0.5f} },
						[](SAT::ApplyVelocityFrameAgent& thisAgent) 
						{
							glm::vec4 origin = thisAgent.getShape().getTransformedOrigin();
							return origin.y >= 0.850825;
						}
				);
				//adding this function to help pinpoint a breakpoint when the obejct punctures
				redAgent_AngledPuncture->setCustomTickFailFunc(
					[](SAT::ApplyVelocityFrameAgent& thisAgent) {
						if (thisAgent.getTransform().position.y < 1.0)
						{
							return false; //don't fail test on tick yet
						}
						return false;
					}
				);
				KF_DownAngledPuncture->AddKeyFrameAgent(blueAgent_AngledPuncture);
				KF_DownAngledPuncture->AddKeyFrameAgent(redAgent_AngledPuncture);
				UnitTest_ScaledAngledPuncture->AddKeyFrame(KF_DownAngledPuncture);
			UnitTests->AddTest(UnitTest_ScaledAngledPuncture);

			auto UnitTest_ScaledBigAngledPuncture = std::make_shared< SAT::ApplyVelocityTest>();
			auto KF_DownBigAngledPuncture = std::make_shared<SAT::ApplyVelocityKeyFrame>(1.0f /*secs*/);
			auto blueAgent_BigAngledPuncture = std::make_shared<SAT::ApplyVelocityFrameAgent>(
				blueCapsuleCollision,
				vec3(0, 0, 0),
				blueCapsuleTransform,
				blueCapsuleTransform,
				[](SAT::ApplyVelocityFrameAgent&) {return true; }
			);
			auto redAgent_BigAngledPuncture = std::make_shared< SAT::ApplyVelocityFrameAgent >
				(
					redCapsuleCollision,
					vec3(0, -moveSpeed, 0),
					redCapsuleTransform,
					SAT::ColumnBasedTransform{ {0, 3.0f, 0 }, {glm::angleAxis(glm::radians(33.0f), glm::normalize(glm::vec3(1.0f,1.0f,0.0f)))}, {1.5f,1.5f,1.5f} },
					[](SAT::ApplyVelocityFrameAgent& thisAgent)
			{
				glm::vec4 origin = thisAgent.getShape().getTransformedOrigin();
				//puncture position: pos:[0, 0.834968, 0]; 
				return origin.y >= 0.850825;
			}
			);
			//adding this function to help pinpoint a breakpoint when the obejct punctures
			redAgent_BigAngledPuncture->setCustomTickFailFunc(
				[](SAT::ApplyVelocityFrameAgent& thisAgent) {
				if (thisAgent.getTransform().position.y < 1.0)
				{
					return false; //don't fail test on tick yet
				}
				return false;
			}
			);
			KF_DownBigAngledPuncture->AddKeyFrameAgent(blueAgent_BigAngledPuncture);
			KF_DownBigAngledPuncture->AddKeyFrameAgent(redAgent_BigAngledPuncture);
			UnitTest_ScaledBigAngledPuncture->AddKeyFrame(KF_DownBigAngledPuncture);
			UnitTests->AddTest(UnitTest_ScaledBigAngledPuncture);

			/*               tuple< rotation, scale    >*/
			std::vector<std::tuple<glm::quat, glm::vec3>> transformVariants = {
								/*rotation       scale   */
				std::make_tuple(glm::quat(), vec3(1,1,1)),
				std::make_tuple(glm::quat(), vec3(2,2,2)),
				std::make_tuple(glm::quat(), vec3(0.5f,0.5f,0.5f)),
				std::make_tuple(glm::angleAxis(glm::radians(45.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f))) , vec3(1,1,1) ),
				std::make_tuple(glm::angleAxis(glm::radians(45.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f))), vec3(0.5f,0.5f,0.5f)),
				std::make_tuple(glm::angleAxis(glm::radians(45.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f))), vec3(2,2,2))
			};
			constexpr uint32_t rotIdx = 0, scaleIdx = 1;
				
			// Unit Test testing 4 cardinal directions
			for (const auto& tpl : transformVariants)
			{
				auto UnitTest_CardinalDirections = std::make_shared< SAT::ApplyVelocityTest>();
				auto KF_CardinalDown = std::make_shared<SAT::ApplyVelocityKeyFrame>(1.0f /*secs*/);
				auto KF_CardinalUp = std::make_shared<SAT::ApplyVelocityKeyFrame>(1.0f /*secs*/);
				auto KF_CardinalLeft = std::make_shared<SAT::ApplyVelocityKeyFrame>(1.0f /*secs*/);
				auto KF_CardinalRight = std::make_shared<SAT::ApplyVelocityKeyFrame>(1.0f /*secs*/);
				auto KF_CardinalPosZ = std::make_shared<SAT::ApplyVelocityKeyFrame>(1.0f /*secs*/);
				auto KF_CardinalNegZ = std::make_shared<SAT::ApplyVelocityKeyFrame>(1.0f /*secs*/);
				auto blueAgent_Cardinal = std::make_shared<SAT::ApplyVelocityFrameAgent>(
					blueCapsuleCollision,
					vec3(0, 0, 0),
					blueCapsuleTransform,
					blueCapsuleTransform,
					[](SAT::ApplyVelocityFrameAgent&) {return true; }
				);
				auto redAgent_CardinalDown = std::make_shared< SAT::ApplyVelocityFrameAgent >
					(
						redCapsuleCollision,
						vec3(0, -moveSpeed, 0),
						redCapsuleTransform,
						SAT::ColumnBasedTransform{ {0, 3.0f, 0 }, std::get<rotIdx>(tpl), std::get<scaleIdx>(tpl) },
						[](SAT::ApplyVelocityFrameAgent& thisAgent) {
							glm::vec4 origin = thisAgent.getShape().getTransformedOrigin();
							return origin.y > 0;
						}
				);

				//make copies and just change out appropriate fields
				auto redAgent_CardinalUp = std::make_shared< SAT::ApplyVelocityFrameAgent >(*redAgent_CardinalDown);
				redAgent_CardinalUp->setStartTransform(SAT::ColumnBasedTransform{ {0, -3, 0 }, std::get<rotIdx>(tpl), std::get<scaleIdx>(tpl) });
				redAgent_CardinalUp->setNewVelocity(vec3(0, moveSpeed, 0));
				redAgent_CardinalUp->setCustomCompleteTestFunc([](SAT::ApplyVelocityFrameAgent& thisAgent) {
					glm::vec4 origin = thisAgent.getShape().getTransformedOrigin();
					return origin.y < 0;
				});

				//make copies and just change out appropriate fields
				auto redAgent_CardinalLeft = std::make_shared< SAT::ApplyVelocityFrameAgent >(*redAgent_CardinalDown);
				redAgent_CardinalLeft->setStartTransform(SAT::ColumnBasedTransform{ {3, 0, 0 }, std::get<rotIdx>(tpl), std::get<scaleIdx>(tpl) });
				redAgent_CardinalLeft->setNewVelocity(vec3(-moveSpeed, 0, 0));
				redAgent_CardinalLeft->setCustomCompleteTestFunc([](SAT::ApplyVelocityFrameAgent& thisAgent) {
					glm::vec4 origin = thisAgent.getShape().getTransformedOrigin();
					return origin.x > 0;
				});

				//make copies and just change out appropriate fields
				auto redAgent_CardinalRight = std::make_shared< SAT::ApplyVelocityFrameAgent >(*redAgent_CardinalDown);
				redAgent_CardinalRight->setStartTransform(SAT::ColumnBasedTransform{ {-3, 0, 0 }, std::get<rotIdx>(tpl), std::get<scaleIdx>(tpl) });
				redAgent_CardinalRight->setNewVelocity(vec3(moveSpeed, 0, 0));
				redAgent_CardinalRight->setCustomCompleteTestFunc([](SAT::ApplyVelocityFrameAgent& thisAgent) {
					glm::vec4 origin = thisAgent.getShape().getTransformedOrigin();
					return origin.x < 0;
				});

				auto redAgent_CardinalPosZ = std::make_shared< SAT::ApplyVelocityFrameAgent >(*redAgent_CardinalDown);
				redAgent_CardinalPosZ->setStartTransform(SAT::ColumnBasedTransform{ {0, 0, -3 }, std::get<rotIdx>(tpl), std::get<scaleIdx>(tpl) });
				redAgent_CardinalPosZ->setNewVelocity(vec3(0, 0, moveSpeed));
				redAgent_CardinalPosZ->setCustomCompleteTestFunc([](SAT::ApplyVelocityFrameAgent& thisAgent) {
					glm::vec4 origin = thisAgent.getShape().getTransformedOrigin();
					return origin.z < 0;
				});
				auto redAgent_CardinalNegZ = std::make_shared< SAT::ApplyVelocityFrameAgent >(*redAgent_CardinalDown);
				redAgent_CardinalNegZ->setStartTransform(SAT::ColumnBasedTransform{ {0, 0, 3 }, std::get<rotIdx>(tpl), std::get<scaleIdx>(tpl) });
				redAgent_CardinalNegZ->setNewVelocity(vec3(0, 0, -moveSpeed));
				redAgent_CardinalNegZ->setCustomCompleteTestFunc([](SAT::ApplyVelocityFrameAgent& thisAgent) {
					glm::vec4 origin = thisAgent.getShape().getTransformedOrigin();
					return origin.z > 0;
				});

				KF_CardinalDown->AddKeyFrameAgent(blueAgent_Cardinal);
				KF_CardinalDown->AddKeyFrameAgent(redAgent_CardinalDown);
				UnitTest_CardinalDirections->AddKeyFrame(KF_CardinalDown);

				KF_CardinalLeft->AddKeyFrameAgent(redAgent_CardinalLeft);
				KF_CardinalLeft->AddKeyFrameAgent(blueAgent_Cardinal);
				UnitTest_CardinalDirections->AddKeyFrame(KF_CardinalLeft);

				KF_CardinalUp->AddKeyFrameAgent(redAgent_CardinalUp);
				KF_CardinalUp->AddKeyFrameAgent(blueAgent_Cardinal);
				UnitTest_CardinalDirections->AddKeyFrame(KF_CardinalUp);

				KF_CardinalRight->AddKeyFrameAgent(redAgent_CardinalRight);
				KF_CardinalRight->AddKeyFrameAgent(blueAgent_Cardinal);
				UnitTest_CardinalDirections->AddKeyFrame(KF_CardinalRight);

				KF_CardinalPosZ->AddKeyFrameAgent(redAgent_CardinalPosZ);
				KF_CardinalPosZ->AddKeyFrameAgent(blueAgent_Cardinal);
				UnitTest_CardinalDirections->AddKeyFrame(KF_CardinalPosZ);

				KF_CardinalNegZ->AddKeyFrameAgent(redAgent_CardinalNegZ);
				KF_CardinalNegZ->AddKeyFrameAgent(blueAgent_Cardinal);
				UnitTest_CardinalDirections->AddKeyFrame(KF_CardinalNegZ);
				UnitTests->AddTest(UnitTest_CardinalDirections);
			}

		}

		~CapsuleDemo()
		{
			//ec(glDeleteVertexArrays(1, &cubeVAO));
			ec(glDeleteBuffers(1, &cubeVBO));
			//ec(glDeleteVertexArrays(1, &capsuleVAO));
			ec(glDeleteBuffers(1, &capsuleVBO));
		}

		virtual void handleModuleFocused(GLFWwindow* window) override
		{
			camera.exclusiveGLFWCallbackRegister(window);
		}

		virtual void tickGameLoop(GLFWwindow* window) override
		{
			using glm::vec2;
			using glm::vec3;
			using glm::mat4;


			float currentTime = static_cast<float>(glfwGetTime());
			deltaTime = currentTime - lastFrameTime;
			lastFrameTime = currentTime;

			processInput(window);

			ec(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			ec(glEnable(GL_DEPTH_TEST));
			ec(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			//collision should be adjusted and complete before we attempt to render anything
			blueCapsuleCollision.updateTransform(blueCapsuleTransform.getModelMatrix());
			redCapsuleCollision.updateTransform(redCapsuleTransform.getModelMatrix());

			if (bEnableCollision && !UnitTests->isRunning())
			{
				glm::vec4 mtv;
				if (SAT::Shape::CollisionTest(redCapsuleCollision, blueCapsuleCollision, mtv))
				{
					std::cout << "MTV: "; printVec3(mtv); std::cout << "\tvel: "; printVec3(cachedVelocity); std::cout << "\tpos:"; printVec3(redCapsuleTransform.position); std::cout << std::endl;
					redCapsuleTransform.position += vec3(mtv);
					redCapsuleCollision.updateTransform(redCapsuleTransform.getModelMatrix());
				}
				else
				{
					if (cachedVelocity != vec3(0.0f))
					{
						std::cout << "\t\t\t"; std::cout << "\tvel: "; printVec3(cachedVelocity); std::cout << "\tpos:"; printVec3(redCapsuleTransform.position); std::cout << std::endl;
						cachedVelocity = vec3(0.0f);
					}
				}
			}
			else if (bTickUnitTests)
			{
				UnitTests->tick(deltaTime);
			}

			//draw collision axes
			constexpr float axisSizeScale = 1000.0f;
			const glm::vec3 edgeEdgeColor(0.2f, 0.2f, 0.2f);
			std::vector<glm::vec3> blueFaceAxes, redFaceAxes, edgeEdgeAxes;
			blueCapsuleCollision.appendFaceAxes(blueFaceAxes);
			redCapsuleCollision.appendFaceAxes(redFaceAxes);
			SAT::Shape::appendEdgeXEdgeAxes(blueCapsuleCollision, redCapsuleCollision, edgeEdgeAxes);

			//intentionally draw axes before projections on axes
			//glDisable(GL_DEPTH_TEST);
			mat4 view = camera.getView();
			mat4 projection = glm::perspective(glm::radians(camera.getFOV()), static_cast<float>(width) / height, 0.1f, 100.0f);
			vec3 axisDisplace = bEnableAxisOffset ? axisOffset : vec3(0.0f);
			for (const glm::vec3& axis : blueFaceAxes)
			{
				drawDebugLine(axisDisplace, (axis) * axisSizeScale + axisDisplace, vec3(blueCapsuleColor * 0.25f), mat4(1.0f), view, projection);
				drawDebugLine(axisDisplace, -(axis) * axisSizeScale + axisDisplace, vec3(blueCapsuleColor * 0.25f), mat4(1.0f), view, projection);
			}
			for (const glm::vec3& axis : redFaceAxes)
			{
				drawDebugLine(axisDisplace * 2.0f, axis * axisSizeScale + axisDisplace * 2.0f, vec3(redCapsuleColor * 0.25f), mat4(1.0f), view, projection);
				drawDebugLine(axisDisplace * 2.0f, -axis * axisSizeScale + axisDisplace * 2.0f, vec3(redCapsuleColor * 0.25f), mat4(1.0f), view, projection);
			}
			for (const glm::vec3& axis : edgeEdgeAxes)
			{
				drawDebugLine(vec3(0.0f), axis * axisSizeScale, vec3(edgeEdgeColor / 2.0f), mat4(1.0f), view, projection);
				drawDebugLine(vec3(0.0f), -axis * axisSizeScale, vec3(edgeEdgeColor / 2.0f), mat4(1.0f), view, projection);
			}
			ec(glDepthFunc(GL_ALWAYS)); //always show highlights
			std::vector<glm::vec3> allAxes = blueFaceAxes;
			allAxes.insert(allAxes.end(), redFaceAxes.begin(), redFaceAxes.end());
			allAxes.insert(allAxes.end(), edgeEdgeAxes.begin(), edgeEdgeAxes.end());
			for (const glm::vec3& axis : allAxes)
			{
				SAT::ProjectionRange projection1 = blueCapsuleCollision.projectToAxis(axis);
				vec3 pnt1A = axis * projection1.min;
				vec3 pnt1B = axis * projection1.max;

				SAT::ProjectionRange projection2 = redCapsuleCollision.projectToAxis(axis);
				vec3 pnt2A = axis * projection2.min;
				vec3 pnt2B = axis * projection2.max;

				bool bDisjoint = projection1.max < projection2.min || projection2.max < projection1.min;
				vec3 overlapColor = redCapsuleColor + blueCapsuleColor;
				drawDebugLine(pnt1A, pnt1B, bDisjoint ? blueCapsuleColor : overlapColor, mat4(1.0f), view, projection);
				drawDebugLine(pnt2A, pnt2B, bDisjoint ? redCapsuleColor : overlapColor, mat4(1.0f), view, projection);
			}
			ec(glDepthFunc(GL_LESS)); //default depth func
			ec(glEnable(GL_DEPTH_TEST));
			//ec(glBindVertexArray(cubeVAO));
			{//render light
				mat4 model = lightTransform.getModelMatrix();
				lampShader.use();
				lampShader.setUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
				lampShader.setUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));  //since we don't update for each cube, it would be more efficient to do this outside of the loop.
				lampShader.setUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
				lampShader.setUniform3f("lightColor", lightColor);
				enableAttributes_cubeVBO();
				ec(glDrawArrays(GL_TRIANGLES, 0, 36));
				disableAttributes_cubeVBO();

			}
			objShader.use();
			objShader.setUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
			objShader.setUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
			objShader.setUniform3f("lightPosition", lightTransform.position);
			objShader.setUniform3f("lightColor", lightColor);
			objShader.setUniform3f("cameraPosition", camera.getPosition());
			
			float pointSize = 8.0f;

			if (bDrawDebugSATInfo)
			{
				SAT::drawDebugCollisionShape(debugShapeShader, redCapsuleCollision, redCapsuleColor, pointSize, true, true, view, projection);
				SAT::drawDebugCollisionShape(debugShapeShader, blueCapsuleCollision, blueCapsuleColor,pointSize, true, true, view, projection);
			}
			else
			{
				//ec(glBindVertexArray(capsuleVAO));
				enableAttributes_capsuleVBO();
				GLuint numCapsuleVerts = capsuleVertSize / (sizeof(float) * 6);
				{ //render red cube
					//resist temptation to update collision transform here, collision should be separated from rendering (for mtv corrections)
					mat4 model = redCapsuleTransform.getModelMatrix();
					objShader.setUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
					mat4 transposeInverseModel = transpose(inverse(model));
					objShader.setUniformMatrix4fv("transposeInverseModel", 1, GL_FALSE, glm::value_ptr(transposeInverseModel));
					objShader.setUniform3f("objectColor", redCapsuleColor);
					ec(glDrawArrays(GL_TRIANGLES, 0, numCapsuleVerts));
				}
				{// render blue cube
					//resist temptation to update collision transform here, collision should be separated from rendering (for mtv corrections)
					mat4 model = blueCapsuleTransform.getModelMatrix();
					objShader.setUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
					mat4 transposeInverseModel = transpose(inverse(model));
					objShader.setUniformMatrix4fv("transposeInverseModel", 1, GL_FALSE, glm::value_ptr(transposeInverseModel));
					objShader.setUniform3f("objectColor", blueCapsuleColor);
					ec(glDrawArrays(GL_TRIANGLES, 0, numCapsuleVerts));
				}
				disableAttributes_capsuleVBO();
			}
		}
	
	private:

		inline bool isZeroVector(const glm::vec3& testVec)
		{
			constexpr float EPSILON = 0.001f;
			return glm::length2(testVec) < EPSILON * EPSILON;
		}

		void processInput(GLFWwindow* window)
		{
			static int initializeOneTimePrintMsg = []() -> int {
				std::cout << "Hold ALT to move objects with WASD-EQ" << std::endl
					<< " Hold ALT + CONTROL to rotate objects with WASD-EQ" << std::endl
					<< " Press T to toggle which the object is moved " << std::endl
					<< " Press V to toggle locking translations(and rotations) to camera (easier movements) " << std::endl
					<< " Press M to toggle slightly displacing axes so that parallel axes are visible simultaneously " << std::endl
					<< " Press P to print debug information" << std::endl
					<< " Press R to reset object positions to default." << std::endl
					<< " Press C to toggle collision detection" << std::endl
					<< " Press 9/0 to decrease/increase scale" << std::endl
					<< " Press U to start unit tests; press left/right to skip through unit tests" << std::endl
					<< " " << std::endl
					//vec3 capture1_D, capture2_G, capture3_F, capture4_r, capture5_m, capture6_Gl, capture7_Gv, capture8_albedo, capture9_normal;
					<< std::endl;
				return 0;
			}();


			using glm::vec3; using glm::vec4;
			static Deprecated_InputTracker input; //using static vars in polling function may be a bad idea since cpp11 guarantees access is atomic -- I should bench this
			input.updateState(window);
#ifndef HTML_BUILD
			if (input.isKeyJustPressed(window, GLFW_KEY_ESCAPE))
			{
				glfwSetWindowShouldClose(window, true);
			}
#endif //!HTML_BUILD
			if (input.isKeyJustPressed(window, GLFW_KEY_T))
			{
				transformTarget = transformTarget == &blueCapsuleTransform ? &redCapsuleTransform : &blueCapsuleTransform;
			}
			if (input.isKeyJustPressed(window, GLFW_KEY_V))
			{
				bUseCameraAxesForObjectMovement = !bUseCameraAxesForObjectMovement;
			}
			if (input.isKeyJustPressed(window, GLFW_KEY_M))
			{
				bEnableAxisOffset = !bEnableAxisOffset;
			}
			if (input.isKeyJustPressed(window, GLFW_KEY_R))
			{
				redCapsuleTransform = defaultRedCapsuleTransform;
				blueCapsuleTransform = defaultBlueCapsuleTransform;
			}
			if (input.isKeyJustPressed(window, GLFW_KEY_C))
			{
				bEnableCollision = !bEnableCollision;
			}
			if (input.isKeyJustPressed(window, GLFW_KEY_F))
			{
				bDrawDebugSATInfo = !bDrawDebugSATInfo;
			}
			const glm::vec3 scaleSpeedPerSec(1, 1, 1);
			if (input.isKeyDown(window, GLFW_KEY_9))
			{
				vec3 delta = -scaleSpeedPerSec * deltaTime;
				transformTarget->scale += delta;
				redCapsuleCollision.updateTransform(redCapsuleTransform.getModelMatrix());
				blueCapsuleCollision.updateTransform(blueCapsuleTransform.getModelMatrix());
			}
			if (input.isKeyDown(window, GLFW_KEY_0))
			{
				transformTarget->scale += scaleSpeedPerSec * deltaTime;
				redCapsuleCollision.updateTransform(redCapsuleTransform.getModelMatrix());
				blueCapsuleCollision.updateTransform(blueCapsuleTransform.getModelMatrix());
			}
			if (input.isKeyJustPressed(window, GLFW_KEY_P))
			{
				using std::cout; using std::endl;
				cout << "camera position: ";  printVec3(camera.getPosition()); cout << " yaw:" << camera.getYaw() << " pitch: " << camera.getPitch() << endl;
				cout << "camera front:"; printVec3(camera.getFront()); cout << " right:"; printVec3(camera.getRight()); cout << " up:"; printVec3(camera.getUp()); cout << endl;
				auto printXform = [](ColumnBasedTransform& transform) {
					glm::vec3 rotQuat{ transform.rotQuat.x, transform.rotQuat.y, transform.rotQuat.z };
					cout << "pos:";  printVec3(transform.position); cout << " rotation:"; printVec3(rotQuat); cout << "w" <<transform.rotQuat.w <<" scale:"; printVec3(transform.scale); cout << endl;
				};
				cout << "red objecpt =>"; printXform(redCapsuleTransform);
				cout << "blue object =>"; printXform(blueCapsuleTransform);
			}
			if (input.isKeyJustPressed(window, GLFW_KEY_U))
			{
				bTickUnitTests = !bTickUnitTests;
				if (bTickUnitTests)
				{
					UnitTests->restartAllTests();
					UnitTests->start();
				}
				else
				{
					UnitTests->stop();
				}
			}
			// -------- MOVEMENT -----------------
			if (SAT::bEnableCameraMove && !(input.isKeyDown(window, GLFW_KEY_LEFT_ALT) || input.isKeyDown(window, GLFW_KEY_LEFT_SHIFT)))
			{
				camera.handleInput(window, deltaTime);
			}
			else
			{
				bool bUpdateMovement = false;
				vec3 movementInput = vec3(0.0f);
				vec3 cachedTargetPos = transformTarget->position;

				/////////////// W /////////////// 
				if (input.isKeyDown(window, GLFW_KEY_W))
				{
					if (SAT::bEnableRotateTarget || input.isKeyDown(window, GLFW_KEY_LEFT_CONTROL) || input.isKeyDown(window, GLFW_KEY_RIGHT_CONTROL))
					{
						if (bUseCameraAxesForObjectMovement)
						{
							glm::quat newRotation = glm::angleAxis(glm::radians(-rotationSpeed * deltaTime), camera.getRight()) * transformTarget->rotQuat;
							transformTarget->rotQuat = newRotation;
						}
						else
						{
							glm::quat newRotation = glm::angleAxis(glm::radians(-rotationSpeed * deltaTime), vec3(1,0,0)) * transformTarget->rotQuat;
							transformTarget->rotQuat = newRotation;
						}
					}
					else
					{
						if (bUseCameraAxesForObjectMovement)
						{
							movementInput += camera.getUp();
						}
						else
						{
							movementInput += vec3(0, 1, 0);
						}
					}
				}
				/////////////// S /////////////// 
				if (input.isKeyDown(window, GLFW_KEY_S))
				{
					if (SAT::bEnableRotateTarget || input.isKeyDown(window, GLFW_KEY_LEFT_CONTROL) || input.isKeyDown(window, GLFW_KEY_RIGHT_CONTROL))
					{
						if (bUseCameraAxesForObjectMovement)
						{
							glm::quat newRotation = glm::angleAxis(glm::radians(rotationSpeed * deltaTime), camera.getRight()) * transformTarget->rotQuat;
							transformTarget->rotQuat = newRotation;
						}
						else
						{
							glm::quat newRotation = glm::angleAxis(glm::radians(rotationSpeed * deltaTime), vec3(1,0,0)) * transformTarget->rotQuat;
							transformTarget->rotQuat = newRotation;
						}
					}
					else
					{
						if (bUseCameraAxesForObjectMovement)
						{
							movementInput += -camera.getUp();
						}
						else
						{
							movementInput += vec3(0, -1, 0);
						}
					}
				}
				/////////////// Q /////////////// 
				if (input.isKeyDown(window, GLFW_KEY_Q))
				{
					if (SAT::bEnableRotateTarget || input.isKeyDown(window, GLFW_KEY_LEFT_CONTROL) || input.isKeyDown(window, GLFW_KEY_RIGHT_CONTROL))
					{
						if (bUseCameraAxesForObjectMovement)
						{
							glm::quat newRotation = glm::angleAxis(glm::radians(rotationSpeed * deltaTime), -camera.getFront()) * transformTarget->rotQuat;
							transformTarget->rotQuat = newRotation;
						}
						else
						{
							glm::quat newRotation = glm::angleAxis(glm::radians(rotationSpeed * deltaTime), vec3(0, 0, 1)) * transformTarget->rotQuat;
							transformTarget->rotQuat = newRotation;
						}
					}
					else
					{
						if (bUseCameraAxesForObjectMovement)
						{
							movementInput += -camera.getFront();
						}
						else
						{
							movementInput += vec3(0, 0, 1);
						}
					}
				}
				/////////////// E /////////////// 
				if (input.isKeyDown(window, GLFW_KEY_E))
				{
					if (SAT::bEnableRotateTarget || input.isKeyDown(window, GLFW_KEY_LEFT_CONTROL) || input.isKeyDown(window, GLFW_KEY_RIGHT_CONTROL))
					{
						if (bUseCameraAxesForObjectMovement)
						{
							glm::quat newRotation = glm::angleAxis(glm::radians(-rotationSpeed * deltaTime), -camera.getFront()) * transformTarget->rotQuat;
							transformTarget->rotQuat = newRotation;
						}
						else
						{
							glm::quat newRotation = glm::angleAxis(glm::radians(-rotationSpeed * deltaTime), vec3(0, 0, 1)) * transformTarget->rotQuat;
							transformTarget->rotQuat = newRotation;
						}
					} 
					else
					{
						if (bUseCameraAxesForObjectMovement)
						{
							movementInput += camera.getFront();
						}
						else
						{
							movementInput += vec3(0, 0, -1);
						}
					}
				}
				/////////////// A /////////////// 
				if (input.isKeyDown(window, GLFW_KEY_A))
				{
					if (SAT::bEnableRotateTarget || input.isKeyDown(window, GLFW_KEY_LEFT_CONTROL) || input.isKeyDown(window, GLFW_KEY_RIGHT_CONTROL))
					{
						if (bUseCameraAxesForObjectMovement)
						{
							vec3 toTarget = transformTarget->position - camera.getPosition();
							vec3 adjustedUp = glm::normalize(glm::cross(camera.getRight(), glm::normalize(toTarget)));
							glm::quat newRotation = glm::angleAxis(glm::radians(-rotationSpeed * deltaTime), adjustedUp) * transformTarget->rotQuat;
							//glm::quat newRotation = glm::angleAxis(glm::radians(-rotationSpeed * deltaTime), camera.getUp()) * transformTarget->rotQuat;
							transformTarget->rotQuat = newRotation;
						}
						else
						{
							glm::quat newRotation = glm::angleAxis(glm::radians(-rotationSpeed * deltaTime), glm::vec3(0,0,1)) * transformTarget->rotQuat;
							transformTarget->rotQuat = newRotation;
						}
					}
					else
					{
						if (bUseCameraAxesForObjectMovement)
						{
							movementInput += -camera.getRight();
						}
						else
						{
							movementInput += vec3(-1, 0, 0);
						}
					}
				}
				/////////////// D /////////////// 
				if (input.isKeyDown(window, GLFW_KEY_D))
				{
					if (SAT::bEnableRotateTarget || input.isKeyDown(window, GLFW_KEY_LEFT_CONTROL) || input.isKeyDown(window, GLFW_KEY_RIGHT_CONTROL))
					{
						if (bUseCameraAxesForObjectMovement)
						{
							vec3 toTarget = transformTarget->position - camera.getPosition();
							vec3 adjustedUp = glm::normalize(glm::cross(camera.getRight(), glm::normalize(toTarget)));
							glm::quat newRotation = glm::angleAxis(glm::radians(rotationSpeed * deltaTime), adjustedUp) * transformTarget->rotQuat;
							//glm::quat newRotation = glm::angleAxis(glm::radians(rotationSpeed * deltaTime), camera.getUp()) * transformTarget->rotQuat;
							transformTarget->rotQuat = newRotation;
						}
						else
						{
							glm::quat newRotation = glm::angleAxis(glm::radians(rotationSpeed * deltaTime), glm::vec3(0, 0, 1)) * transformTarget->rotQuat;
							transformTarget->rotQuat = newRotation;
						}
					}
					else
					{
						if (bUseCameraAxesForObjectMovement)
						{
							movementInput += camera.getRight();
						}
						else
						{
							movementInput += vec3(1, 0, 0);
						}
					}
				}
				//probably should use epsilon comparison for when opposite buttons held, but this should cover compares against initialization
				if (movementInput != vec3(0.0f))
				{
					movementInput = glm::normalize(movementInput);
					cachedVelocity = deltaTime * moveSpeed * movementInput;
					transformTarget->position += cachedVelocity;
				}
				if (bBlockCollisionFailures)
				{
					vec4 mtv;
					if (SAT::Shape::CollisionTest(redCapsuleCollision, blueCapsuleCollision, mtv))
					{
						//undo the move
						transformTarget->position = cachedTargetPos;
						//redCubeTransform.position += vec3(mtv);
					}
				}
			}
			if (input.isKeyJustPressed(window, GLFW_KEY_N))
			{
				std::cout << "debug force collision test" << std::endl;
				SAT::Shape::CollisionTest(redCapsuleCollision, blueCapsuleCollision);
			}

			// -------- UNIT TEST ---------
			if (input.isKeyJustPressed(window, GLFW_KEY_UP))
			{
			}
			else if (input.isKeyJustPressed(window, GLFW_KEY_DOWN))
			{
				UnitTests->restartCurrentTest();
			}
			else if (input.isKeyJustPressed(window, GLFW_KEY_LEFT))
			{
				UnitTests->previousTest();
			}
			else if (input.isKeyJustPressed(window, GLFW_KEY_RIGHT))
			{
				UnitTests->nextTest();
			}

		}


	};

	void true_main()
	{
		//using glm::vec2;
		//using glm::vec3;
		//using glm::mat4;

		//int width = 1200;
		//int height = 800;
		//
		//GLFWwindow* window = init_window(width, height);

		//glViewport(0, 0, width, height);
		//glfwSetFramebufferSizeCallback(window, [](GLFWwindow*window, int width, int height) {  glViewport(0, 0, width, height); });
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		//CapsuleDemo capsuleDemo(width, height);
		//capsuleDemo.handleModuleFocused(window);

		///////////////////////////////////////////////////////////////////////
		//// Game Loop
		///////////////////////////////////////////////////////////////////////
		//while (!glfwWindowShouldClose(window))
		//{
		//	capsuleDemo.tickGameLoop(window);

		//	glfwSwapBuffers(window);
		//	glfwPollEvents();
		//}

		//glfwSwapBuffers(window);
		//glfwPollEvents();

		//glfwTerminate();
	}
}

std::shared_ptr<ISATDemo> factory_CapsuleShape(int width, int height)
{
	return std::make_shared<CapsuleDemo>(width, height);
}

#endif //WITH_SAT_DEMO_BUILD

//int main()
//{
//	true_main();
//}