///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////
#ifdef WITH_SAT_DEMO_BUILD
#include "SATUtils.h"
namespace SAT
{
	void printVec3(glm::vec3 v)
	{
		std::cout << "[" << v.x << ", " << v.y << ", " << v.z << "]";
	}

	void drawDebugLine(const glm::vec3& pntA, const glm::vec3& pntB, const glm::vec3& color, 
		const glm::mat4& /*model*/, const glm::mat4& view, const glm::mat4 projection)
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
#endif //WITH_SAT_DEMO_BUILD

