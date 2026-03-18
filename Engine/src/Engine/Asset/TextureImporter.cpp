#include "pch.h"
#include "TextureImporter.h"

#include <filesystem>

#include "Engine/Asset/TextureAsset.h"

#include "Engine/Renderer/Builders/TextureBuilder.h"
#include "Engine/Core/Image.h"

namespace Engine {
	Ref<TextureAsset> TextureImporter::ImportTexture(AssetHandle handle, const AssetMetadata& metadata) {
		TextureBuilder builder;
		Ref<Image> image = Image::Load(metadata.Get<std::filesystem::path>("Path"));

		builder.FromImage(image);

		auto asset = MakeRef<TextureAsset>();
		asset->Resource = Texture::Create(builder.BuildSpecification());

		return asset;
	}
}
