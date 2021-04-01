#include "pch.h"
#include "TextureLoader.h"

#include "Log.h"

namespace Game
{
	std::unordered_map<std::string, Pointer<Texture>> TextureLoader::s_Textures;

	Pointer<Texture> TextureLoader::Load(const std::string &fileName, const std::string &dir)
	{
		const auto name = dir + "/" + fileName;

		if(const auto iter = s_Textures.find(name); iter != s_Textures.end())
			return iter->second;

		try
		{
			LOG_INFO("Loading texture from \"{}\"", name);
			Image image;
			image.Load(name);
			
			return s_Textures[name] = MakePointer<Texture>(image);
		}
		catch(std::exception &ex)
		{
			LOG_ERROR("Unable to load texture \"{}\", because: {}", name, ex.what());
		}

		return nullptr;
	}
}
