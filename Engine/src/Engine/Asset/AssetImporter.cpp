#include "pch.h"
#include "AssetImporter.h"

#include <unordered_map>
#include <functional>

#include "TextureImporter.h"
namespace Engine {

	using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
	static std::unordered_map<AssetType, AssetImportFunction> s_AssertImportFunction = {
		{AssetType::Texture, TextureImporter::ImportTexture}
	};

	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata) {
		if (!s_AssertImportFunction.contains(metadata.Type)) {
			ENGINE_ASSERT("No importer available for asset type: {}", metadata.Type);
			return nullptr;
		}

		return s_AssertImportFunction.at(metadata.Type)(handle, metadata);
	}
}
