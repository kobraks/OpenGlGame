#pragma once
#include <memory>
#include <unordered_map>
#include <unordered_set>

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
		enum class ParametersName : uint32_t
		{
			DeleteStatus =  GL_DELETE_STATUS,
			LinkStatus = GL_LINK_STATUS,
			ValidateStatus = GL_VALIDATE_STATUS,
			InfoLogLength = GL_INFO_LOG_LENGTH,
			AttachedShaders = GL_ATTACHED_SHADERS,
			ActiveAtomicCounterBuffers = GL_ACTIVE_ATOMIC_COUNTER_BUFFERS,
			ActiveAttributes = GL_ACTIVE_ATTRIBUTES,
			ActiveAttributesMaxLength = GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
			ActiveUniforms = GL_ACTIVE_UNIFORMS,
			ActiveUniformBlocks = GL_ACTIVE_UNIFORM_BLOCKS,
			ActiveUniformBlockMaxNameLength = GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH,
			ActiveUniformMaxLength = GL_ACTIVE_UNIFORM_MAX_LENGTH,
			ComputeWorkGroupSize = GL_COMPUTE_WORK_GROUP_SIZE,
			BinaryLength = GL_PROGRAM_BINARY_LENGTH,
			TransformFeedbackBufferMode = GL_TRANSFORM_FEEDBACK_BUFFER_MODE,
			TransformFeedbackVaryings = GL_TRANSFORM_FEEDBACK_VARYINGS,
			TransformFeedbackVaryingMaxLength = GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH,
			GeometryVerticesOut = GL_GEOMETRY_VERTICES_OUT,
			GeometryInputType = GL_GEOMETRY_INPUT_TYPE,
			GeomteryOutputType = GL_GEOMETRY_OUTPUT_TYPE
		};
		
		class Internals
		{
		public:
			IdType Program = 0;
			
			std::unordered_map<std::string, UniformLocationType> UniformsLocation;
			std::unordered_map<std::string, AttributeLocationType> Attributes;

			std::unordered_set<Pointer<Shader>> Shaders;

			bool Linked = false;
			bool Changed = true;

			std::string Name;

			Internals(std::string name);
			Internals(IdType id, std::string name);
			~Internals();
		};
		
		Pointer<Internals> m_Internals = nullptr;
	public:
		explicit ShaderProgram(const std::string &name);
		ShaderProgram(IdType id, const std::string &name);
		ShaderProgram(const std::string &name, Pointer<Shader> shader);

		template <class ...Args>
		ShaderProgram(const std::string &name, Pointer<Shader> shader, Args &&... args) : ShaderProgram(name)
		{
			Attach(shader, std::forward<Args>(args)...);

			Link();

			Detach(shader, std::forward<Args>(args)...);
		}

		operator IdType() const { return m_Internals->Program; }
		IdType ID() const { return m_Internals->Program; }

		bool IsLinked() const { return m_Internals->Linked; }

		const std::string Name() const { return m_Internals->Name; };

		void Attach(Pointer<Shader> shader);
		void Detach(Pointer<Shader> shader);

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

		int GetProgramI(ParametersName name) const;
		void GetProgramIV(ParametersName name, int* params) const;
	public:
		template <class ...Args>
		void Attach(Pointer<Shader> shader, Args &&... args)
		{
			Attach(shader);
			Attach(std::forward<Args>(args)...);
		}

		template <class ...Args>
		void Detach(Pointer<Shader> shader, Args &&... args)
		{
			Detach(shader);
			Detach(std::forward<Args>(args)...);
		}
	};
}