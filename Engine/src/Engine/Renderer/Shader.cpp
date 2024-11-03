#include "pch.h"
#include "Shader.h"

#include "ShaderSource.h"

namespace Engine {
	constexpr std::string_view ShaderTypeString(Shader::Type type) {
		switch(type) {
			case Shader::Type::Unknown:
				return "Unknown";
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
		}

		return "Unknown";
	}

	Shader::Internals::Internals(const Shader::Type type) : Type(type) {
		Shader = glCreateShader(static_cast<::GLenum>(type));
	}

	Shader::Internals::~Internals() {
		glDeleteShader(Shader);
	}

	void Shader::Internals::SetSource(Ref<ShaderSource> source) {
		const auto cstr = source->Source().data();

		LOG_GL_INFO("Attaching source code to {} shader: {}", Shader, ShaderTypeString(Type));
		LOG_GL_DEBUG("Source code: \n{}\nEND", cstr);

		Source = source;

		glShaderSource(Shader, 1, &cstr, nullptr);
	}

	bool Shader::Internals::Compile() {
		LOG_GL_INFO("Compiling [id: {}] {} Shader", Shader, ShaderTypeString(Type));

		glCompileShader(Shader);

		int status = Get(ParameterName::CompileStatus);

		if (status == GL_FALSE) {
			LOG_GL_ERROR("Unable to compile [id: {}] {} Shader", Shader, ShaderTypeString(Type));
			return Compiled = false;
		}

		LOG_GL_INFO("Sucessfuly compiled [id: {}] {} Shader", Shader, ShaderTypeString(Type));
		return Compiled = true;
	}

	std::string Shader::Internals::GetLog() const {
		int length = Get(ParameterName::LogLength);

		if (length > 0) {
			std::string log(length + 1, 0);

			glGetShaderInfoLog(Shader, length, &length, &log[0]);

			return log;
		}

		return {};
	}

	int Shader::Internals::Get(ParameterName name) const {
		int value = 0;
		Get(name, &value);

		return value;
	}

	void Shader::Internals::Get(ParameterName pName, int *value) const {
		glGetShaderiv(Shader, static_cast<GLenum>(pName), value);
	}

	Shader::Shader(const Type type) {
		ENGINE_ASSERT(type != Type::Unknown);

		m_Internals = MakeRef<Internals>(type);

		LOG_GL_INFO("Creating {} Shader", ShaderTypeString(type));
		LOG_GL_INFO("{} Shader id: {}", ShaderTypeString(type), m_Internals->Shader);
	}

	Shader::Shader(const Type type, Ref<ShaderSource> source) : Shader(type){
		SetSource(source);
		Compile();
	}

	std::string_view Shader::TypeToString() const {
		return ShaderTypeString(m_Internals->Type);
	}

	std::string_view Shader::TypeToString(Type type) {
		return ShaderTypeString(type);
	}
}
