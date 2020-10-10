#include "pch.h"
#include "Shader.h"

#include "Assert.h"
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

	static void DestroyShader(Shader::IdType *id)
	{
		glDeleteShader(*id);
		delete id;
	}

	static Pointer<Shader::IdType> CreateShader(uint32_t shaderType)
	{
		auto shader = Pointer<Shader::IdType>(new Shader::IdType{}, DestroyShader);
		*shader = glCreateShader(shaderType);
		
		return shader;
	}

	Shader::Shader(const Type &type) : m_Type(type)
	{
		ASSERT(type != Type::Unknown, "Unknown ShaderType");

		m_Shader = CreateShader(static_cast<uint32_t>(type));
		
		GL_LOG_INFO("Creating {} shader", ShaderTypeToString(type));
		GL_LOG_INFO("{} Shader id: {}", ShaderTypeToString(type), *m_Shader);
	}

	Shader::Shader(const Type &type, const ShaderSource &source) : Shader(type)
	{
		SetSource(source);
		Compile();
	}

	bool Shader::Compile()
	{
		GL_LOG_INFO("Compiling [id: {}] {} Shader", *m_Shader, TypeToString());

		int status = GL_FALSE;

		glCompileShader(*m_Shader);
		glGetShaderiv(*m_Shader, GL_COMPILE_STATUS, &status);

		if(status == GL_FALSE)
		{
			GL_LOG_ERROR("Unable to compile [id: {}] {} Shader", *m_Shader, TypeToString());
			return m_Complied = false;
		}

		GL_LOG_INFO("Sucessful compiled [id: {}] {} Shader", *m_Shader, TypeToString());
		return m_Complied = true;
	}

	void Shader::SetSource(const ShaderSource &source)
	{
		auto cstr = source.GetSource().c_str();

		GL_LOG_INFO("Attaching source code to {} shader {}", *m_Shader, TypeToString());
		GL_LOG_DEBUG("Source code: \n{}\nEND", source.GetSource());

		m_Source = source;
		glShaderSource(*m_Shader, 1, &cstr, nullptr);
	}

	std::string Shader::GetLog() const
	{
		int length = 0;

		glGetShaderiv(*m_Shader, GL_INFO_LOG_LENGTH, &length);

		if(length > 0)
		{
			std::string log(static_cast<std::string::size_type>(length + 1), 0);
			glGetShaderInfoLog(*m_Shader, length, &length, &log[0]);

			return log;
		}

		return {};
	}

	std::string_view Shader::TypeToString() const
	{
		return ShaderTypeToString(m_Type);
	}
}
