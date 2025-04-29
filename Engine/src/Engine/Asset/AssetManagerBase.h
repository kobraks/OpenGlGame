#pragma once
#include "Engine/Asset/Asset.h"

namespace Engine {
	using AssetMap = std::unordered_map<AssetHandle, Ref<Asset>>;

	class AssetManagerBase {
	public:
		virtual Ref<Asset> GetAsset(const AssetHandle& handle) const = 0;

		virtual bool IsAssetHandleValid(const AssetHandle& handle) const = 0;
		virtual bool IsAssetLoaded(const AssetHandle& handle) const = 0;
	};
}