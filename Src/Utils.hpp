#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cstring>
#include <cmath>
#include <string_view>
#include <glm/glm.hpp>

#include "API.hpp"

#define JM_BIT_FIELD(T) \
inline constexpr T operator|(T a, T b) noexcept \
{ return static_cast<T>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b)); }\
inline constexpr T operator&(T a, T b) noexcept \
{ return static_cast<T>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b)); }\
inline constexpr T& operator|=(T& a, T b) noexcept \
{ a = a | b; return a; }

namespace jm
{
	constexpr float PI = 3.141592653589793f;
	constexpr float TWO_PI = 6.283185307179586f;
	constexpr float HALF_PI = 1.5707963267948966f;
	
	/***
	 * Checks the the given bitfield has a specific flag set.
	 * @tparam T The type of the bitfield enum.
	 * @param bits The bitfield to check.
	 * @param flag The flag to check for.
	 * @return Whether the bitfield has the flag set.
	 */
	template <typename T>
	inline constexpr bool HasFlag(T bits, T flag)
	{
		return (int)(bits & flag) != 0;
	}
	
	/***
	 * Deleter for use with unique_ptr which calls std::free
	 */
	struct FreeDel
	{
		void operator()(void* mem) const noexcept
		{
			std::free(mem);
		}
	};
	
	inline int8_t FloatToSNorm(float x)
	{
		return (int8_t)(glm::clamp(x, -1.0f, 1.0f) * 127.0f);
	}
	
	inline int64_t NanoTime()
	{
		using namespace std::chrono;
		return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
	}
	
	template <typename T, typename U>
	constexpr inline auto RoundToNextMultiple(T value, U multiple)
	{
		auto valModMul = value % multiple;
		return valModMul == 0 ? value : (value + multiple - valModMul);
	}
	
	inline bool StringEqualCaseInsensitive(std::string_view a, std::string_view b)
	{
		return a.size() == b.size() && strncasecmp(a.data(), b.data(), a.size()) == 0;
	}
	
	/**
	 * Concatenates a list of string views into one string object.
	 * @param list The list of parts to concatenate
	 * @return The combined string
	 */
	JAPI std::string Concat(std::initializer_list<std::string_view> list);
	
	/**
	 * Removes whitespace from the start and end of the input string.
	 * @param input The string to remove whitespace from.
	 * @return Input without leading and trailing whitespace.
	 */
	JAPI std::string_view TrimString(std::string_view input);
	
	JAPI uint64_t HashFNV1a64(std::string_view s);
	JAPI uint32_t HashFNV1a32(std::string_view s);
	
	class CTStringHash
	{
	public:
		constexpr CTStringHash(uint32_t _hash = 0) noexcept
			: hash(_hash) { }
		
		constexpr CTStringHash(const char* const text) noexcept
			: hash(CalcHash(text, SIZE_MAX, FNV_OFFSET_BASIS)) { }
		
		constexpr CTStringHash(const std::string_view text) noexcept
			: hash(CalcHash(text.data(), text.size(), FNV_OFFSET_BASIS)) { }
		
		constexpr CTStringHash Append(CTStringHash other) const noexcept
		{
			return CTStringHash(other.hash + 0x9e3779b9 + (hash << 6) + (hash >> 2));
		}
		
		uint32_t hash;
		
	private:
		static constexpr uint32_t FNV_OFFSET_BASIS = 2166136261;
		static constexpr uint32_t FNV_PRIME = 16777619;
		
		static constexpr uint32_t CalcHash(const char* const str, const size_t len, const uint32_t value) noexcept
		{
			return (len == 0 || *str == '\0') ? value : CalcHash(str + 1, len - 1, (value ^ (uint32_t)str[0]) * FNV_PRIME);
		}
	};
	
	inline bool StringEndsWith(std::string_view string, std::string_view suffix)
	{
		return string.size() >= suffix.size() && string.compare(string.size() - suffix.size(), suffix.size(), suffix) == 0;
	}
	
	inline bool StringStartsWith(std::string_view string, std::string_view prefix)
	{
		return string.size() >= prefix.size() && string.compare(0, prefix.size(), prefix) == 0;
	}
	
	/**
	 * Invokes a callback for each parts of a string that is separated by a given delimiter. Empty parts are skipped.
	 * @tparam CallbackTp The callback type, signature should be void(std::string_view)
	 * @param string The string to loop through the parts of.
	 * @param delimiter The delimiter to use.
	 * @param callback The callback function.
	 */
	template <typename CallbackTp>
	void IterateStringParts(std::string_view string, char delimiter, CallbackTp callback)
	{
		for (size_t pos = 0; pos < string.size(); pos++)
		{
			const size_t end = string.find(delimiter, pos);
			if (end == pos)
				continue;
			
			const size_t partLen = end == std::string_view::npos ? std::string_view::npos : (end - pos);
			callback(string.substr(pos, partLen));
			
			if (end == std::string_view::npos)
				break;
			pos = end;
		}
	}
	
	JAPI void SplitString(std::string_view string, char delimiter, std::vector<std::string_view>& partsOut);
	
	template <typename CollectionTp, typename ItemTp>
	inline bool Contains(const CollectionTp& collection, const ItemTp& item)
	{
		for (const auto& i : collection)
		{
			if (i == item)
				return true;
		}
		return false;
	}
	
	inline int8_t ToSNorm(float x)
	{
		return (int8_t)glm::clamp((int)(x * 127), -127, 127);
	}
	
	JAPI bool DebugMode();
	
	JAPI void Panic(std::string message);
	
	JAPI std::string CanonicalPath(std::string_view path);
	JAPI std::string_view BaseName(std::string_view path);
	JAPI std::string_view PathWithoutExtension(std::string_view fileName);
	JAPI std::string_view PathExtension(std::string_view fileName); //Does not include the dot before the extension!
	JAPI std::string_view ParentPath(std::string_view path, bool includeSlash);
	
	JAPI std::vector<char> Base64Decode(std::string_view in);
}
