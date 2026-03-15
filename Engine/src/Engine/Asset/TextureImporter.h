#pragma once
#include "Asset.h"
#include "AssetMetadata.h"

namespace Engine {
	class Texture;

	class TextureImporter {
	public:
		static Ref<Texture> ImportTexture(AssetHandle handle, const AssetMetadata& metadata);
	};


}