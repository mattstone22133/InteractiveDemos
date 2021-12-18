#include <stb_image.h>

#include "EngineSystems/RenderSystem/Rendering/TextureWrapper.h"
#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"
#include <stdexcept>



namespace Engine
{
	static bool isPowerOf2(int value)
	{
		//powers of 2 should only occupy a single bit; use bitwise operators to ensure this is true
		return (value & (value - 1)) == 0;
	}

	TextureWrapper::TextureWrapper(const TextureInitParams& init) :
		params(init)
	{

	}

	TextureWrapper::~TextureWrapper()
	{

	}

	void TextureWrapper::onAcquireGPUResources()
	{

		int img_width, img_height, img_nrChannels;
		unsigned char* textureData = stbi_load(params.exe_relative_filepath, &img_width, &img_height, &img_nrChannels, 0);
		if (!textureData)
		{
			std::cerr << "failed to load texture: " << params.exe_relative_filepath << std::endl;
			return;
		}


		ec(glGenTextures(1, &glTextureId));

		if (params.texture_unit >= 0)
		{
			ec(glActiveTexture(params.texture_unit));
		}
		ec(glBindTexture(GL_TEXTURE_2D, glTextureId));

		int mode = -1;
		int dataFormat = -1;
		if (img_nrChannels == 3)
		{
#if MODERN_OPENGL
			mode = params.useGammaCorrection ? GL_SRGB : GL_RGB;
#else
			mode = GL_RGB;
#endif //MODERN_OPENGL
			dataFormat = GL_RGB;
		}
		else if (img_nrChannels == 4)
		{
#if MODERN_OPENGL
			mode = params.useGammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
#else
			mode = GL_RGBA;
#endif //MODERN_OPENGL
			dataFormat = GL_RGBA;
		}
		else if (img_nrChannels == 1)
		{
#if MODERN_OPENGL
			mode = GL_RED;
			dataFormat = GL_RED;
#else
			throw std::runtime_error("single channel image provided, but GL_RED is not implemented in GLES2");
#endif //MODERN_OPENGL
		}
		else
		{
			std::cerr << "unsupported image format for texture at " << params.exe_relative_filepath << " there are " << img_nrChannels << "channels" << std::endl;
		}

		ec(glTexImage2D(GL_TEXTURE_2D, 0, mode, img_width, img_height, 0, dataFormat, GL_UNSIGNED_BYTE, textureData));
#if MODERN_OPENGL
		if (!params.bGenerateMips)
		{
			ec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
			ec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
		}
#else
#endif //MODERN_OPENGL
		ec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		ec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		ec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		ec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		if (params.bGenerateMips && isPowerOf2(img_height) && isPowerOf2(img_width))
		{
			ec(glGenerateMipmap(GL_TEXTURE_2D));
		}
		stbi_image_free(textureData);
	}

	void TextureWrapper::onReleaseGPUResources()
	{
		if (glIsTexture(glTextureId))
		{
			ec(glDeleteTextures(1, &glTextureId));
		}
	}

}