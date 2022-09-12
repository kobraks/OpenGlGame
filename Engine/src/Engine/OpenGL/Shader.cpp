#include "pch.h"
#include "Engine/OpenGL/Shader.h"

#include "Engine/Renderer/Context.h"

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

	Shader::Internals::Internals(const Shader::Type &type) : Type(type), Functions(OpenGlFunctions::GetFunctions())
	{
		Shader = Functions.CreateShader(static_cast<uint32_t>(type));
	}

	Shader::Internals::~Internals()
	{
		Functions.DeleteShader(Shader);
	}

	void Shader::Internals::SetSource(const ShaderSource &source)
	{
		GL_LOG_INFO("Attaching source code to {} shader {}", Shader, ShaderTypeToString(Type));
		GL_LOG_DEBUG("Source code: \n{}\nEND", source.Source());

		Source = source;

		Functions.ShaderSource(Shader, source.Source());
	}

	bool Shader::Internals::Compile()
	{
		GL_LOG_INFO("Compliling [id: {}] {} Shader", Shader, ShaderTypeToString(Type));

		Functions.CompileShader(Shader);

		if (const int status = Get(ShaderParameterName::CompileStatus); status == GL_FALSE)
		{
			GL_LOG_ERROR("Unable to compile [id: {}] {} Shader", Shader, ShaderTypeToString(Type));
			return Compiled = false;
		}

		GL_LOG_INFO("Sucessful compiled [id: {}] {} Shader", Shader, ShaderTypeToString(Type));
		return Compiled = true;
	}

	std::string Shader::Internals::GetLog() const
	{
		return Functions.ShaderInfoLog(Shader);
	}

	int Shader::Internals::Get(ShaderParameterName name) const
	{
		return Functions.GetShader(Shader, name);
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
