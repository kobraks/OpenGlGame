#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/UUID.h"

#include <mutex>

namespace Engine {
	class AssetManager;

	enum class AssetType {
		None = 0,
		Texture,
		Mesh,
		Shader,
		Material,
		Script,
		Scene,
		Audio,
	};

	using AssetHandle = UUID;

	class Asset {
		friend class AssetManager;
	public:
		virtual ~Asset() = default;
		AssetHandle Handle() const { return m_Handle; }

		virtual AssetType GetType() const = 0;

	private:
		AssetHandle m_Handle;
	};

	namespace Utils {
		std::string_view ToString(AssetType type);
	}
}

template <>
struct fmt::formatter<Engine::AssetType> : fmt::formatter<std::string_view> {
	auto format(Engine::AssetType v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};