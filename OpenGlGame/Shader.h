#pragma once
#include <glad/glad.h>
#include "Types.h"

namespace Game
{
	class ShaderSource
	{
		std::string m_Source;
	public:
		ShaderSource() = default;
		static ShaderSource Load(const std::string &fileName, const std::string &directory);

		ShaderSource(std::string source) : m_Source(std::move(source)) {}

		const std::string& Source() const { return m_Source; }
		void Source(const std::string &source) { m_Source = source; }
	};

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

		enum class ParameterName : GLenum
		{
			ShaderType = GL_SHADER_TYPE,
			DeleteStatus = GL_DELETE_STATUS,
			CompileStatus = GL_COMPILE_STATUS,
			LogLength = GL_INFO_LOG_LENGTH,
			SourceLength = GL_SHADER_SOURCE_LENGTH
		};

		using IdType = uint32_t;
	private:
		class Internals
		{
		public:
			ShaderSource Source;
			Type Type     = Type::Unknown;
			bool Complied = false;
			IdType Shader;

			explicit Internals(const Shader::Type& type);
			~Internals();

			void SetSource(const ShaderSource& source);
			bool Compile();
			std::string GetLog() const;

			int Get(ParameterName name) const;
			void Get(ParameterName name, int *value) const;
		};

		Pointer<Internals> m_Internals;
	public:
		explicit Shader(const Type &type);
		Shader(const Type &type, const ShaderSource &source);

		bool Compile() { return m_Internals->Compile(); }

		bool IsCompiled() const { return m_Internals->Complied; }

		void SetSource(const ShaderSource &source) { m_Internals->SetSource(source); }
		const ShaderSource& GetSource() const { return m_Internals->Source; }

		std::string GetLog() const { return m_Internals->GetLog(); }

		operator IdType() const { return m_Internals->Shader; }
		IdType ID() const { return m_Internals->Shader; }
		Type GetType() const { return m_Internals->Type; }

		std::string_view TypeToString() const;

		static std::string_view TypeToString(Type type);
	};

#define SHADER_IMPLEMENTATION_TYPE(type)\
	class type##Shader : public Shader\
	{\
	public:\
		type##Shader() : Shader(Shader::Type::type){}\
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
	
}
