#include "Asset.hpp"
#include "Utils.hpp"

#include <unordered_map>

namespace jm
{
	std::vector<detail::AssetLoader> detail::assetLoaders;
	
	struct Asset
	{
		std::string name;
		void* assetMemory;
		bool loaded;
		size_t loaderIndex;
		std::function<std::vector<char>()> readCallback;
		
		bool operator<(const Asset& other) const
		{
			return name < other.name;
		}
		
		bool operator<(std::string_view bName) const
		{
			return name < bName;
		}
	};
	
	static std::vector<Asset> assets;
	
	static int64_t FindAssetLoader(std::string_view name)
	{
		size_t lastDot = name.rfind('.');
		if (lastDot == std::string_view::npos)
			return -1;
		std::string_view ext = name.substr(lastDot + 1);
		
		for (int64_t i = (int64_t)detail::assetLoaders.size() - 1; i >= 0; i--)
		{
			if (ext == detail::assetLoaders[i].extension)
				return i;
		}
		
		return -1;
	}
	
	static void ProcessAsset(std::string name, std::function<std::vector<char>()> readCallback)
	{
		int64_t loader = FindAssetLoader(name);
		if (loader == -1)
		{
			std::cout << "Warning: No asset loader found for '" << name << "'." << std::endl;
			return;
		}
		
		Asset& asset = assets.emplace_back();
		asset.name = std::move(name);
		asset.loaderIndex = loader;
		asset.readCallback = std::move(readCallback);
	}
	
	static std::unique_ptr<char[]> assetMemory;
	
	inline void LoadAsset(Asset& asset)
	{
		if (asset.loaded)
			return;
		
		std::vector<char> data = asset.readCallback();
		
		detail::assetLoaders[asset.loaderIndex].loadCallback(data, asset.name, asset.assetMemory);
		
		asset.readCallback = nullptr;
		asset.loaded = true;
	}
	
	void detail::LoadAssets()
	{
		if (!FindAssetsDir(ProcessAsset) && !FindAssetsZip(ProcessAsset))
		{
			return;
		}
		
		std::sort(assets.begin(), assets.end(), [&] (const Asset& a, const Asset& b)
		{
			return a.name < b.name;
		});
		
		std::vector<size_t> assetMemoryOffset(assets.size());
		size_t nextAssetMemoryOffset = 0;
		for (size_t i = 0; i < assets.size(); i++)
		{
			assetMemoryOffset[i] = RoundToNextMultiple(nextAssetMemoryOffset, alignof(std::max_align_t));
			nextAssetMemoryOffset = assetMemoryOffset[i] + assetLoaders[assets[i].loaderIndex].typeSize;
		}
		
		assetMemory = std::make_unique<char[]>(nextAssetMemoryOffset);
		
		for (size_t i = 0; i < assets.size(); i++)
		{
			assets[i].assetMemory = assetMemory.get() + assetMemoryOffset[i];
		}
		
		for (Asset& asset : assets)
		{
			LoadAsset(asset);
		}
	}
	
	void* detail::GetAsset(std::string_view name, std::type_index type)
	{
		auto it = std::lower_bound(assets.begin(), assets.end(), name);
		if (it == assets.end() || it->name != name)
		{
			Panic(Concat({ "Asset not found: '", name, "'." }));
		}
		LoadAsset(*it);
		if (assetLoaders[it->loaderIndex].typeIndex != type)
		{
			Panic(Concat({ "Attempted to get asset '", name, "' as an incorrect type." }));
		}
		return it->assetMemory;
	}
}
