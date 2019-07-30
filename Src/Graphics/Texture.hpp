#pragma once

#include "GLHandle.hpp"
#include "Format.hpp"
#include "Sampler.hpp"
#include "../API.hpp"

#include <optional>
#include <cmath>
#include <vector>
#include <any>
#include <gsl/span>

namespace jm
{
	class JAPI Texture
	{
	public:
		void SetMinFilter(TextureMinFilter minFilter);
		void SetMagFilter(TextureMagFilter magFilter);
		
		void SetMinLod(float minLod);
		void SetMaxLod(float maxLod);
		
		void SetMaxAnistropy(float maxAnistropy);
		
		void SetWrapU(TextureWrapMode wrapMode);
		void SetWrapV(TextureWrapMode wrapMode);
		void SetWrapW(TextureWrapMode wrapMode);
		
		void GenerateMipmaps();
		
		inline Format GetFormat() const
		{ return m_format; }
		inline uint32_t MipLevels() const
		{ return m_mipLevels; }
		inline uint32_t Handle() const
		{ return m_handle.Get(); }
		inline uint32_t UniqueID() const
		{ return m_uniqueId; }
		
		void Bind(int unit) const;
		
		static inline uint32_t CalculateMipLevels(uint32_t minResolution)
		{
#if defined(__GNUC__)
			return 31 - __builtin_clz(minResolution);
#else
			return std::log2((double)minResolution);
#endif
		}
		
	protected:
		Texture(uint32_t target, Format format, uint32_t mipLevels);
		
	private:
		inline void SetParameterI(uint32_t parameterName, int value);
		inline void SetParameterF(uint32_t parameterName, float value);
		
		detail::GLHandle<detail::GLObjectTypes::Texture> m_handle;
		uint32_t m_target;
		uint32_t m_uniqueId;
		Format m_format;
		uint32_t m_mipLevels;
	};
	
	class JAPI Texture2D : public Texture
	{
	public:
		enum LoadFlags
		{
			LOAD_NO_MIPMAPS = 0x1,
			LOAD_SRGB = 0x2,
			LOAD_GRAYSCALE = 0x4
		};
		
		Texture2D(uint32_t width, uint32_t height, Format format, uint32_t mipLevels = 0);
		
		void SetData(uint32_t level, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
		             DataType dataType, uint32_t dataChannels, const void* data);
		
		static Texture2D Load(gsl::span<const char> fileData, LoadFlags flags);
		
		static void RegisterAssetLoader();
		
		uint32_t Width() const { return m_width; }
		uint32_t Height() const { return m_height; }
		
	private:
		friend void SetRenderTarget(Texture2D* color, Texture2D* depth);
		
		struct Framebuffer
		{
			uint32_t depthUID;
			detail::GLHandle<detail::GLObjectTypes::Framebuffer> framebuffer;
		};
		
		std::vector<Framebuffer> m_framebuffers;
		
		uint32_t m_width;
		uint32_t m_height;
	};
	
	JAPI void UpdateFullscreenTexture(std::optional<Texture2D>& texture, Format format);
}
