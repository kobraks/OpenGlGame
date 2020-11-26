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
		static ShaderSource Load(const std::string &fileName);

		ShaderSource(const std::string &source) { m_Source = source; }

		const std::string& GetSource() const { return m_Source; }
		void SetSource(const std::string &source) { m_Source = source; }
	};

	class Shader
	{
	public:
		enum class Type : uint32_t
		{
			Unknown = 0,
			Vertex = GL_VERTEX_SHADER,
			Fragment = GL_FRAGMENT_SHADER,
			Geometry = GL_GEOMETRY_SHADER
		};

		using IdType = uint32_t;
	private:
		ShaderSource m_Source;

		Type m_Type     = Type::Unknown;
		bool m_Complied = false;
		Pointer<IdType> m_Shader;
	public:
		explicit Shader(const Type &type);
		Shader(const Type &type, const ShaderSource &source);

		bool Compile();

		void SetSource(const ShaderSource &source);
		const ShaderSource& GetSource() const { return m_Source; }

		std::string GetLog() const;

		operator IdType() const { return *m_Shader; }
		IdType ID() const { return *m_Shader; }
		Type GetType() const { return m_Type; }

		std::string_view TypeToString() const;
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
}
