#pragma once
#include "Asset.h"
#include "AssetMetadata.h"

namespace Engine {
	class TextureAsset;

	class TextureImporter {
	public:
		static Ref<TextureAsset> ImportTexture(AssetHandle handle, const AssetMetadata& metadata);
	};


}