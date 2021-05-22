#include "pch.h"
#include "ShaderLoader.h"

#include <fstream>
#include <sstream>

#include "Shader.h"
#include "ShaderProgram.h"

namespace Game
{
	static constexpr uint32_t VERTEX_SHADER     = 0;
	static constexpr uint32_t CONTROL_SHADER    = 1;
	static constexpr uint32_t EVALUATION_SHADER = 2;
	static constexpr uint32_t GEOMETRY_SHADER   = 3;
	static constexpr uint32_t FRAGMENT_SHADER   = 4;
	static constexpr uint32_t COMPUTE_SHADER    = 5;

	static std::string GetShaderEx(uint32_t type)
	{
		switch(type)
		{
			case VERTEX_SHADER: return ".vert";
			case CONTROL_SHADER: return ".tesc";
			case EVALUATION_SHADER: return ".tese";
			case GEOMETRY_SHADER: return ".geom";
			case FRAGMENT_SHADER: return ".frag";
			case COMPUTE_SHADER: return ".comp";
		}

		return "";
	}

	static std::string GetShaderFileName(uint32_t type, const std::string &name)
	{
		return name + GetShaderEx(type);
	}

	static bool IsSameShaderType(uint32_t currentType, const std::string &fileName)
	{
		const std::string ex = GetShaderEx(currentType);

		return fileName.find(ex) != std::string::npos;
	}

	static bool IsInclude(const std::string &line)
	{
		return false;
	}

	Pointer<ShaderProgram> ShaderLoader::Load(const std::string &name, const std::string &path, bool detachShaders)
	{
		Pointer<ShaderProgram> program = MakePointer<ShaderProgram>(name);
		std::vector<Pointer<Shader>> shaders;
		shaders.reserve(6);

		for(uint32_t i = 0; i < 6; ++i)
		{
			auto shader = CreateShader(i, LoadFile(i, GetShaderFileName(i, name), path));

			if(shader)
			{
				program->Attach(shader);
				shaders.emplace_back(shader);
			}
		}

		program->Link();

		if(detachShaders)
		{
			for(const auto &shader : shaders)
				program->Detach(shader);
		}

		return program;
	}

	std::string ShaderLoader::Include(uint32_t type, const std::string &path, std::unordered_set<std::string> &paths)
	{
		if(IsSameShaderType(type, path))
		{
			if(paths.contains(path))
			{
				ASSERT(false, "File already included");
				throw std::runtime_error("Already included file");
			}

			paths.emplace(path);
			return LoadIncludedFile(type, path, paths);
		}

		ASSERT(false, "Included wrong shaderType")
		throw std::runtime_error("Included wrong shaderType");
		return "";
	}

	std::string ShaderLoader::LoadIncludedFile(uint32_t type, const std::string &path, std::unordered_set<std::string> &paths)
	{
		std::ifstream file(path, std::ios::in);

		if(file.good())
		{
			std::stringstream str;

			std::string line;

			while(std::getline(file, line))
			{
				str << PreProcessLine(type, line, paths) << "\n";
			}

			return str.str();
		}

		file.close();
		return "";
	}

	std::string ShaderLoader::PreProcessLine(uint32_t type, const std::string &line, std::unordered_set<std::string> &paths)
	{
		if(IsInclude(line))
		{
			std::string path = line;

			return Include(type, path, paths);
		}
		return line;
	}

	std::string ShaderLoader::LoadFile(uint32_t type, const std::string &fileName, const std::string &path)
	{
		std::unordered_set<std::string> paths;
		const std::string name = path + "/" + fileName;

		std::ifstream file(name, std::ios::in);

		if(file.good())
		{
			std::stringstream str;

			std::string line;

			while(std::getline(file, line))
			{
				str << PreProcessLine(type, line, paths) << "\n";
			}

			return str.str();
		}

		file.close();
		return "";
	}

	Pointer<Shader> ShaderLoader::CreateShader(uint32_t type, const std::string &source)
	{
		if(source.empty())
			return nullptr;

		Pointer<Shader> shader = nullptr;

		switch(type)
		{
			case VERTEX_SHADER: shader = MakePointer<VertexShader>();
				break;
			case CONTROL_SHADER: shader = MakePointer<ControlShader>();
				break;
			case EVALUATION_SHADER: shader = MakePointer<EvaluationShader>();
				break;
			case GEOMETRY_SHADER: shader = MakePointer<GeometryShader>();
				break;
			case FRAGMENT_SHADER: shader = MakePointer<FragmentShader>();
				break;
			case COMPUTE_SHADER: shader = MakePointer<ComputeShader>();
				break;

			default: LOG_WARN("Unknown shader");
				return nullptr;
		}

		shader->SetSource(ShaderSource(source));
		shader->Compile();

		return shader;
	}
}
