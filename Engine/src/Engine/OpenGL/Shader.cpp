#include "pch.h"
#include "Engine/OpenGL/Shader.h"

#include <fstream>
#include <sstream>

#include "Engine/Core/Assert.h"
#include "Engine/Core/Log.h"

namespace Game
{
	static constexpr std::string_view ShaderTypeToString(const Shader::Type &type)
	{
		switch(type)
		{
			case Shader::Type::Vertex:
				return "Vertex";
			case Shader::Type::Fragment:
				return "Fragment";
			case Shader::Type::Geometry:
				return "Geometry";
			case Shader::Type::Compute:
				return "Compute";
			case Shader::Type::Control:
				return "Tessellation Control";
			case Shader::Type::Evaluation:
				return "Tessellation Evaluation";
			case Shader::Type::Unknown:
				return "Unknown";
		}
		return {};
	}

	ShaderSource ShaderSource::Load(const std::filesystem::path path)
	{
		const auto name = path.string();
		LOG_DEBUG("Loading Shader source from: \"{}\"", name);

		std::ifstream file(name, std::ios::in);

		if (!file.good())
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

	Shader::Internals::Internals(const Shader::Type &type) : Type(type)
	{
		Shader = glCreateShader(static_cast<GLenum>(type));
	}

	Shader::Internals::~Internals()
	{
		glDeleteShader(Shader);
	}

	void Shader::Internals::SetSource(const ShaderSource &source)
	{
		auto cstr = source.Source().c_str();

		GL_LOG_INFO("Attaching source code to {} shader {}", Shader, ShaderTypeToString(Type));
		GL_LOG_DEBUG("Source code: \n{}\nEND", source.Source());

		Source = source;

		glShaderSource(Shader, 1, &cstr, nullptr);
	}

	bool Shader::Internals::Compile()
	{
		GL_LOG_INFO("Compliling [id: {}] {} Shader", Shader, ShaderTypeToString(Type));

		glCompileShader(Shader);

		int status = Get(ParameterName::CompileStatus);

		if (status == GL_FALSE)
		{
			GL_LOG_ERROR("Unable to compile [id: {}] {} Shader", Shader, ShaderTypeToString(Type));
			return Compiled = false;
		}

		GL_LOG_INFO("Sucessful compiled [id: {}] {} Shader", Shader, ShaderTypeToString(Type));
		return Compiled = true;
	}

	std::string Shader::Internals::GetLog() const
	{
		int length = Get(ParameterName::LogLength);

		if (length > 0)
		{
			std::string log(length + 1, 0);

			glGetShaderInfoLog(Shader, length, &length, &log[0]);

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

	void Shader::Internals::Get(ParameterName name, int *value) const
	{
		glGetShaderiv(Shader, static_cast<GLenum>(name), value);
	}

	Shader::Shader(const Type &type)
	{
		ASSERT(type != Type::Unknown, "Unknown Shader Type");

		m_Internals = MakePointer<Internals>(type);

		GL_LOG_INFO("Creating {} Shader", ShaderTypeToString(type));
		GL_LOG_INFO("{} Shader id: {}", ShaderTypeToString(type), m_Internals->Shader);
	}

	Shader::Shader(const Type &type, const ShaderSource &source) : Shader(type)
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
		return ShaderTypeToString((type));
	}
}
