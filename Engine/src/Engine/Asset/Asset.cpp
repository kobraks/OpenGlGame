#include "pch.h"
#include "Asset.h"

namespace Engine {
	namespace Utils {
		std::string_view ToString(AssetType type) {
			switch (type) {
			case AssetType::None: return "None";
			case AssetType::Texture: return "Texture";
			case AssetType::Mesh: return "Mesh";
			case AssetType::Shader: return "Shader";
			case AssetType::Material: return "Material";
			case AssetType::Script: return "Script";
			case AssetType::Scene: return "Scene";
			case AssetType::Audio: return "Audio";
			default: return "Unknown";
			}
		}
	}
}
 