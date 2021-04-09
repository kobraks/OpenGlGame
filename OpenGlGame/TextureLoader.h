#pragma once
#include <unordered_map>
#include <string>
#include "Texture.h"

namespace Game
{
	class TextureLoader
	{
		static std::unordered_map<std::string, Pointer<Texture>> s_Textures;
	public:
		static Pointer<Texture> Load(const std::string& fileName, const std::string& dir);

		static void ClearCashed();
	};
}
