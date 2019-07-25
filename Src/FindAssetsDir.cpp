#include "Asset.hpp"

#include <cstring>
#include <string>

#ifdef __linux__

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include <gsl/gsl>
#include <fstream>

namespace jm::detail
{
	static bool FindAssetsRec(const std::string& name, size_t prefixLen, ProcessAssetCB processAsset)
	{
		DIR* dir = opendir(name.c_str());
		if (dir == nullptr)
			return false;
		
		auto _f1 = gsl::finally([&] { closedir(dir); });
		
		while (dirent* entry = readdir(dir))
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			
			std::string fullPath = name + "/" + entry->d_name;
			if (entry->d_type == DT_DIR)
			{
				if (!FindAssetsRec(fullPath, prefixLen, processAsset))
					return false;
			}
			else if (entry->d_type == DT_REG)
			{
				std::string assetName = fullPath.substr(prefixLen);
				processAsset(std::move(assetName), [fullPath=std::move(fullPath)] () -> std::vector<char>
				{
					std::ifstream stream(fullPath, std::ios::binary);
					if (!stream)
						return { };
					
					std::vector<char> result;
					
					char data[4096];
					while (!stream.eof())
					{
						stream.read(data, sizeof(data));
						result.insert(result.end(), data, data + stream.gcount());
					}
					
					return result;
				});
			}
		}
		
		return true;
	}
	
	bool FindAssetsDir(ProcessAssetCB processAsset)
	{
		std::string path = "./Assets";
		return FindAssetsRec(path, path.size() + 1, processAsset);
	}
}

#else

namespace jm::detail
{
	bool FindAssetsDir(ProcessAssetCB processAsset)
	{
		return false;
	}
}

#endif
