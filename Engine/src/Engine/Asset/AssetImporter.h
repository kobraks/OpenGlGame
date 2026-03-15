#pragma once
#include "AssetMetadata.h"
#include "Engine/Renderer/Texture.h"

namespace Engine {
	class AssetImporter {
	public:
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};
}