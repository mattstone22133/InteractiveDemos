#pragma once

///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////

#ifdef WITH_OLD_OPENGL3_PROJECT_BUILD
#include <string>
#include <cstdint>

class Deprecated_Texture2D
{
public:
	Deprecated_Texture2D(
		const std::string& filePath,
		int /*GLint*/ wrap_s = GL_MIRRORED_REPEAT,
		int /*GLint*/ wrap_t = GL_MIRRORED_REPEAT,
		int /*GLint*/ min_filter = GL_LINEAR,
		int /*GLint*/ mag_filter = GL_LINEAR
	);

	~Deprecated_Texture2D();

	void setTexParami(/*GLenum*/uint32_t option, /*GLint*/int Param);
	bool getLoadSuccess() { return bLoadSuccess; }
	uint32_t/*GLuint*/ getTextureId() { return textureId; }

	int getWidth() { return width; }
	int getHeight() { return height; }
private:
	bool bLoadSuccess;
	uint32_t/*GLuint*/ textureId;
	int width;
	int height;
	int nrChannels;
};

#endif //WITH_OLD_OPENGL3_PROJECT_BUILD
