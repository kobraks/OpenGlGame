#pragma once
#include <memory>
#include <unordered_map>
#include <optional>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/mat2x2.hpp>
#include <glm/mat2x3.hpp>
#include <glm/mat2x4.hpp>
#include <glm/mat3x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x2.hpp>
#include <glm/mat4x3.hpp>
#include <glm/mat4x4.hpp>

#include "Types.h"
#include "Shader.h"
#include "Texture.h"

namespace Game
{
#define DEFAULT_PROGRAM_SHADER_NAME "Default"

	class ShaderProgram
	{
	public:
		using IdType = uint32_t;
		using UniformLocationType = int32_t;
		using AttributeLocationType = int32_t;

		constexpr static UniformLocationType INVALID_UNIFORM_LOCATION     = -1;
		constexpr static AttributeLocationType INVALID_ATTRIBUTE_LOCATION = -1;

	private:
		mutable std::unordered_map<std::string, int32_t> m_UniformsLocation;
		mutable std::unordered_map<std::string, int> m_Attributes;

		Pointer<IdType> m_Program = nullptr;

		bool m_Linked          = false;
		mutable bool m_Changed = true;
		std::string m_Name;

	public:
		explicit ShaderProgram(const std::string &name);
		ShaderProgram(IdType id, const std::string &name);
		ShaderProgram(const std::string &name, const Shader &shader);

		template <class ...Args>
		ShaderProgram(const std::string &name, const Shader &shader, Args &&... args) : ShaderProgram(name)
		{
			Attach(shader, std::forward<Args>(args)...);

			Link();

			Detach(shader, std::forward<Args>(args)...);
		}

		operator IdType() const { return *m_Program; }
		IdType ID() const { return *m_Program; }

		bool IsLinked() const { return m_Linked; }

		const std::string Name() const { return m_Name; };

		void Attach(const Shader &shader);
		void Detach(const Shader &shader);

		bool Link();

		void Use() const;

		std::string GetLog() const;
		AttributeLocationType GetAttributeLocation(const std::string &name) const;
		UniformLocationType GetUniformLocation(const std::string &name) const;

		void UniformValue(const std::string &name, int32_t value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, int32_t value, int32_t value2)
		{
			return UniformValue(GetUniformLocation(name), value, value2);
		}

		void UniformValue(const std::string &name, int32_t value, int32_t value2, int32_t value3)
		{
			return UniformValue(GetUniformLocation(name), value, value2, value3);
		}

		void UniformValue(const std::string &name, uint32_t value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, uint32_t value, uint32_t value2)
		{
			return UniformValue(GetUniformLocation(name), value, value2);
		}

		void UniformValue(const std::string &name, uint32_t value, uint32_t value2, uint32_t value3)
		{
			return UniformValue(GetUniformLocation(name), value, value2, value3);
		}

		void UniformValue(const std::string &name, float value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, float value, float value2)
		{
			return UniformValue(GetUniformLocation(name), value, value2);
		}

		void UniformValue(const std::string &name, float value, float value2, float value3)
		{
			return UniformValue(GetUniformLocation(name), value, value3);
		}

		void UniformValue(const std::string &name, glm::vec2 value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, glm::vec3 value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, glm::vec4 value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, glm::mat2x2 value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, glm::mat2x3 value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, glm::mat2x4 value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, glm::mat3x2 value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, glm::mat3x3 value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, glm::mat3x4 value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, glm::mat4x2 value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, glm::mat4x3 value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, glm::mat4x4 value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, const Texture &texture, int32_t sampleUnit = 0)
		{
			return UniformValue(GetUniformLocation(name), texture, sampleUnit);
		}

		void UniformValue(UniformLocationType location, int32_t value);
		void UniformValue(UniformLocationType location, int32_t value, int32_t value2);
		void UniformValue(UniformLocationType location, int32_t value, int32_t value2, int32_t value3);

		void UniformValue(UniformLocationType location, uint32_t value);
		void UniformValue(UniformLocationType location, uint32_t value, uint32_t value2);
		void UniformValue(UniformLocationType location, uint32_t value, uint32_t value2, uint32_t value3);

		void UniformValue(UniformLocationType location, float value);
		void UniformValue(UniformLocationType location, float value, float value2);
		void UniformValue(UniformLocationType location, float value, float value2, float value3);

		void UniformValue(UniformLocationType location, glm::vec2 value);
		void UniformValue(UniformLocationType location, glm::vec3 value);
		void UniformValue(UniformLocationType location, glm::vec4 value);

		void UniformValue(UniformLocationType location, glm::mat2x2 value);
		void UniformValue(UniformLocationType location, glm::mat2x3 value);
		void UniformValue(UniformLocationType location, glm::mat2x4 value);
		void UniformValue(UniformLocationType location, glm::mat3x2 value);
		void UniformValue(UniformLocationType location, glm::mat3x3 value);
		void UniformValue(UniformLocationType location, glm::mat3x4 value);
		void UniformValue(UniformLocationType location, glm::mat4x2 value);
		void UniformValue(UniformLocationType location, glm::mat4x3 value);
		void UniformValue(UniformLocationType location, glm::mat4x4 value);

		void UniformValue(UniformLocationType location, const Texture &texture, int32_t sampleUnit = 0);

		bool HasUniform(const std::string &name) const;

		static ShaderProgram* GetDefault();

	private:
		void Populate();

	public:
		template <class ...Args>
		void Attach(const Shader &shader, Args &&... args)
		{
			Attach(shader);
			Attach(std::forward<Args>(args)...);
		}

		template <class ...Args>
		void Detach(const Shader &shader, Args &&... args)
		{
			Detach(shader);
			Detach(std::forward<Args>(args)...);
		}
	};
}
