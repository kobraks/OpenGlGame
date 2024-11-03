#pragma once
#include "Engine/Core/Base.h"

#include <glad/glad.h>

#include <string>

namespace Engine {
	class ShaderSource;

	class Shader {
	public:
		enum class Type : GLenum{
			Unknown = 0,
			Vertex = GL_VERTEX_SHADER,
			Fragment = GL_FRAGMENT_SHADER,
			Geometry = GL_GEOMETRY_SHADER,
			Compute = GL_COMPUTE_SHADER,
			Control = GL_TESS_CONTROL_SHADER,
			Evaluation = GL_TESS_EVALUATION_SHADER
		};

		enum class ParameterName : GLenum {
			ShaderType = GL_SHADER_TYPE,
			DeleteStatus = GL_DELETE_STATUS,
			CompileStatus = GL_COMPILE_STATUS,
			LogLength = GL_INFO_LOG_LENGTH,
			SourceLength = GL_SHADER_SOURCE_LENGTH
		};

		using IDType = uint32_t;

		explicit Shader(Type type);
		Shader(Type type, Ref<ShaderSource> source);

		bool Compile() { return m_Internals->Compile(); }
		bool IsCompiled() { return m_Internals->Compiled; }

		void SetSource(Ref<ShaderSource> source) { m_Internals->SetSource(source); }
		Ref<ShaderSource> GetSource() const { return m_Internals->Source; }

		std::string GetLog() const { return m_Internals->GetLog(); }

		operator IDType() const { return m_Internals->Shader; }
		IDType ID() const { return m_Internals->Shader; }

		Type GetType() const { return m_Internals->Type; }

		std::string_view TypeToString() const;

		static std::string_view TypeToString(Type type);

	private:
		class Internals {
		public:
			Ref<ShaderSource> Source;
			Type Type     = Type::Unknown;
			bool Compiled = false;
			IDType Shader = 0;

			explicit Internals(Shader::Type type);
			~Internals();

			void SetSource(Ref<ShaderSource> source);
			bool Compile();
			std::string GetLog() const;

			int Get(ParameterName name) const;
			void Get(ParameterName pName, int *value) const;
		};

		Ref<Internals> m_Internals;
	};

#define SHADER_CLASS_IMPLEMENTATION_TYPE(type)\
	class type##Shader : public Shader {\
	public:\
		type##Shader() : Shader(Shader::Type::type){}\
		type##Shader(Ref<ShaderSource> source) : Shader(Shader::Type::type, source){}\
	}

	SHADER_CLASS_IMPLEMENTATION_TYPE(Vertex);

	SHADER_CLASS_IMPLEMENTATION_TYPE(Fragment);

	SHADER_CLASS_IMPLEMENTATION_TYPE(Geometry);

	SHADER_CLASS_IMPLEMENTATION_TYPE(Compute);

	SHADER_CLASS_IMPLEMENTATION_TYPE(Control);

	SHADER_CLASS_IMPLEMENTATION_TYPE(Evaluation);
}
