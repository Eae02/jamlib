#pragma once

#include "API.hpp"

#include <string>

namespace jm::save
{
	JAPI void Init(const char* org, const char* app);
	
	JAPI void Write();
	
	JAPI const std::string* GetValue(std::string_view name);
	
	JAPI void SetValue(std::string_view name, std::string value);
	
	inline const std::string& GetValue(std::string_view name, const std::string& def)
	{
		if (const std::string* val = GetValue(name))
			return *val;
		return def;
	}
}
