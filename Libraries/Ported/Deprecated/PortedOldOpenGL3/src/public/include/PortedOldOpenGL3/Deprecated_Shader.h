#pragma once
///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////
#ifdef WITH_OLD_OPENGL3_PROJECT_BUILD

#include<string>
#include<memory>
#include<vector>
#include<cstdint>

//#include <glad/glad.h> 
#include<Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Deprecated_Texture2D;

class Deprecated_Shader
{
private:
	void initShaderHelper(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, const std::string& geometryShaderFilePath, bool stringsAreFilePaths = true);

public:
	explicit Deprecated_Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, bool stringsAreFilePaths = true);
	explicit Deprecated_Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, const std::string& geometryShaderFilePath, bool stringsAreFilePaths = true);
	virtual ~Deprecated_Shader();

	Deprecated_Shader(const Deprecated_Shader&) = delete;
	Deprecated_Shader(const Deprecated_Shader&&) = delete;
	Deprecated_Shader& operator=(const Deprecated_Shader&) = delete;
	Deprecated_Shader& operator=(const Deprecated_Shader&&) = delete;

	bool createFailed() { return failed; }

	void use(bool activate = true);
	uint32_t /*GLuint*/ getId();

	void setUniform4f(const char* uniform, float red, float green, float blue, float alpha);
	void setUniform4f(const char* uniform, const glm::vec4& values);
	void setUniform3f(const char* uniform, float red, float green, float blue);
	void setUniform3f(const char* uniform, const glm::vec3& values);
	void setUniform1f(const char* uniformName, float value);
	void setUniform1i(const char* uniformname, int newValue);
	void setUniformMatrix4fv(const char* uniform, int numberMatrices, uint32_t /*GLuint*/ normalize, const float* data);

	/** DEPRECATED: I made this earlier on and now handle textures differently. Leaving so my early files still work.*/
	void addTexture(std::shared_ptr<Deprecated_Texture2D>& texture, const std::string& textureSampleName);

	/** DEPRECATED: I made this earlier on and now handle textures differently. Leaving so my early files still work.*/
	void activateTextures();


private:
	bool failed;
	bool active;
	uint32_t /*GLuint*/ linkedProgram;

	std::vector<std::shared_ptr<Deprecated_Texture2D>> textures;

private:
	bool shaderCompileSuccess(GLuint shaderID);
	bool programLinkSuccess(GLuint programID);
};

#endif //WITH_OLD_OPENGL3_PROJECT_BUILD
