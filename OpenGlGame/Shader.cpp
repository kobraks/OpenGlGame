#include "pch.h"
#include "Shader.h"

#include <fstream>
#include <sstream>

#include "Assert.h"
#include "GLCheck.h"
#include "Log.h"

namespace Game
{
	static constexpr std::string_view ShaderTypeToString(const Shader::Type& type)
	{
		switch (type)
		{
		case Shader::Type::Vertex: return "Vertex";
		case Shader::Type::Fragment: return "Fragment";
		case Shader::Type::Geometry: return "Geometry";
		case Shader::Type::Compute: return "Compute";
		case Shader::Type::Control: return "Tessellation Control";
		case Shader::Type::Evaluation: return "Tessellation Evaluation";
		case Shader::Type::Unknown: return "Unknown";
		}
		return {};
	}

	ShaderSource ShaderSource::Load(const std::string& fileName, const std::string& directory)
	{
		const auto name = directory + "/" + fileName;
		LOG_DEBUG("Loading shader source from: \"{}\"", name);

		std::ifstream file(name, std::ios::in);

		if (!file.good())
		{
			LOG_ERROR("Unable to open and load \"{}\"", name);
			file.close();
			return ShaderSource("");
		}

		std::stringstream stream;
		stream << file.rdbuf();

		file.close();

		return ShaderSource(stream.str());
	}

	Shader::Internals::Internals(const Shader::Type& type) : Type(type)
	{
		GL_CHECK(Shader = glCreateShader(static_cast<GLenum>(type)));
	}

	Shader::Internals::~Internals()
	{
		GL_CHECK(glDeleteShader(Shader));
	}

	void Shader::Internals::SetSource(const ShaderSource& source)
	{
		auto cstr = source.Source().c_str();

		GL_LOG_INFO("Attaching source code to {} shader {}", Shader, ShaderTypeToString(Type));
		GL_LOG_DEBUG("Source code: \n{}\nEND", source.Source());

		Source = source;

		GL_CHECK(glShaderSource(Shader, 1, &cstr, nullptr));
	}

	bool Shader::Internals::Compile()
	{
		GL_LOG_INFO("Compiling [id: {}] {} Shader", Shader, ShaderTypeToString(Type));


		GL_CHECK(glCompileShader(Shader));
		int status = Get(ParameterName::CompileStatus);

		if (status == GL_FALSE)
		{
			GL_LOG_ERROR("Unable to compile [id: {}] {} Shader", Shader, ShaderTypeToString(Type));
			return Complied = false;
		}

		GL_LOG_INFO("Sucessful compiled [id: {}] {} Shader", Shader, ShaderTypeToString(Type));
		return Complied = true;
	}

	std::string Shader::Internals::GetLog() const
	{
		int length = Get(ParameterName::LogLength);

		if (length > 0)
		{
			std::string log(length + 1, 0);

			GL_CHECK(glGetShaderInfoLog(Shader, length, &length, &log[0]));

			return log;
		}

		return {};
	}

	int Shader::Internals::Get(ParameterName name) const
	{
		int value = 0;
		Get(name, &value);

		return value;
	}

	void Shader::Internals::Get(ParameterName name, int* value) const
	{
		GL_CHECK(glGetShaderiv(Shader, static_cast<GLenum>(name), value));
	}

	Shader::Shader(const Type& type)
	{
		ASSERT(type != Type::Unknown, "Unknown ShaderType");

		m_Internals = MakePointer<Internals>(type);

		GL_LOG_INFO("Creating {} shader", ShaderTypeToString(type));
		GL_LOG_INFO("{} Shader id: {}", ShaderTypeToString(type), m_Internals->Shader);
	}

	Shader::Shader(const Type& type, const ShaderSource& source) : Shader(type)
	{
		SetSource(source);
		Compile();
	}

	std::string_view Shader::TypeToString() const
	{
		return ShaderTypeToString(m_Internals->Type);
	}

	std::string_view Shader::TypeToString(Type type)
	{
		return ShaderTypeToString(type);
	}
}
