#include "pch.h"
#include "EditorAssetManager.h"

namespace Engine {
	bool EditorAssetManager::IsAssetHandleValid(const AssetHandle& handle) const {
		return handle != 0 && m_Registry.Contains(handle);
	}

	bool EditorAssetManager::IsAssetLoaded(const AssetHandle& handle) const {
		return m_LoadedAssets.contains(handle);
	}

	const AssetMetadata& EditorAssetManager::GetMetadata(const AssetHandle& handle) const {
		static AssetMetadata s_NullMetadata;
		if (!m_Registry.Contains(handle))
			return s_NullMetadata;

		return m_Registry.Get(handle);
	}

	Ref<Asset> EditorAssetManager::GetAsset(const AssetHandle& handle) const {
		if (!IsAssetHandleValid(handle))
			return nullptr;

		Ref<Asset> asset = nullptr;
		if (IsAssetLoaded(handle)) {
			asset = m_LoadedAssets.at(handle);
		}
		else {
			const AssetMetadata& metadata = GetMetadata(handle);

			
		}

		return asset;
 	}
}
