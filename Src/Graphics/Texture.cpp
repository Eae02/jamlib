#include "Texture.hpp"
#include "OpenGL.hpp"
#include "Sampler.hpp"
#include "../Asset.hpp"

#include <stb_image.h>
#include <iostream>

namespace jm
{
	static uint32_t nextTextureId = 1;
	static int currentTextureUnit = 0;
	static uint32_t currentBoundTexture[32];
	
	Texture::Texture(uint32_t target, Format format, uint32_t mipLevels)
		: m_target(target), m_uniqueId(nextTextureId++), m_format(format), m_mipLevels(mipLevels)
	{
		GLuint texture;
		JM_MODERN_GL(
			glCreateTextures(target, 1, &texture);
			,
			glGenTextures(1, &texture);
		)
		m_handle = texture;
	}
	
	void Texture::SetMinFilter(TextureMinFilter minFilter)
	{
		SetParameterI(GL_TEXTURE_MIN_FILTER, detail::TranslateTextureMinFilter(minFilter));
	}
	
	void Texture::SetMagFilter(TextureMagFilter magFilter)
	{
		SetParameterI(GL_TEXTURE_MAG_FILTER, magFilter == TextureMagFilter::Linear ? GL_LINEAR : GL_NEAREST);
	}
	
	void Texture::SetMinLod(float minLod)
	{
		SetParameterF(GL_TEXTURE_MIN_LOD, minLod);
	}
	
	void Texture::SetMaxLod(float maxLod)
	{
		SetParameterF(GL_TEXTURE_MAX_LOD, maxLod);
	}
	
	void Texture::SetMaxAnistropy(float maxAnistropy)
	{
		SetParameterF(GL_TEXTURE_MAX_ANISOTROPY, maxAnistropy);
	}
	
	void Texture::SetWrapU(TextureWrapMode wrapMode)
	{
		SetParameterI(GL_TEXTURE_WRAP_S, detail::TranslateTextureWrapMode(wrapMode));
	}
	
	void Texture::SetWrapV(TextureWrapMode wrapMode)
	{
		SetParameterI(GL_TEXTURE_WRAP_T, detail::TranslateTextureWrapMode(wrapMode));
	}
	
	void Texture::SetWrapW(TextureWrapMode wrapMode)
	{
		SetParameterI(GL_TEXTURE_WRAP_R, detail::TranslateTextureWrapMode(wrapMode));
	}
	
	void Texture::SetParameterI(uint32_t parameterName, int value)
	{
		JM_MODERN_GL(
			glTextureParameteri(Handle(), parameterName, value);
			,
			Bind(-1);
			glTexParameteri(m_target, parameterName, value);
		)
	}
	
	void Texture::SetParameterF(uint32_t parameterName, float value)
	{
		JM_MODERN_GL(
			glTextureParameterf(Handle(), parameterName, value);
			,
			Bind(-1);
			glTexParameterf(m_target, parameterName, value);
		)
	}
	
	void Texture::Bind(int unit) const
	{
		JM_MODERN_GL(
			if (unit == -1 && currentBoundTexture[0] != m_uniqueId)
			{
				glBindTexture(m_target, m_handle.Get());
				unit = 0;
			}
			else
			{
				glBindTextureUnit(unit, Handle());
			}
			,
			if (unit == -1)
				unit = currentTextureUnit;
			if (currentBoundTexture[unit] != m_uniqueId)
			{
				if (unit != currentTextureUnit)
				{
					glActiveTexture(GL_TEXTURE0 + unit);
					currentTextureUnit = unit;
				}
				glBindTexture(m_target, m_handle.Get());
			}
		)
		
		currentBoundTexture[unit] = m_uniqueId;
	}
	
	void Texture::GenerateMipmaps()
	{
		JM_MODERN_GL(
			glGenerateTextureMipmap(Handle());
			,
			Bind(-1);
			glGenerateMipmap(m_target);
		)
	}
	
	Texture2D::Texture2D(uint32_t width, uint32_t height, Format format, uint32_t mipLevels)
		: Texture(GL_TEXTURE_2D, format, mipLevels ? mipLevels : CalculateMipLevels(std::min(width, height))),
		  m_width(width), m_height(height)
	{
		JM_MODERN_GL(
			glTextureStorage2D(Handle(), MipLevels(), detail::GetGLFormat(format), width, height);
			,
			Bind(-1);
			glTexStorage2D(GL_TEXTURE_2D, MipLevels(), detail::GetGLFormat(format), width, height);
		)
	}
	
	void Texture2D::SetData(uint32_t level, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
	                        DataType dataType, uint32_t dataChannels, const void* data)
	{
		const GLenum format = detail::GetGLTextureFormat(dataType, dataChannels);
		const GLenum type = detail::GetGLDataType(dataType);
		
		JM_MODERN_GL(
			glTextureSubImage2D(Handle(), level, xOffset, yOffset, width, height, format, type, data);
			,
			Bind(-1);
			glTexSubImage2D(GL_TEXTURE_2D, level, xOffset, yOffset, width, height, format, type, data);
		)
	}
	
	Texture2D Texture2D::Load(gsl::span<const char> fileData, LoadFlags flags)
	{
		bool grayscale = flags & LOAD_GRAYSCALE;
		
		int width, height, channels;
		stbi_uc* imageData = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(fileData.data()), fileData.size(),
			&width, &height, &channels, grayscale ? 1 : 4);
		
		if (imageData == nullptr)
		{
			std::cerr << "Failed to load image: " << stbi_failure_reason() << std::endl;
			std::abort();
		}
		
		Format format = Format::RGBA8_UNorm;
		if (grayscale)
			format = Format::R8_UNorm;
		else if (flags & LOAD_SRGB)
			format = Format::RGBA8_sRGB;
		
		Texture2D texture(width, height, format, (flags & LOAD_NO_MIPMAPS) ? 1 : 0);
		
		texture.SetData(0, 0, 0, width, height, DataType::UInt8Norm, channels, imageData);
		
		if (!(flags & LOAD_NO_MIPMAPS))
		{
			texture.GenerateMipmaps();
		}
		
		stbi_image_free(imageData);
		
		return texture;
	}
	
	void Texture2D::RegisterAssetLoader()
	{
		stbi_set_flip_vertically_on_load(true);
		
		auto assetLoader = [] (gsl::span<const char> fileData) -> Texture2D
		{
			return Texture2D::Load(fileData, (LoadFlags)0);
		};
		
		jm::RegisterAssetLoader<Texture2D>("png", assetLoader);
		jm::RegisterAssetLoader<Texture2D>("jpg", assetLoader);
		jm::RegisterAssetLoader<Texture2D>("jpeg", assetLoader);
		jm::RegisterAssetLoader<Texture2D>("tga", assetLoader);
	}
}
