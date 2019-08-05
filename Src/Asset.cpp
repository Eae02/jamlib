#include "Asset.hpp"
#include "Utils.hpp"

#include <unordered_map>

#ifdef __linux__
#include <sys/stat.h>
#endif

namespace jm
{
	std::vector<detail::AssetLoader> detail::assetLoaders;
	
	struct Asset
	{
		std::string name;
		std::string source;
		void* assetMemory;
		bool loaded;
		size_t loaderIndex;
		std::function<std::vector<char>()> readCallback;
		std::chrono::system_clock::time_point loadTime;
		std::vector<std::function<void(void*)>> initCallbacks;
		
		void Unload()
		{
			if (loaded)
			{
				detail::assetLoaders[loaderIndex].destructor(assetMemory);
				loaded = false;
			}
		}
		
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
	
	static void ProcessAsset(std::string name, std::string source, std::function<std::vector<char>()> readCallback)
	{
		int64_t loader = FindAssetLoader(name);
		if (loader == -1)
		{
			std::cout << "Warning: No asset loader found for '" << name << "'." << std::endl;
			return;
		}
		
		Asset& asset = assets.emplace_back();
		asset.name = std::move(name);
		asset.source = std::move(source);
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
		
		asset.loadTime = std::chrono::system_clock::now();
		asset.loaded = true;
		
		if (asset.source.empty())
		{
			asset.readCallback = nullptr;
		}
		
		for (const std::function<void(void*)>& initCallback : asset.initCallbacks)
		{
			initCallback(asset.assetMemory);
		}
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
	
	static inline Asset& FindAsset(std::string_view name, const std::type_index* type)
	{
		std::string nameCanon = CanonicalPath(name);
		auto it = std::lower_bound(assets.begin(), assets.end(), nameCanon);
		if (it == assets.end() || it->name != nameCanon)
		{
			Panic(Concat({ "Asset not found: '", name, "'." }));
		}
		if (type != nullptr && detail::assetLoaders[it->loaderIndex].typeIndex != *type)
		{
			Panic(Concat({ "Attempted to get asset '", name, "' as an incorrect type." }));
		}
		return *it;
	}
	
	void* detail::GetAsset(std::string_view name, std::type_index type)
	{
		Asset& asset = FindAsset(name, &type);
		LoadAsset(asset);
		return asset.assetMemory;
	}
	
	void detail::InitAssetCallback(std::string_view name, std::type_index type, std::function<void(void*)> callback)
	{
		Asset& asset = FindAsset(name, &type);
		if (asset.loaded)
			callback(asset.assetMemory);
		if (!asset.source.empty())
			asset.initCallbacks.push_back(std::move(callback));
	}
	
	void detail::PollChangedAssets()
	{
#ifdef __linux__
		for (Asset& asset : assets)
		{
			if (asset.source.empty())
				continue;
			
			struct stat attrib;
			stat(asset.source.c_str(), &attrib);
			auto lastWriteTime = std::chrono::system_clock::from_time_t(attrib.st_mtime);
			
			if (lastWriteTime > asset.loadTime)
			{
				asset.Unload();
				LoadAsset(asset);
				std::cout << "Reloaded asset '" << asset.name << "'.\n";
			}
		}
#endif
	}
	
	void DisableAssetReload(std::string_view assetName)
	{
		FindAsset(assetName, nullptr).source.clear();
	}
}
