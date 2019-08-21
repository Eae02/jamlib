#include "SaveFile.hpp"
#include "Utils.hpp"

#include <vector>
#include <fstream>
#include <yaml-cpp/yaml.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <SDL_filesystem.h>
#endif

namespace jm::save
{
	static std::vector<std::pair<std::string, std::string>> savedValues;
	
	static std::string savePath;
	
	bool clearSave;
	
	void Init(const char* org, const char* app)
	{
#ifdef __EMSCRIPTEN__
		savePath = Concat({ "/jam/", org, "-", app, ".yaml" });
#else
		char* prefPath = SDL_GetPrefPath(org, app);
		savePath = Concat({ prefPath, "settings.yaml" });
		SDL_free(prefPath);
#endif
		
		if (clearSave)
			return;
		
		std::ifstream stream(savePath, std::ios::binary);
		if (!stream)
			return;
		
		YAML::Node root = YAML::Load(stream);
		for (auto it = root.begin(); it != root.end(); ++it)
		{
			savedValues.emplace_back(it->first.as<std::string>(), it->second.as<std::string>());
		}
	}
	
	void Write()
	{
		if (savePath.empty())
			return;
		
		YAML::Node node;
		for (const auto& val : savedValues)
		{
			node[val.first] = val.second;
		}
		std::ofstream stream(savePath, std::ios::binary);
		stream << node;
		stream.close();
		
#ifdef __EMSCRIPTEN__
		EM_ASM(FS.syncfs(function(){}););
#endif
	}
	
	const std::string* GetValue(std::string_view name)
	{
		for (auto& val : savedValues)
		{
			if (val.first == name)
			{
				return &val.second;
			}
		}
		return nullptr;
	}
	
	void SetValue(std::string_view name, std::string value)
	{
		bool written = false;
		for (auto& val : savedValues)
		{
			if (val.first == name)
			{
				val.second = std::move(value);
				written = true;
				break;
			}
		}
		
		if (!written)
		{
			savedValues.emplace_back(name, std::move(value));
		}
		
		Write();
	}
}
