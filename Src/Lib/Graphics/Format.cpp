#include "Format.hpp"
#include "OpenGL.hpp"

#include <cstdlib>

namespace jm::detail
{
	uint32_t GetGLFormat(Format format)
	{
		switch (format)
		{
		case Format::R8_UNorm: return GL_R8;
		case Format::R8_SNorm: return GL_R8_SNORM;
		case Format::R8_UInt: return GL_R8UI;
		case Format::R8_SInt: return GL_R8I;
		case Format::R16_UInt: return GL_R16UI;
		case Format::R16_SInt: return GL_R16I;
		case Format::R16_Float: return GL_R16F;
		case Format::R32_UInt: return GL_R32UI;
		case Format::R32_SInt: return GL_R32I;
		case Format::R32_Float: return GL_R32F;
		case Format::RG8_UNorm: return GL_RG8;
		case Format::RG8_SNorm: return GL_RG8_SNORM;
		case Format::RG8_UInt: return GL_RG8UI;
		case Format::RG8_SInt: return GL_RG8I;
		case Format::RG16_UInt: return GL_RG16UI;
		case Format::RG16_SInt: return GL_RG16I;
		case Format::RG16_Float: return GL_RG16F;
		case Format::RG32_UInt: return GL_RG32UI;
		case Format::RG32_SInt: return GL_RG32I;
		case Format::RG32_Float: return GL_RG32F;
		case Format::RGBA8_UNorm: return GL_RGBA8;
		case Format::RGBA8_SNorm: return GL_RGBA8_SNORM;
		case Format::RGBA8_UInt: return GL_RGBA8UI;
		case Format::RGBA8_SInt: return GL_RGBA8I;
		case Format::RGBA8_sRGB: return GL_SRGB8_ALPHA8;
		case Format::RGBA16_UInt: return GL_RGBA16UI;
		case Format::RGBA16_SInt: return GL_RGBA16I;
		case Format::RGBA16_Float: return GL_RGBA16F;
		case Format::RGBA32_UInt: return GL_RGBA32UI;
		case Format::RGBA32_SInt: return GL_RGBA32I;
		case Format::RGBA32_Float: return GL_RGBA32F;
		case Format::Depth32: return GL_DEPTH_COMPONENT32F;
		case Format::Depth24: return GL_DEPTH_COMPONENT24;
		case Format::Depth16: return GL_DEPTH_COMPONENT16;
		default: std::abort();
		}
	}
	
	uint32_t GetGLDataType(DataType type)
	{
		switch (type)
		{
		case DataType::Float32:
			return GL_FLOAT;
		case DataType::UInt8:
		case DataType::UInt8Norm:
			return GL_UNSIGNED_BYTE;
		case DataType::UInt16:
		case DataType::UInt16Norm:
			return GL_UNSIGNED_SHORT;
		case DataType::UInt32:
		case DataType::UInt32Norm:
			return GL_UNSIGNED_INT;
		case DataType::SInt8:
		case DataType::SInt8Norm:
			return GL_BYTE;
		case DataType::SInt16:
		case DataType::SInt16Norm:
			return GL_SHORT;
		case DataType::SInt32:
		case DataType::SInt32Norm:
			return GL_INT;
		default:
			std::abort();
		}
	}
	
	uint32_t GetGLTextureFormat(DataType type, uint32_t numChannels)
	{
		const GLenum normFormats[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
		const GLenum integerFormats[] = { GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_RGBA_INTEGER };
		
		if (type != DataType::Float32 &&
			type != DataType::UInt8Norm && type != DataType::UInt16Norm && type != DataType::UInt32Norm &&
			type != DataType::SInt8Norm && type != DataType::SInt16Norm && type != DataType::SInt32Norm)
		{
			return integerFormats[numChannels - 1];
		}
		else
		{
			return normFormats[numChannels - 1];
		}
	}
}
