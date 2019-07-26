#pragma once

#include "GLHandle.hpp"
#include "../API.hpp"

#include <optional>
#include <string_view>
#include <glm/glm.hpp>

namespace jm
{
	enum class SwizzleMode
	{
		R,
		G,
		B,
		A,
		Zero,
		One
	};
	
	enum class Filter
	{
		Linear,
		Nearest
	};
	
	enum class TextureMinFilter
	{
		Nearest,
		Linear,
		NearestMipmapNearest,
		LinearMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapLinear
	};
	
	using TextureMagFilter = Filter;
	
	JAPI TextureMinFilter MakeTextureMinFilter(Filter filter, std::optional<Filter> mipFilter);
	
	enum class TextureWrapMode
	{
		ClampToEdge,
		MirroredRepeat,
		Repeat,
		ClampToBorder
	};
	
	namespace detail
	{
		uint32_t TranslateTextureWrapMode(TextureWrapMode wrapMode);
		uint32_t TranslateTextureMinFilter(TextureMinFilter minFilter);
		void DestroyGlobalSamplers();
	}
	
	class JAPI Sampler
	{
	public:
		Sampler();
		
		void SetMinFilter(TextureMinFilter minFilter);
		void SetMagFilter(TextureMagFilter magFilter);
		
		void SetMinLod(float minLod);
		void SetMaxLod(float maxLod);
		
		void SetMaxAnistropy(float maxAnistropy);
		
		void SetWrapU(TextureWrapMode wrapMode);
		void SetWrapV(TextureWrapMode wrapMode);
		void SetWrapW(TextureWrapMode wrapMode);
		
		void SetBorderColor(const glm::vec4& color);
		
		uint32_t UniqueID() const
		{
			return m_uniqueID;
		}
		
		uint32_t Handle() const
		{
			return m_sampler.Get();
		}
		
	private:
		uint32_t m_uniqueID;
		detail::GLHandle<detail::GLObjectTypes::Sampler> m_sampler;
	};
	
	JAPI const Sampler& Pixel2DSampler();
	
	JAPI void BindSampler(const Sampler* sampler, uint32_t binding);
}
