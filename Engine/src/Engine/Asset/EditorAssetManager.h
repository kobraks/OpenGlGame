#pragma once
#include "Engine/Asset/AssetManagerBase.h"
#include "Engine/Asset/AssetRegistry.h"

#include <unordered_map>
#include <future>
#include <type_traits>

namespace Engine {
	class EditorAssetManager : public AssetManagerBase {
		virtual Ref<Asset> GetAsset(const AssetHandle& handle) const override;

		virtual bool IsAssetHandleValid(const AssetHandle& handle) const override;
		virtual bool IsAssetLoaded(const AssetHandle& handle) const override;

		const AssetMetadata& GetMetadata(const AssetHandle& handle) const;
	private:
		AssetRegistry m_Registry;
		AssetMap m_LoadedAssets;
	};
}
