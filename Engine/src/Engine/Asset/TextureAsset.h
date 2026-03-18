#pragma once
#include "Asset.h"

#include "Engine/Renderer/Texture.h"

namespace Engine {
	class TextureAsset : public Asset {
	public:
		Ref<Texture> Resource = nullptr;

		AssetType GetType() const override {
			return AssetType::Texture;
		}
	};
}