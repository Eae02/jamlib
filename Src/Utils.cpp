#include "Utils.hpp"

#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stack>
#include <SDL_messagebox.h>

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
	
	std::string_view BaseName(std::string_view path)
	{
		const size_t lastSlash = path.rfind('/');
		if (lastSlash == std::string_view::npos)
			return path;
		return path.substr(lastSlash + 1);
	}
	
	std::string_view PathWithoutExtension(std::string_view fileName)
	{
		const size_t lastDot = fileName.rfind('.');
		if (lastDot == std::string_view::npos)
			return fileName;
		return fileName.substr(0, lastDot);
	}
	
	std::string_view PathExtension(std::string_view fileName)
	{
		const size_t lastDot = fileName.rfind('.');
		if (lastDot == std::string_view::npos)
			return { };
		return fileName.substr(lastDot + 1);
	}
	
	std::string_view ParentPath(std::string_view path, bool includeSlash)
	{
		const size_t lastSlash = path.rfind('/');
		if (lastSlash == std::string_view::npos)
			return { };
		return path.substr(0, lastSlash + (includeSlash ? 1 : 0));
	}
	
	extern bool debugMode;
	
	bool DebugMode()
	{
		return debugMode;
	}
	
	void Panic(std::string message)
	{
		if (debugMode)
		{
			std::cerr << "Fatal error: " << message << "\n";
			std::abort();
		}
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", message.c_str(), nullptr);
		std::exit(1);
	}
	
	static const char* Base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	
	std::vector<char> Base64Decode(std::string_view in)
	{
		std::vector<char> out;
		
		std::vector<int> translate(256, -1);
		for (int i = 0; i < 64; i++)
		{
			translate[Base64Chars[i]] = i;
		}
		
		int val = 0;
		int valb = -8;
		for (char c : in)
		{
			if (translate[c] == -1)
				break;
			val = (val << 6) + translate[c];
			valb += 6;
			if (valb >= 0)
			{
				out.push_back(char((val >> valb) & 0xFF));
				valb -= 8;
			}
		}
		return out;
	}
}
