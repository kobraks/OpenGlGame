#pragma once
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
#include "GLEnums.h"

namespace Game
{
#define DEFAULT_PROGRAM_SHADER_NAME "Default"
#define PROGRAM_SHADER_NAME "Shader Program"

	class UniformBuffer;
	class Texture;

	class ShaderProgram
	{
	public:
		using IdType = uint32_t;
		using UniformLocationType = int32_t;
		using AttributeLocationType = int32_t;
		using UniformBlockIndexType = uint32_t;

		constexpr static UniformLocationType INVALID_UNIFORM_LOCATION      = -1;
		constexpr static AttributeLocationType INVALID_ATTRIBUTE_LOCATION  = -1;
		constexpr static UniformBlockIndexType INVALID_UNIFORM_BLOCK_INDEX = GL_INVALID_INDEX;

	private:
		struct UniformInfo
		{
			std::string Name;
			int Size;
			UniformType Type;
			UniformLocationType Location;
		};

		struct UniformBlockInfo
		{
			std::string Name;
			uint32_t Size;
			Shader::Type ShaderType;
			UniformBlockIndexType Index;
		};

		enum class ParametersName : uint32_t
		{
			DeleteStatus = GL_DELETE_STATUS,
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

			mutable std::unordered_map<std::string, UniformLocationType> UniformsLocation;
			mutable std::unordered_map<std::string, AttributeLocationType> Attributes;
			mutable std::unordered_map<std::string, UniformBlockIndexType> UniformBlocksIndex;
			std::vector<UniformInfo> ActiveUniforms;
			std::vector<UniformBlockInfo> ActiveUniformBlocks;

			std::unordered_set<Pointer<Shader>> Shaders;

			bool Linked  = false;
			bool Changed = true;

			std::string Name;

			Internals(std::string name);
			Internals(IdType id, std::string name);
			~Internals();

			void Attach(Pointer<Shader> shader);
			void Detach(Pointer<Shader> shader);

			bool IsAttached(const Pointer<Shader> &shader) const;
			bool IsAttached(Shader::Type type);

			bool Link();

			void Use();

			std::string GetLog() const;

			bool HasUniform(const std::string &name) const;

			AttributeLocationType GetAttributeLocation(const std::string &name) const;
			UniformLocationType GetUniformLocation(const std::string &name) const;
			UniformBlockIndexType GetUniformBlockIndex(const std::string &name) const;

			const std::vector<UniformInfo>& GetActiveUniforms() const { return ActiveUniforms; }

			int Get(ParametersName name) const;
			void Get(ParametersName name, int *params) const;

			void Populate();
			void PopulateUniformBlocks();
			void PopulateUniforms();

			int GetActiveUniformI(uint32_t index, GLenum pName) const;
			int GetActiveUniformBlockI(uint32_t index, GLenum pName) const;
			std::string GetActiveUniformBlockName(uint32_t index) const;

			UniformBlockInfo QueryUniformBlock(uint32_t index) const;
			UniformInfo QueryUniform(uint32_t index) const;
		};

		Pointer<Internals> m_Internals = nullptr;
	public:
		explicit ShaderProgram(const std::string &name = PROGRAM_SHADER_NAME);
		ShaderProgram(IdType id, const std::string &name = PROGRAM_SHADER_NAME);
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

		void Attach(Pointer<Shader> shader) { return m_Internals->Attach(shader); }
		void Detach(Pointer<Shader> shader) { return m_Internals->Detach(shader); }

		bool IsAttached(const Pointer<Shader> &shader) const { return m_Internals->IsAttached(shader); }
		bool IsAttached(Shader::Type type) const { return m_Internals->IsAttached(type); }

		Pointer<Shader> GetShader(Shader::Type type);

		bool Link() { return m_Internals->Link(); }

		void Use() const { m_Internals->Use(); }

		std::string GetLog() const { return m_Internals->GetLog(); }

		size_t GetAttachedShaderCount() const { return m_Internals->Shaders.size(); }

		AttributeLocationType GetAttributeLocation(const std::string &name) const { return m_Internals->GetAttributeLocation(name); }
		UniformLocationType GetUniformLocation(const std::string &name) const { return m_Internals->GetUniformLocation(name); }
		UniformBlockIndexType GetUniformBlockIndex(const std::string &name) const { return m_Internals->GetUniformBlockIndex(name); }

		bool HasUniform(const std::string &name) const { return m_Internals->HasUniform(name); }

		static ShaderProgram* GetDefault();

		int Get(ParametersName name) const { return m_Internals->Get(name); }
		void Get(ParametersName name, int *params) const { return m_Internals->Get(name, params); }

		std::unordered_set<Pointer<Shader>>::iterator begin() { return m_Internals->Shaders.begin(); }
		std::unordered_set<Pointer<Shader>>::iterator end() { return m_Internals->Shaders.end(); }

		std::unordered_set<Pointer<Shader>>::const_iterator begin() const { return m_Internals->Shaders.begin(); }
		std::unordered_set<Pointer<Shader>>::const_iterator end() const { return m_Internals->Shaders.end(); }

		const std::vector<UniformInfo>& GetActiveUniforms() const;
		const std::vector<UniformBlockInfo>& GetActiveUniformBlocks() const { return m_Internals->ActiveUniformBlocks; }

		size_t GetActiveUniformCount() const { return m_Internals->ActiveUniforms.size(); }
		size_t GetActiveUniformBlockCount() const { return m_Internals->ActiveUniformBlocks.size(); }

		UniformInfo QueryUniform(UniformLocationType location) const;
		UniformBlockInfo QueryUniformBlock(UniformBlockIndexType index) const;

		UniformInfo QueryUniform(const std::string &name) const;
		UniformBlockInfo QueryUniformBlock(const std::string &name) const;
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

		void UniformValue(const std::string &name, bool value)
		{
			return UniformValue(GetUniformLocation(name), static_cast<int32_t>(value));
		}

		void UniformValue(const std::string &name, bool value1, bool value2)
		{
			return UniformValue(GetUniformLocation(name), static_cast<int32_t>(value1), static_cast<int32_t>(value2));
		}

		void UniformValue(const std::string &name, bool value1, bool value2, bool value3)
		{
			return UniformValue(GetUniformLocation(name), static_cast<int32_t>(value1), static_cast<int32_t>(value2), static_cast<int32_t>(value3));
		}

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

		void UniformValue(const std::string &name, const glm::vec2 &value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, const glm::vec3 &value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, const glm::vec4 &value)
		{
			return UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(const std::string &name, const glm::mat2x2 &value, bool transpose = false)
		{
			return UniformValue(GetUniformLocation(name), value, transpose);
		}

		void UniformValue(const std::string &name, const glm::mat2x3 &value, bool transpose = false)
		{
			return UniformValue(GetUniformLocation(name), value, transpose);
		}

		void UniformValue(const std::string &name, const glm::mat2x4 &value, bool transpose = false)
		{
			return UniformValue(GetUniformLocation(name), value, transpose);
		}

		void UniformValue(const std::string &name, const glm::mat3x2 &value, bool transpose = false)
		{
			return UniformValue(GetUniformLocation(name), value, transpose);
		}

		void UniformValue(const std::string &name, const glm::mat3x3 &value, bool transpose = false)
		{
			return UniformValue(GetUniformLocation(name), value, transpose);
		}

		void UniformValue(const std::string &name, const glm::mat3x4 &value, bool transpose = false)
		{
			return UniformValue(GetUniformLocation(name), value, transpose);
		}

		void UniformValue(const std::string &name, const glm::mat4x2 &value, bool transpose = false)
		{
			return UniformValue(GetUniformLocation(name), value, transpose);
		}

		void UniformValue(const std::string &name, const glm::mat4x3 &value, bool transpose = false)
		{
			return UniformValue(GetUniformLocation(name), value, transpose);
		}

		void UniformValue(const std::string &name, const glm::mat4x4 &value, bool transpose = false)
		{
			return UniformValue(GetUniformLocation(name), value, transpose);
		}

		void UniformValue(const std::string &name, const Texture &texture, int32_t sampleUnit = 0)
		{
			return UniformValue(GetUniformLocation(name), texture, sampleUnit);
		}

		void BindUniformBuffer(const std::string &name, const UniformBuffer &buffer)
		{
			return BindUniformBuffer(GetUniformBlockIndex(name), buffer);
		}
		void BindUniformBuffer(const std::string &name, const UniformBuffer &buffer, size_t size, size_t offset)
		{
			return BindUniformBuffer(GetUniformBlockIndex(name), buffer, size, offset);
		}
		void UniformValue(UniformLocationType location, bool value)
		{
			return UniformValue(location, static_cast<int32_t>(value));
		}

		void UniformValue(UniformLocationType location, bool value1, bool value2)
		{
			UniformValue(location, static_cast<int32_t>(value1), static_cast<int32_t>(value2));
		}

		void UniformValue(UniformLocationType location, bool value1, bool value2, bool value3)
		{
			UniformValue(location, static_cast<int32_t>(value1), static_cast<int32_t>(value2), static_cast<int32_t>(value3));
		}

		void UniformValue(UniformLocationType location, int32_t value);
		void UniformValue(UniformLocationType location, int32_t value1, int32_t value2);
		void UniformValue(UniformLocationType location, int32_t value1, int32_t value2, int32_t value3);

		void UniformValue(UniformLocationType location, uint32_t value);
		void UniformValue(UniformLocationType location, uint32_t value1, uint32_t value2);
		void UniformValue(UniformLocationType location, uint32_t value1, uint32_t value2, uint32_t value3);

		void UniformValue(UniformLocationType location, float value);
		void UniformValue(UniformLocationType location, float value1, float value2);
		void UniformValue(UniformLocationType location, float value1, float value2, float value3);

		void UniformValue(UniformLocationType location, const glm::vec2 &value);
		void UniformValue(UniformLocationType location, const glm::vec3 &value);
		void UniformValue(UniformLocationType location, const glm::vec4 &value);

		void UniformValue(UniformLocationType location, const glm::mat2x2 &value, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat2x3 &value, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat2x4 &value, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat3x2 &value, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat3x3 &value, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat3x4 &value, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat4x2 &value, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat4x3 &value, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat4x4 &value, bool transpose = false);

		void UniformValue(UniformLocationType location, const Texture &texture, int32_t sampleUnit = 0);

		void BindUniformBuffer(UniformBlockIndexType index, const UniformBuffer &buffer);
		void BindUniformBuffer(UniformBlockIndexType index, const UniformBuffer &buffer, size_t size, size_t offset);
	};
}
