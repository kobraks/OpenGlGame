#pragma once

#include "Engine/Core/Base.h"
#include "Engine/OpenGL/OpenGlFunctions.h"
#include "Engine/OpenGL/ShaderSource.h"

#include <string>
#include <filesystem>

#include <glad/glad.h>

namespace Game
{
	class Shader
	{
	public:
		enum class Type : uint32_t
		{
			Unknown = 0,
			Vertex = GL_VERTEX_SHADER,
			Fragment = GL_FRAGMENT_SHADER,
			Geometry = GL_GEOMETRY_SHADER,
			Compute = GL_COMPUTE_SHADER,
			Control = GL_TESS_CONTROL_SHADER,
			Evaluation = GL_TESS_EVALUATION_SHADER
		};

		using IDType = uint32_t;

	private:
		class Internals
		{
		public:
			ShaderSource Source;

			Type Type = Type::Unknown;
			bool Compiled = false;
			IDType Shader = 0;

			mutable OpenGlFunctions Functions;

			explicit Internals(const Shader::Type& type);

			~Internals();

			void SetSource(const ShaderSource& source);
			bool Compile();

			std::string GetLog() const;

			int Get(ShaderParameterName name) const;
		};

		Pointer<Internals> m_Internals;

	public:
		explicit Shader(const Type& type);
		Shader(const Type &type, const ShaderSource& source);

		bool Compile() { return m_Internals->Compile(); }
		bool IsCompiled() const { return m_Internals->Compiled; }

		void SetSource(const ShaderSource &source) { m_Internals->SetSource(source); }
		const ShaderSource& GetSource() const { return m_Internals->Source; }

		std::string GetLog() const { return m_Internals->GetLog(); }

		operator IDType() const { return m_Internals->Shader; }

		IDType ID() const { return m_Internals->Shader; }

		Type GetType() const { return m_Internals->Type; }

		std::string_view TypeToString() const;

		static std::string_view TypeToString(Type type);
	};

#define SHADER_IMPLEMENTATION_TYPE(type)\
	class type##Shader : public Shader\
	{\
	public:\
		type##Shader() : Shader(Shader::Type::type) {}\
		\
		type##Shader(const ShaderSource& source) : Shader(Shader::Type::type, source){}\
		\
	}

	SHADER_IMPLEMENTATION_TYPE(Vertex);
	SHADER_IMPLEMENTATION_TYPE(Fragment);
	SHADER_IMPLEMENTATION_TYPE(Geometry);
	SHADER_IMPLEMENTATION_TYPE(Compute);
	SHADER_IMPLEMENTATION_TYPE(Control);
	SHADER_IMPLEMENTATION_TYPE(Evaluation);

#undef SHADER_IMPLEMENTATION_TYPE
}
