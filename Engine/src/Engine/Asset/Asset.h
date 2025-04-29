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
		Audio,
	};

	using AssetHandle = UUID;

	class Asset {
		friend class AssetManager;
	public:
		AssetHandle Handle() const { return m_Handle; }

		virtual AssetType GetType() const = 0;

	private:
		AssetHandle m_Handle;
	};
}