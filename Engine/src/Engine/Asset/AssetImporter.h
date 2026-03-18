#pragma once
#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Asset/TextureAsset.h"

namespace Engine {
	class AssetImporter {
	public:
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};
}