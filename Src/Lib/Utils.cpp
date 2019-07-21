#include "Utils.hpp"

#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stack>

namespace jm
{
	std::string_view TrimString(std::string_view input)
	{
		if (input.empty())
			return input;
		
		size_t startWhitespace = 0;
		while (std::isspace(input[startWhitespace]))
		{
			startWhitespace++;
			if (startWhitespace >= input.size())
				return { };
		}
		
		size_t endWhitespace = input.size() - 1;
		while (std::isspace(input[endWhitespace]))
		{
			endWhitespace--;
		}
		
		return input.substr(startWhitespace, (endWhitespace + 1) - startWhitespace);
	}
	
	std::string Concat(std::initializer_list<std::string_view> list)
	{
		size_t size = 0;
		for (std::string_view entry : list)
			size += entry.size();
		
		std::string result(size, ' ');
		
		char* output = result.data();
		for (std::string_view entry : list)
		{
			std::copy(entry.begin(), entry.end(), output);
			output += entry.size();
		}
		
		return result;
	}
	
	void SplitString(std::string_view string, char delimiter, std::vector<std::string_view>& partsOut)
	{
		IterateStringParts(string, delimiter, [&] (std::string_view part)
		{
			partsOut.push_back(part);
		});
	}
	
	uint64_t HashFNV1a64(std::string_view s)
	{
		constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ull;
		constexpr uint64_t FNV_PRIME = 1099511628211ull;
		
		uint64_t h = FNV_OFFSET_BASIS;
		for (char c : s)
		{
			h ^= static_cast<uint8_t>(c);
			h *= FNV_PRIME;
		}
		return h;
	}
	
	uint32_t HashFNV1a32(std::string_view s)
	{
		constexpr uint32_t FNV_OFFSET_BASIS = 2166136261;
		constexpr uint32_t FNV_PRIME = 16777619;
		
		uint32_t h = FNV_OFFSET_BASIS;
		for (char c : s)
		{
			h ^= static_cast<uint8_t>(c);
			h *= FNV_PRIME;
		}
		return h;
	}
	
	std::string CanonicalPath(std::string_view path)
	{
		std::vector<std::string_view> parts;
		IterateStringParts(path, '/', [&] (std::string_view part)
		{
			if (part == ".." && !parts.empty())
			{
				parts.pop_back();
			}
			else if (part != ".")
			{
				parts.push_back(part);
			}
		});
		
		if (parts.empty())
			return {};
		
		std::ostringstream outStream;
		outStream << parts[0];
		for (size_t i = 1; i < parts.size(); i++)
			outStream << "/" << parts[i];
		return outStream.str();
	}
}
