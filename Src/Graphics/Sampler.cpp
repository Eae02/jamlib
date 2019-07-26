#include "Sampler.hpp"
#include "OpenGL.hpp"

#include <exception>

namespace jm
{
	TextureMinFilter MakeTextureMinFilter(Filter filter, std::optional<Filter> mipFilter)
	{
		switch (filter)
		{
		case Filter::Linear:
			if (!mipFilter.has_value())
				return TextureMinFilter::Linear;
			if (mipFilter.value() == Filter::Linear)
				return TextureMinFilter::LinearMipmapLinear;
			return TextureMinFilter::LinearMipmapNearest;
			
		case Filter::Nearest:
			if (!mipFilter.has_value())
				return TextureMinFilter::Nearest;
			if (mipFilter.value() == Filter::Linear)
				return TextureMinFilter::NearestMipmapLinear;
			return TextureMinFilter::NearestMipmapNearest;
		}
		
		std::terminate();
	}
	
	uint32_t detail::TranslateTextureWrapMode(TextureWrapMode wrapMode)
	{
		switch (wrapMode)
		{
		case TextureWrapMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
		case TextureWrapMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
		case TextureWrapMode::Repeat: return GL_REPEAT;
		case TextureWrapMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
		}
		std::abort();
	}
	
	uint32_t detail::TranslateTextureMinFilter(TextureMinFilter minFilter)
	{
		switch (minFilter)
		{
		case TextureMinFilter::Nearest: return GL_NEAREST;
		case TextureMinFilter::Linear: return GL_LINEAR;
		case TextureMinFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
		case TextureMinFilter::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
		case TextureMinFilter::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
		case TextureMinFilter::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
		}
		
		std::abort();
	}
	
	static uint32_t currentSampler[32];
	
	void BindSampler(const Sampler* sampler, uint32_t binding)
	{
		const uint32_t id = sampler ? sampler->UniqueID() : 0;
		if (currentSampler[binding] != id)
		{
			currentSampler[binding] = id;
			glBindSampler(binding, sampler ? sampler->Handle() : 0);
		}
	}
	
	static Sampler* pixel2DSampler = nullptr;
	
	const Sampler& Pixel2DSampler()
	{
		if (pixel2DSampler == nullptr)
		{
			pixel2DSampler = new Sampler;
			pixel2DSampler->SetMinFilter(TextureMinFilter::LinearMipmapLinear);
			pixel2DSampler->SetMagFilter(Filter::Nearest);
			pixel2DSampler->SetWrapU(TextureWrapMode::ClampToEdge);
			pixel2DSampler->SetWrapV(TextureWrapMode::ClampToEdge);
			pixel2DSampler->SetWrapW(TextureWrapMode::ClampToEdge);
		}
		return *pixel2DSampler;
	}
	
	void detail::DestroyGlobalSamplers()
	{
		delete pixel2DSampler;
	}
	
	static uint32_t nextUniqueID = 1;
	
	Sampler::Sampler()
		: m_uniqueID(nextUniqueID++)
	{
		GLuint sampler;
		glGenSamplers(1, &sampler);
		m_sampler = sampler;
	}
	
	void Sampler::SetMinFilter(TextureMinFilter minFilter)
	{
		glSamplerParameteri(m_sampler.Get(),  GL_TEXTURE_MIN_FILTER, detail::TranslateTextureMinFilter(minFilter));
	}
	
	void Sampler::SetMagFilter(TextureMagFilter magFilter)
	{
		glSamplerParameteri(m_sampler.Get(),  GL_TEXTURE_MAG_FILTER,
		                    magFilter == TextureMagFilter::Linear ? GL_LINEAR : GL_NEAREST);
	}
	
	void Sampler::SetMinLod(float minLod)
	{
		glSamplerParameterf(m_sampler.Get(), GL_TEXTURE_MIN_LOD, minLod);
	}
	
	void Sampler::SetMaxLod(float maxLod)
	{
		glSamplerParameterf(m_sampler.Get(), GL_TEXTURE_MAX_LOD, maxLod);
	}
	
	void Sampler::SetMaxAnistropy(float maxAnistropy)
	{
		if (detail::maxAnistropy != 0)
		{
			if (maxAnistropy > detail::maxAnistropy)
				maxAnistropy = detail::maxAnistropy;
			glSamplerParameterf(m_sampler.Get(), GL_TEXTURE_MAX_ANISOTROPY, maxAnistropy);
		}
	}
	
	void Sampler::SetWrapU(TextureWrapMode wrapMode)
	{
		glSamplerParameteri(m_sampler.Get(), GL_TEXTURE_WRAP_S, detail::TranslateTextureWrapMode(wrapMode));
	}
	
	void Sampler::SetWrapV(TextureWrapMode wrapMode)
	{
		glSamplerParameteri(m_sampler.Get(), GL_TEXTURE_WRAP_T, detail::TranslateTextureWrapMode(wrapMode));
	}
	
	void Sampler::SetWrapW(TextureWrapMode wrapMode)
	{
		glSamplerParameteri(m_sampler.Get(), GL_TEXTURE_WRAP_R, detail::TranslateTextureWrapMode(wrapMode));
	}
	
	void Sampler::SetBorderColor(const glm::vec4& color)
	{
		glSamplerParameterfv(m_sampler.Get(), GL_TEXTURE_BORDER_COLOR, reinterpret_cast<const float*>(&color));
	}
}
