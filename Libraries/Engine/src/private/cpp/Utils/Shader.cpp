#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"
#include "Utils/Shader.h"
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"

namespace Engine
{
	Shader::Shader(ShaderParams& in) : initParams(in)
	{

	}

	Shader::~Shader()
	{
		
	}

	void Shader::shaderCompileCheck(const char* debugShaderTypeName, /*GLuint*/uint32_t shader)
	{
		GLint success = 0;
		ec(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
		if (!success)
		{
			constexpr int size = 4096;
			char infolog[size];

			ec(glGetShaderInfoLog(shader, size, nullptr, infolog));
			std::cerr << "SHADER COMPILE ERROR: " << debugShaderTypeName << infolog;
			std::cerr << std::endl;
		}
	}

	void Shader::shaderLinkCheck()
	{
		GLint success = 0;
		ec(glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success));
		if (!success)
		{
			constexpr int size = 4096;
			char infolog[size];

			ec(glGetProgramInfoLog(shaderProgram, size, nullptr, infolog));
			std::cerr << "SHADER LINK ERROR: " << infolog;
			std::cerr << std::endl;
		}
	}

	void Shader::use()
	{
		ec(glUseProgram(shaderProgram));
	}

	void Shader::setMat4(const char* uniformName, const glm::mat4& matrix4)
	{
		setUniformMatrix4fv(uniformName, 1, GL_FALSE, glm::value_ptr(matrix4));
	}

	void Shader::setInt(const char* uniformName, int value)
	{
		setUniform1i(uniformName, value);
	}

	void Shader::setFloat(const char* uniformName, float value)
	{
		setUniform1f(uniformName, value);
	}

	void Shader::setUniform4f(const char* uniform, float red, float green, float blue, float alpha)
	{
		int uniformLocation = glGetUniformLocation(shaderProgram, uniform);

		//must be using the shader to update uniform value
		ec(glUseProgram(shaderProgram));
		ec(glUniform4f(uniformLocation, red, green, blue, alpha));
	}

	void Shader::setUniform4f(const char* uniform, const glm::vec4& values)
	{
		setUniform4f(uniform, values.r, values.g, values.b, values.a);
	}

	void Shader::setUniform3f(const char* uniform, float red, float green, float blue)
	{
		int uniformLocation = glGetUniformLocation(shaderProgram, uniform);
		ec(glUseProgram(shaderProgram));
		ec(glUniform3f(uniformLocation, red, green, blue));
	}

	void Shader::setUniform3f(const char* uniform, const glm::vec3& vals)
	{
		int uniformLocation = glGetUniformLocation(shaderProgram, uniform);
		ec(glUseProgram(shaderProgram));
		ec(glUniform3f(uniformLocation, vals.r, vals.g, vals.b));
	}

	void Shader::setUniform1i(const char* uniform, int newValue)
	{
		int uniformLocation = glGetUniformLocation(shaderProgram, uniform);
		ec(glUseProgram(shaderProgram));
		ec(glUniform1i(uniformLocation, newValue));
	}

	void Shader::setUniformMatrix4fv(const char* uniform, int numberMatrices, /*GLuint*/ uint8_t transpose, const float* data)
	{
		int uniformLocation = glGetUniformLocation(shaderProgram, uniform);
		glUseProgram(shaderProgram);
		glUniformMatrix4fv(uniformLocation, numberMatrices, GLboolean(transpose), data);
	}

	void Shader::setUniform1f(const char* uniformName, float value)
	{
		GLuint uniformLocationInShader = glGetUniformLocation(shaderProgram, uniformName);
		glUseProgram(shaderProgram);
		glUniform1f(uniformLocationInShader, value);
	}

	void Shader::onAcquireGPUResources()
	{
		const bool bIsPipeline = initParams.vertex_src.has_value() && initParams.fragment_src.has_value();
		bool bIsCompute = false;
#if ENABLE_MODERN_OPENGL_SHADERS
		bIsCompute = initParams.compute_src.has_value();
#endif //ENABLE_MODERN_OPENGL_SHADERS
		if ((bIsPipeline || bIsCompute) && !(bIsCompute && bIsPipeline))
		{
			if (bIsCompute)
			{
#if ENABLE_MODERN_OPENGL_SHADERS
				GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
				const char* compute_cstr = initParams.compute_src->c_str();
				ec(glShaderSource(computeShader, 1, &compute_cstr, nullptr));
				ec(glCompileShader(computeShader));
				shaderCompileCheck("compute shader", computeShader);

				shaderProgram = glCreateProgram();
				ec(glAttachShader(shaderProgram, computeShader));
				ec(glLinkProgram(shaderProgram));
				shaderLinkCheck(shaderProgram);

				ec(glDeleteShader(computeShader));
#endif //ENABLE_MODERN_OPENGL_SHADERS
			}
			else if (bIsPipeline)
			{
				GLuint vertexShader = 0, fragmentShader = 0;

				/////////////////////////////////////////////////////////////////////////////////////
				// create the individual shaders
				/////////////////////////////////////////////////////////////////////////////////////
				if (initParams.vertex_src.has_value())
				{
					vertexShader = glCreateShader(GL_VERTEX_SHADER);
					const char* vert_cstr = initParams.vertex_src->c_str();
					ec(glShaderSource(vertexShader, 1, &vert_cstr, nullptr));
					ec(glCompileShader(vertexShader));
					shaderCompileCheck("vertex shader", vertexShader);
				}
#if ENABLE_MODERN_OPENGL_SHADERS
				GLuint tessControlShader = 0, tessEvalShader = 0, geometryShader = 0;

				if (initParams.tessellation_control_src.has_value())
				{
					tessControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
					const char* tcs_cstr = initParams.tessellation_control_src->c_str();
					glShaderSource(tessControlShader, 1, &tcs_cstr, nullptr);
					glCompileShader(tessControlShader);
					shaderCompileCheck("tessellation control shader", tessControlShader);
				}
				if (initParams.tessellation_evaluation_src.has_value())
				{
					tessEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
					const char* tes_cstr = initParams.tessellation_evaluation_src->c_str();
					glShaderSource(tessEvalShader, 1, &tes_cstr, nullptr);
					glCompileShader(tessEvalShader);
					shaderCompileCheck("tessellation evaluation shader", tessEvalShader);
				}
				if (initParams.geometry_src.has_value())
				{
					geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
					const char* geo_cstr = initParams.geometry_src->c_str();
					glShaderSource(geometryShader, 1, &geo_cstr, nullptr);
					glCompileShader(geometryShader);
					shaderCompileCheck("geometry shader", geometryShader);
				}
#endif //ENABLE_MODERN_OPENGL_SHADERS
				if (initParams.fragment_src.has_value())
				{
					fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
					const char* frag_cstr = initParams.fragment_src->c_str();
					ec(glShaderSource(fragmentShader, 1, &frag_cstr, nullptr));
					ec(glCompileShader(fragmentShader));
					shaderCompileCheck("fragment shader", fragmentShader);
				}

				/////////////////////////////////////////////////////////////////////////////////////
				// link the shaders into a program
				/////////////////////////////////////////////////////////////////////////////////////
				shaderProgram = glCreateProgram();
				if (vertexShader) { ec(glAttachShader(shaderProgram, vertexShader)); }
#if ENABLE_MODERN_OPENGL_SHADERS
				if (tessControlShader) { ec(glAttachShader(shaderProgram, tessControlShader)); }
				if (tessEvalShader) { ec(glAttachShader(shaderProgram, tessEvalShader)); }
				if (geometryShader) { ec(glAttachShader(shaderProgram, geometryShader)); }
#endif //ENABLE_MODERN_OPENGL_SHADERS
				if (fragmentShader) { ec(glAttachShader(shaderProgram, fragmentShader)); }
				ec(glLinkProgram(shaderProgram));
				shaderLinkCheck();

				//CLEAN UP
				ec(glDeleteShader(vertexShader));
#if ENABLE_MODERN_OPENGL_SHADERS
				glDeleteShader(tessControlShader);
				glDeleteShader(tessEvalShader);
				glDeleteShader(geometryShader);
#endif //ENABLE_MODERN_OPENGL_SHADERS
				ec(glDeleteShader(fragmentShader));
			}
		}
	}

	void Shader::onReleaseGPUResources()
	{
		ec(glDeleteProgram(shaderProgram));
	}

}