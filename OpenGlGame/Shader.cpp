#include "pch.h"
#include "Shader.h"

#include <fstream>
#include <sstream>

#include "Assert.h"
#include "GLCheck.h"
#include "Log.h"

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
			case Shader::Type::Unknown:
				return "Unknown";
		}
		return {};
	}

	ShaderSource ShaderSource::Load(const std::string &fileName, const std::string &directory)
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

	Shader::Internals::Internals(const Shader::Type &type) : Type(type)
	{
		GL_CHECK(Shader = glCreateShader(static_cast<GLenum>(type)));
	}

	Shader::Internals::~Internals()
	{
		GL_CHECK(glDeleteShader(Shader));
	}

	Shader::Shader(const Type &type)
	{
		ASSERT(type != Type::Unknown, "Unknown ShaderType");

		m_Internals = MakePointer<Internals>(type);
		
		GL_LOG_INFO("Creating {} shader", ShaderTypeToString(type));
		GL_LOG_INFO("{} Shader id: {}", ShaderTypeToString(type), m_Internals->Shader);
	}

	Shader::Shader(const Type &type, const ShaderSource &source) : Shader(type)
	{
		SetSource(source);
		Compile();
	}

	bool Shader::Compile()
	{
		const IdType& shader = m_Internals->Shader;
		
		GL_LOG_INFO("Compiling [id: {}] {} Shader", shader, TypeToString());

		int status = GL_FALSE;

		GL_CHECK(glCompileShader(shader));
		GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &status));

		if(status == GL_FALSE)
		{
			GL_LOG_ERROR("Unable to compile [id: {}] {} Shader", shader, TypeToString());
			return m_Internals->Complied = false;
		}

		GL_LOG_INFO("Sucessful compiled [id: {}] {} Shader", shader, TypeToString());
		return m_Internals->Complied = true;
	}

	void Shader::SetSource(const ShaderSource &source)
	{
		auto cstr = source.GetSource().c_str();

		GL_LOG_INFO("Attaching source code to {} shader {}", m_Internals->Shader, TypeToString());
		GL_LOG_DEBUG("Source code: \n{}\nEND", source.GetSource());

		m_Internals->Source = source;
		GL_CHECK(glShaderSource(m_Internals->Shader, 1, &cstr, nullptr));
	}

	std::string Shader::GetLog() const
	{
		int length = 0;

		GL_CHECK(glGetShaderiv(m_Internals->Shader, GL_INFO_LOG_LENGTH, &length));

		if(length > 0)
		{
			std::string log(static_cast<std::string::size_type>(length + 1), 0);
			GL_CHECK(glGetShaderInfoLog(m_Internals->Shader, length, &length, &log[0]));

			return log;
		}

		return {};
	}

	std::string_view Shader::TypeToString() const
	{
		return ShaderTypeToString(m_Internals->Type);
	}
}
