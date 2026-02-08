#include "pch.h"
#include "AssetRegistry.h"
#include <mutex>

#define ENGINE_ASSET_REGISTRY_LOG 1

#if ENGINE_ASSET_REGISTRY_LOG
#define ASSET_LOG(...) LOG_ENGINE_TRACE("Asset", __VA_ARGS__)
#else
#define ASSET_LOG(...)
#endif

namespace Engine {
	static std::mutex s_AssetRegistryMutex;

	AssetMetadata& AssetRegistry::operator[](const AssetHandle handle) {
		std::scoped_lock lock(s_AssetRegistryMutex);

		ASSET_LOG("Retriving handle {}", handle);
		return m_AssetRegistry[handle];
	}

	AssetMetadata& AssetRegistry::Get(const AssetHandle handle) {
		std::scoped_lock lock(s_AssetRegistryMutex);

		ENGINE_ASSERT(m_AssetRegistry.contains(handle));
		ASSET_LOG("Retriving handle {}", handle);
		return m_AssetRegistry.at(handle);
	}

	const AssetMetadata& AssetRegistry::Get(const AssetHandle handle) const {
		std::scoped_lock lock(s_AssetRegistryMutex);

		ENGINE_ASSERT(m_AssetRegistry.find(handle) != m_AssetRegistry.end());
		ASSET_LOG("Retriving handle {}", handle);
		return m_AssetRegistry.at(handle);
	}

	bool AssetRegistry::Contains(const AssetHandle handle) const {
		std::scoped_lock lock(s_AssetRegistryMutex);

		ASSET_LOG("Contains handle {}", handle);
		return m_AssetRegistry.contains(handle);
	}

	size_t AssetRegistry::Remove(const AssetHandle handle) {
		std::scoped_lock lock(s_AssetRegistryMutex);

		ENGINE_ASSERT(m_AssetRegistry.find(handle) != m_AssetRegistry.end());
		ASSET_LOG("Removing handle {}", handle);
		return m_AssetRegistry.erase(handle);
	}

	void AssetRegistry::Clear() {
		std::scoped_lock lock(s_AssetRegistryMutex);

		ASSET_LOG("Clear asset registy");
		m_AssetRegistry.clear();
	}

	auto AssetRegistry::Find(const AssetHandle handle) {
		std::scoped_lock lock(s_AssetRegistryMutex);
		return m_AssetRegistry.find(handle);
	}

	auto AssetRegistry::Find(const AssetHandle handle) const {
		std::scoped_lock lock(s_AssetRegistryMutex);
		return m_AssetRegistry.find(handle);
	}
}
