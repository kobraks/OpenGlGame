#pragma once

#include <unordered_set>
#include <string>

#include "Types.h"

namespace Game
{
	class ShaderProgram;
	class Shader;

	class ShaderLoader
	{
	public:
		static Pointer<ShaderProgram> Load(const std::string &name, const std::string &path, bool detachShaders = true);


	private:
		static std::string Include(uint32_t type, const std::string &path);
		static std::string Include(uint32_t type, const std::string &path, std::unordered_set<std::string> &paths);
		static std::string LoadIncludedFile(uint32_t type, const std::string &path, std::unordered_set<std::string> &paths);
		static std::string PreProcessLine(uint32_t type, const std::string &line, std::unordered_set<std::string> &paths);

		static std::string LoadFile(uint32_t type, const std::string &fileName, const std::string &path);

		static Pointer<Shader> CreateShader(uint32_t type, const std::string &source);
	};
}
