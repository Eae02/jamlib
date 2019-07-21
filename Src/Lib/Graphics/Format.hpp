#pragma once

#include <cstdint>

namespace jm
{
	enum class Format
	{
		R8_UNorm,
		R8_SNorm,
		R8_UInt,
		R8_SInt,
		R16_UInt,
		R16_SInt,
		R16_Float,
		R32_UInt,
		R32_SInt,
		R32_Float,
		
		RG8_UNorm,
		RG8_SNorm,
		RG8_UInt,
		RG8_SInt,
		RG16_UInt,
		RG16_SInt,
		RG16_Float,
		RG32_UInt,
		RG32_SInt,
		RG32_Float,
		
		RGBA8_UNorm,
		RGBA8_SNorm,
		RGBA8_UInt,
		RGBA8_SInt,
		RGBA8_sRGB,
		RGBA16_UInt,
		RGBA16_SInt,
		RGBA16_Float,
		RGBA32_UInt,
		RGBA32_SInt,
		RGBA32_Float,
		
		Depth32,
		Depth24,
		Depth16
	};
	
	enum class DataType
	{
		Float32,
		UInt8Norm,
		UInt16Norm,
		UInt32Norm,
		SInt8Norm,
		SInt16Norm,
		SInt32Norm,
		UInt8,
		UInt16,
		UInt32,
		SInt8,
		SInt16,
		SInt32
	};
	
	namespace detail
	{
		uint32_t GetGLFormat(Format format);
		
		uint32_t GetGLDataType(DataType type);
		
		//Translates a data type and channel count to a texture format for use when uploading texture data.
		uint32_t GetGLTextureFormat(DataType type, uint32_t numChannels);
	}
}
