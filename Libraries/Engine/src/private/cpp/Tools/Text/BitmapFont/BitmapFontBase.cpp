#include "Tools/Text/BitmapFont/BitmapFontBase.h"
#include "Utils/Platform//OpenGLES2/PlatformOpenGLESInclude.h"

namespace Engine
{
	GlyphRenderer::~GlyphRenderer()
	{
		//moved over to GPUResource management
	}

	void GlyphRenderer::onAcquireGPUResources()
	{
		_createBuffers();
	}

	void GlyphRenderer::_createBuffers()
	{
		//ec(glGenVertexArrays(1, &vao));
		//ec(glBindVertexArray(vao));


		// transform this by scale [0.f,0.f,0.f,    1.f,0.f,0.f,    0.f,1.f,0.f,   1.f,1.f,0.f]
		float correctedPos[] = { 0.f, 0.f, 0.f,     width, 0.f, 0.f,     0.f, height, 0.f,       width, height, 0.f };
		ec(glGenBuffers(1, &posVBO));
		ec(glBindBuffer(GL_ARRAY_BUFFER, posVBO));
		ec(glBufferData(GL_ARRAY_BUFFER, sizeof(correctedPos), &correctedPos[0], GL_STATIC_DRAW));
		preparePositionVBO();


		//quad indices
		// 2---3
		// | \ |
		// 0---1
		float UVs[] = {
			uvPos.x, uvPos.y,					//idx 0
			uvPos.x + width, uvPos.y,           //idx 1
			uvPos.x, uvPos.y + height,			//idx 2
			uvPos.x + width, uvPos.y + height   //idx 3
		};
		ec(glGenBuffers(1, &uvVBO));
		ec(glBindBuffer(GL_ARRAY_BUFFER, uvVBO));
		ec(glBufferData(GL_ARRAY_BUFFER, sizeof(UVs), &UVs[0], GL_STATIC_DRAW));
		prepareUVVBO();

		ec(glGenBuffers(1, &ebo)); //this EBO could be defined separately as it never changes
		ec(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
		ec(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW));


		uint32_t shaderProgram = glyphShader->getProgramId();
		projection_ul = glGetUniformLocation(shaderProgram, "projection");
		view_ul = glGetUniformLocation(shaderProgram, "view");
		model_ul = glGetUniformLocation(shaderProgram, "model");
		flipV_ul = glGetUniformLocation(shaderProgram, "flipV");
		color_ul = glGetUniformLocation(shaderProgram, "color");
		textureSampler_ul = glGetUniformLocation(shaderProgram, "texture0");

		//ec(glBindVertexArray(0));
	}

	void GlyphRenderer::onReleaseGPUResources()
	{
		if (glIsBuffer(posVBO))
		{
			ec(glDeleteBuffers(1, &posVBO));
		}
		if (glIsBuffer(uvVBO))
		{
			ec(glDeleteBuffers(1, &uvVBO));
		}
		if (glIsBuffer(ebo))
		{
			ec(glDeleteBuffers(1, &ebo));
		}

	}

	void GlyphRenderer::render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model)
	{
		if (!hasAcquiredResources())
		{
			return;
		}


		if (!bHasValidAttributeLocations)
		{
			tryReadAttributesLocations();
		}

		assert(glyphShader);
		{
			//these should be redundant and should be optimized by setting up outside this call
			glyphShader->use();
			ec(glActiveTexture(GL_TEXTURE0));
			ec(glBindTexture(GL_TEXTURE_2D, fontTextureObj->glTextureId));
			ec(glUniformMatrix4fv(projection_ul, 1, GL_FALSE, glm::value_ptr(projection)));
			ec(glUniformMatrix4fv(view_ul, 1, GL_FALSE, glm::value_ptr(view)));
		}

		//this is definitely not the most efficient implementation
		ec(glUniform1i(textureSampler_ul, 0/*0 corresponds to GL_TEXTURE0*/));
		ec(glUniform3f(color_ul, color.r, color.g, color.b));
		ec(glUniform1f(flipV_ul, -1.f));
		ec(glUniformMatrix4fv(model_ul, 1, GL_FALSE, glm::value_ptr(model)));

		preparePositionVBO();
		prepareUVVBO();
		bindEBO();
		//ec(glBindVertexArray(vao));
		//ec(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
		ec(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, /*offset*/0));
		//ec(glBindVertexArray(0));
	}

	void GlyphRenderer::tryReadAttributesLocations()
	{
		if (glyphShader && glIsProgram(glyphShader->getProgramId()))
		{
			attribLoc_pos = glGetAttribLocation(glyphShader->getProgramId(), posAttribName);
			attribLoc_uv = glGetAttribLocation(glyphShader->getProgramId(), uvAttribName);
			bHasValidAttributeLocations = true;
		}
		else
		{
			//something sensible it will hopefully just work if shader hasn't had its resourced provided yet
			attribLoc_pos = 0;
			attribLoc_uv = 1;
		}
	}

	void GlyphRenderer::preparePositionVBO()
	{
		ec(glBindBuffer(GL_ARRAY_BUFFER, posVBO));
		ec(glVertexAttribPointer(attribLoc_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0)));
		ec(glEnableVertexAttribArray(attribLoc_pos));
	}

	void GlyphRenderer::prepareUVVBO()
	{
		ec(glBindBuffer(GL_ARRAY_BUFFER, uvVBO));
		ec(glVertexAttribPointer(attribLoc_uv, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0)));
		ec(glEnableVertexAttribArray(attribLoc_uv));
	}


	void GlyphRenderer::bindEBO()
	{
		ec(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
	}



}


