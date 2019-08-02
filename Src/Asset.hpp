#pragma once

#include "Graphics/Texture.hpp"
#include "API.hpp"

#include <any>
#include <functional>
#include <string>
#include <string_view>
#include <typeindex>
#include <gsl/span>

namespace jm
{
	namespace detail
	{
		using ProcessAssetCB = void (*)(std::string name, std::string source, std::function<std::vector<char>()> loadCallback);
		
		struct AssetLoader
		{
			std::string extension;
			std::function<void(gsl::span<const char> data, const std::string& name, void* asset)> loadCallback;
			void(*destructor)(void*);
			std::type_index typeIndex;
			size_t typeSize;
			
			AssetLoader(std::string _extension, const std::type_index& _typeIndex, size_t _typeSize)
				: extension(std::move(_extension)), typeIndex(_typeIndex), typeSize(_typeSize) { }
		};
		
		JAPI extern std::vector<AssetLoader> assetLoaders;
		
		bool FindAssetsZip(ProcessAssetCB processAsset);
		bool FindAssetsDir(ProcessAssetCB processAsset);
		
		void LoadAssets();
		
		JAPI void* GetAsset(std::string_view name, std::type_index type);
		
		JAPI void InitAssetCallback(std::string_view name, std::type_index type, std::function<void(void*)> callback);
		
		JAPI void PollChangedAssets();
	}
	
	template <typename T>
	inline void RegisterAssetLoader(std::string extension, std::function<T(gsl::span<const char>, const std::string&)> loader)
	{
		auto& assetLoader = detail::assetLoaders.emplace_back(std::move(extension), std::type_index(typeid(T)), sizeof(T));
		assetLoader.loadCallback = [loader=std::move(loader)] (gsl::span<const char> data, const std::string& name, void* asset)
		{
			new (asset) T(loader(data, name));
		};
		assetLoader.destructor = [] (void* asset) { static_cast<T*>(asset)->~T(); };
	}
	
	template <typename T>
	T& GetAsset(std::string_view name)
	{
		return *static_cast<T*>(detail::GetAsset(name, std::type_index(typeid(T))));
	}
	
	template <typename T>
	void InitAssetCallback(std::string_view name, std::function<void(T& asset)> callback)
	{
		detail::InitAssetCallback(name, std::type_index(typeid(T)), [callback=std::move(callback)] (void* asset)
		{
			callback(*static_cast<T*>(asset));
		});
	}
	
	void DisableAssetReload(std::string_view assetName);
}
