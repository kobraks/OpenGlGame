#include "pch.h"
#include "Engine/OpenGL/ShaderSource.h"

namespace Game
{
	ShaderSource ShaderSource::Load(const std::filesystem::path path)
	{
		const auto name = path.string();
		LOG_DEBUG("Loading Shader source from: \"{}\"", name);

		std::ifstream file(name, std::ios::in);

		if(!file.good())
		{
			LOG_ERROR("Unable to open and load \"{}\"", name);
			return ShaderSource("");
		}

		std::stringstream stream;
		stream << file.rdbuf();

		return ShaderSource(stream.str());
	}

	ShaderSource::ShaderSource(const std::string &source) : m_Source(source) {}

	void ShaderSource::Source(const std::string &source)
	{
		m_Source = source;
	}
}
