#include "EngineSystems/RenderSystem/Rendering/TextureWrapper.h"
#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"

#define TEMP_REMOVE_STBI_IMAGE_CODE 1
//#include <stb_image.h>


namespace Engine
{
	TextureWrapper::TextureWrapper(const TextureInitParams& init) :
		params(init)
	{

	}

	TextureWrapper::~TextureWrapper()
	{

	}

	void TextureWrapper::onAcquireGPUResources()
	{

		//throw std::runtime_error("STBI_IMAGE_NEEDS_INCLUDING_AS_DEPENDENCY!");
		std::cerr << "STBI IMAGE NEEDS INCLUDING AS DEPENDENCY! DISABLED FOR PORT!" << std::endl;

#if !TEMP_REMOVE_STBI_IMAGE_CODE 

		int img_width, img_height, img_nrChannels;
		unsigned char* textureData = stbi_load(init.exe_relative_filepath, &img_width, &img_height, &img_nrChannels, 0);
		if (!textureData)
		{
			std::cerr << "failed to load texture: " << init.exe_relative_filepath << std::endl;
			return;
		}

		ec(glGenTextures(1, &glTextureId));

		if (init.texture_unit >= 0)
		{
			ec(glActiveTexture(init.texture_unit));
		}
		ec(glBindTexture(GL_TEXTURE_2D, glTextureId));

		int mode = -1;
		int dataFormat = -1;
		if (img_nrChannels == 3)
		{
			mode = init.useGammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (img_nrChannels == 4)
		{
			mode = init.useGammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}
		else if (img_nrChannels == 1)
		{
			mode = GL_RED;
			dataFormat = GL_RED;
		}
		else
		{
			std::cerr << "unsupported image format for texture at " << init.exe_relative_filepath << " there are " << img_nrChannels << "channels" << std::endl;
		}

		ec(glTexImage2D(GL_TEXTURE_2D, 0, mode, img_width, img_height, 0, dataFormat, GL_UNSIGNED_BYTE, textureData));
		if (!init.bGenerateMips)
		{
			ec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
			ec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
		}
		ec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		ec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		ec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		ec(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		ec(glGenerateMipmap(GL_TEXTURE_2D));
		stbi_image_free(textureData);
#else
#endif //TEMP_REMOVE_STBI_IMAGE_CODE 
	}

	void TextureWrapper::onReleaseGPUResources()
	{
		if (glIsTexture(glTextureId))
		{
			ec(glDeleteTextures(1, &glTextureId));
		}
	}

}