///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////

#ifdef WITH_OLD_OPENGL3_PROJECT_BUILD

//#include<glad/glad.h>
//#include<GLFW/glfw3.h>
#include<Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h>

#include "PortedOldOpenGL3/Deprecated_Shader.h"
#include "PortedOldOpenGL3/Deprecated_Utilities.h"
#include "PortedOldOpenGL3/Deprecated_Texture2D.h"


/** 
	Utility class that uses RAII to cache and restore the currently bound shader in its ctor.
	When the dtor is called it restores the previous shader.

	Useful for setting uniforms. Since a shader must be active when the uniform is set, this can be used in the scope of a setter.
	the setter will create an instance of this class, update the uniform, and when the function goes out scope the previous shader will be restored.
*/
class RAII_ScopedShaderSwitcher
{
//use below to control whether this class functions at compile time.
#define UNIFORM_UPDATE_CACHE_PREVIOUS_SHADER 1

private:
#if UNIFORM_UPDATE_CACHE_PREVIOUS_SHADER
	GLint cachedPreviousShader;
#endif // UNIFORM_UPDATE_CACHE_PREVIOUS_SHADER

public:
	RAII_ScopedShaderSwitcher(GLint switchToThisShader)
	{
#if UNIFORM_UPDATE_CACHE_PREVIOUS_SHADER
		//Cache previous shader and restore it after update; NOTE: I've read that the get* can cause performance hits in multi-threaded opengl drivers: https://www.opengl.org/discussion_boards/showthread.php/177044-How-do-I-get-restore-the-current-shader //Article on opengl perf https://software.intel.com/en-us/articles/opengl-performance-tips-avoid-opengl-calls-that-synchronize-cpu-and-gpu
		glGetIntegerv(GL_CURRENT_PROGRAM, &cachedPreviousShader);
		glUseProgram(switchToThisShader);
#endif // UNIFORM_UPDATE_CACHE_PREVIOUS_SHADER
	}
	~RAII_ScopedShaderSwitcher()
	{
#if UNIFORM_UPDATE_CACHE_PREVIOUS_SHADER
		//restore previous shader
		glUseProgram(cachedPreviousShader);
#endif // UNIFORM_UPDATE_CACHE_PREVIOUS_SHADER
	}
};

void Deprecated_Shader::initShaderHelper(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath,
#if ENABLE_MODERN_OPENGL_SHADERS
	const std::string& geometryShaderFilePath,
#else
	const std::string& /*geometryShaderFilePath*/,
#endif //ENABLE_MODERN_OPENGL_SHADERS
	bool stringsAreFilePaths /*= true*/)
{

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
#if ENABLE_MODERN_OPENGL_SHADERS
	bool hasGeometryShader = geometryShaderFilePath != "";
	GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
#endif //ENABLE_MODERN_OPENGL_SHADERS

	//LOAD SOURCES FROM FILE
	std::string vertShaderSrc, fragShaderSrc, geoShaderSrc;
	if (stringsAreFilePaths)
	{
		failed = !Deprecated_Utils::convertFileToString(vertexShaderFilePath, vertShaderSrc);
		if (failed)
		{
			std::cerr << "failed to load vertex shader from file" << std::endl;
			return;
		}
		failed = !Deprecated_Utils::convertFileToString(fragmentShaderFilePath, fragShaderSrc);
		if (failed)
		{
			std::cerr << "failed to load fragment shader from file" << std::endl;
			return;
		}
#if ENABLE_MODERN_OPENGL_SHADERS
		if (hasGeometryShader)
		{
			failed = !Deprecated_Utils::convertFileToString(geoShaderSrc, geoShaderSrc);
			if (failed)
			{
				std::cerr << "failed to load fragment shader from file" << std::endl;
				return;
			}
		}
#endif //ENABLE_MODERN_OPENGL_SHADERS
	}
	else
	{
		//This is a refactor of the original class to allow directly passing the shader source.
		//could probably refactor this ctor so that we don't need to do this superfluous copy; 
		//but since this is only generally called at start up I'll let it make an extra copy
		vertShaderSrc = vertexShaderFilePath;
		fragShaderSrc = fragmentShaderFilePath;
#if ENABLE_MODERN_OPENGL_SHADERS
		geoShaderSrc = geometryShaderFilePath;
#endif //ENABLE_MODERN_OPENGL_SHADERS
	}

	//SET SHADER SOURCES
	const char* vertShaderSource_cstr = vertShaderSrc.c_str();
	glShaderSource(vertexShader, 1, &vertShaderSource_cstr, nullptr);

	const char* fragShaderSource_cstr = fragShaderSrc.c_str();
	glShaderSource(fragmentShader, 1, &fragShaderSource_cstr, nullptr);

#if ENABLE_MODERN_OPENGL_SHADERS
	const char* geoShaderSource_cstr = geoShaderSrc.c_str();
	glShaderSource(geometryShader, 1, &geoShaderSource_cstr, nullptr);
#endif //ENABLE_MODERN_OPENGL_SHADERS

	//COMPILE SHADERS
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
#if ENABLE_MODERN_OPENGL_SHADERS
	if (hasGeometryShader)
	{
		glCompileShader(geometryShader);
	}
#endif //ENABLE_MODERN_OPENGL_SHADERS

	failed = !shaderCompileSuccess(vertexShader);
	if (failed)
	{
		std::cerr << "VERTEX SHADER failed to compile the vertex shader" << std::endl; 
		return;
	}
	failed = !shaderCompileSuccess(fragmentShader);
	if (failed)
	{
		std::cerr << "FRAGMENT SHADER: failed to compile the fragment shader" << std::endl;
		return;
	}
#if ENABLE_MODERN_OPENGL_SHADERS
	if (hasGeometryShader)
	{
		failed = !shaderCompileSuccess(geometryShader);
		if (failed)
		{
			std::cerr << "GEOMETRY SHADER: failed to compile the geom shader" << std::endl;
			return;
		}
	}
#endif //ENABLE_MODERN_OPENGL_SHADERS

	//ATTACH AND LINK SHADERS
	linkedProgram = glCreateProgram();
	glAttachShader(linkedProgram, vertexShader);
	glAttachShader(linkedProgram, fragmentShader);
#if ENABLE_MODERN_OPENGL_SHADERS
	if (hasGeometryShader)
	{
		glAttachShader(linkedProgram, geometryShader);
	}
#endif //ENABLE_MODERN_OPENGL_SHADERS
	glLinkProgram(linkedProgram);
	failed = !programLinkSuccess(linkedProgram);
	if (failed)
	{
		std::cerr << "failed to link shader program" << std::endl;
		return;
	}

	//CLEAN UP
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
#if ENABLE_MODERN_OPENGL_SHADERS
	glDeleteShader(geometryShader);
#endif //ENABLE_MODERN_OPENGL_SHADERS
}

Deprecated_Shader::Deprecated_Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, bool stringsAreFilePaths /*= true*/)
	:
	failed(false),
	active(false),
	linkedProgram(0),
	textures()
{
	std::string blankGeometryShader = "";
	initShaderHelper(vertexShaderFilePath, fragmentShaderFilePath, blankGeometryShader, stringsAreFilePaths);
}

Deprecated_Shader::Deprecated_Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, const std::string& geometryShaderFilePath, bool stringsAreFilePaths) :
	failed(false),
	active(false),
	linkedProgram(0),
	textures()
{
	initShaderHelper(vertexShaderFilePath, fragmentShaderFilePath, geometryShaderFilePath, stringsAreFilePaths);
}

Deprecated_Shader::~Deprecated_Shader()
{
}

void Deprecated_Shader::use(bool activate)
{
	if (activate)
	{
		glUseProgram(linkedProgram);
		activateTextures();
	}
	else
	{
		glUseProgram(0);
	}
}

GLuint Deprecated_Shader::getId()
{
	return linkedProgram;
}

void Deprecated_Shader::setUniform4f(const char* uniform, float red, float green, float blue, float alpha)
{
	RAII_ScopedShaderSwitcher scoped(linkedProgram);

	//do not need to be using shader to query location of uniform
	int uniformLocation = glGetUniformLocation(linkedProgram, uniform);

	//must be using the shader to update uniform value
	glUseProgram(linkedProgram);
	glUniform4f(uniformLocation, red, green, blue, alpha);
}

void Deprecated_Shader::setUniform4f(const char* uniform, const glm::vec4& values)
{
	setUniform4f(uniform, values.r, values.g, values.b, values.a);
}

void Deprecated_Shader::setUniform3f(const char* uniform, float red, float green, float blue)
{
	RAII_ScopedShaderSwitcher scoped(linkedProgram);

	int uniformLocation = glGetUniformLocation(linkedProgram, uniform);
	glUseProgram(linkedProgram);
	glUniform3f(uniformLocation, red, green, blue);
}

void Deprecated_Shader::setUniform3f(const char* uniform, const glm::vec3& vals)
{
	RAII_ScopedShaderSwitcher scoped(linkedProgram);

	int uniformLocation = glGetUniformLocation(linkedProgram, uniform);
	glUseProgram(linkedProgram);
	glUniform3f(uniformLocation, vals.r, vals.g, vals.b);
}

void Deprecated_Shader::setUniform1i(const char* uniform, int newValue)
{
	RAII_ScopedShaderSwitcher scoped(linkedProgram);

	int uniformLocation = glGetUniformLocation(linkedProgram, uniform);
	glUseProgram(linkedProgram);
	glUniform1i(uniformLocation, newValue);
}


void Deprecated_Shader::setUniformMatrix4fv(const char* uniform, int numberMatrices, uint32_t/*GLuint*/ transpose, const float* data)
{
	RAII_ScopedShaderSwitcher scoped(linkedProgram);

	int uniformLocation = glGetUniformLocation(linkedProgram, uniform);
	glUseProgram(linkedProgram);
	glUniformMatrix4fv(uniformLocation, numberMatrices, GLboolean(transpose), data);
}

void Deprecated_Shader::setUniform1f(const char* uniformName, float value)
{
	GLuint uniformLocationInShader = glGetUniformLocation(getId(), uniformName);
	glUseProgram(linkedProgram);
	glUniform1f(uniformLocationInShader, value);
}

void Deprecated_Shader::addTexture(std::shared_ptr<Deprecated_Texture2D>& texture, const std::string& textureSampleName)
{
	/** DEPRECATED: I made this earlier on and now handle textures differently. Leaving so my early files still work.*/
	if (texture)
	{
		GLuint textureShaderSampleLocation = glGetUniformLocation(getId(), textureSampleName.c_str());
		textures.push_back(texture);
		//NOTE: we're basically setting to use which: GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, ... GL_TEXTURE16
		GLuint GL_TEXTURENUM = GLuint(textures.size() - 1);
		glUniform1i(textureShaderSampleLocation, GL_TEXTURENUM);
	}
}

void Deprecated_Shader::activateTextures()
{
	/** DEPRECATED: I made this earlier on and now handle textures differently. Leaving so my early files still work.*/
	for (size_t i = 0; i < textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + uint32_t(i));
		glBindTexture(GL_TEXTURE_2D, textures[i]->getTextureId());
	}
}

bool Deprecated_Shader::shaderCompileSuccess(GLuint shaderID)
{
	char infolog[256];
	int success = true;

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shaderID, 256, nullptr, infolog);
		std::cerr << "shader compile fail, infolog:\n" << infolog << std::endl;
	}

	return success != 0; //probably more efficient to just return success and allow implicit cast 
}

bool Deprecated_Shader::programLinkSuccess(GLuint programID)
{
	char infolog[256];
	int success = true;

	glGetProgramiv(programID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(programID, 256, nullptr, infolog);
		std::cerr << "OpenGL program link fail, infolog:\n" << infolog << std::endl;
	}

	return success != 0; //probably more efficient to just return success and allow implicit cast 
}

#endif //WITH_OLD_OPENGL3_PROJECT_BUILD
