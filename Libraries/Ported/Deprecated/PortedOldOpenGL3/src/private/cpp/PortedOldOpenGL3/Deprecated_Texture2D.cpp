///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////
#ifdef WITH_OLD_OPENGL3_PROJECT_BUILD

#include<Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h>

//#include <glad\glad.h>
//#include <GLFW\glfw3.h>

#include "PortedOldOpenGL3/Deprecated_Texture2D.h"
//#include "libraries/stb_image.h"
#include <stb_image.h>



Deprecated_Texture2D::Deprecated_Texture2D(
	const std::string& filePath,
	/*GLint*/int wrap_s,
	/*GLint*/int wrap_t,
	/*GLint*/int min_filter,
	/*GLint*/int mag_filter)
		: bLoadSuccess(true), textureId(/*-1*/0)
{
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) 
	{ 
		
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
		stbi_image_free(data);
	}
	else
	{
		bLoadSuccess = false;
	}
}

Deprecated_Texture2D::~Deprecated_Texture2D()
{
	if (textureId >= 0)
	{
		glDeleteTextures(1, &textureId);
	}
}

/** Warning: this will clear what ever texture is currently bound*/
void Deprecated_Texture2D::setTexParami(/*GLenum */ uint32_t option, /*GLint*/ int Param)
{
	if (bLoadSuccess && textureId > 0)
	{
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, option, Param);
	}
}

#endif //WITH_OLD_OPENGL3_PROJECT_BUILD
