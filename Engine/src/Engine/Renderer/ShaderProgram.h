#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Renderer/Shader.h"
#include "Engine/Utils/StdUtils.h"

#include <memory>
#include <string_view>
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
#include <glm/mat4x2.hpp>E
#include <glm/mat4x3.hpp>
#include <glm/mat4x4.hpp>

#include <glad/glad.h>

#include "glm/gtc/type_ptr.hpp"

namespace Engine {
#define DEFAULT_SHADER_NAME "Default"
#define DEFAULT_SHADER_PROGRAM_NAME "Shader Program"

	class UniformBuffer;
	class Texture2D;

	class ShaderProgram {
	public:
		using IDType                = uint32_t;
		using UniformLocationType   = int32_t;
		using AttributeLocationType = int32_t;
		using UniformBlockIndexType = uint32_t;

		constexpr static UniformLocationType InvalidUniformLocation     = -1;
		constexpr static AttributeLocationType InvalidAttributeLocation = -1;
		constexpr static UniformBlockIndexType InvalidUniformBlockIndex = GL_INVALID_INDEX;

		struct UniformInfo {
			std::string Name;
			int Size;
			uint32_t Type;
			UniformLocationType Location;
		};

		struct UniformBlockInfo {
			std::string Name;
			uint32_t Size;
			Shader::Type ShaderType;
			UniformBlockIndexType Index;
		};

		enum class ParametersName : uint32_t {
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

		ShaderProgram(const std::string &name = DEFAULT_SHADER_PROGRAM_NAME);
		ShaderProgram(const std::string &name, Ref<Shader> shader);

		template <class... Args>
		ShaderProgram(const std::string &name, Ref<Shader> shader, Args &&... args) : ShaderProgram(name) {
			Attach(shader, std::forward<Args>(args)...);

			Link();

			Detach(shader, std::forward<Args>(args)...);
		}

		operator IDType() const;
		IDType ID() const;

		bool IsLinked() const;

		std::string_view Name() const;

		void Attach(Ref<Shader> shader);
		void Detach(Ref<Shader> shader);

		template <class... Args>
		void Attach(Ref<Shader> shader, Args &&... args) {
			Attach(shader);
			Attach(std::forward<Args>(args)...);
		}

		template <class... Args>
		void Detach(Ref<Shader> shader, Args &&... args) {
			Detach(shader);
			Detach(std::forward<Args>(args)...);
		}

		bool IsAttached(Ref<Shader> shader);
		bool IsAttached(Shader::Type shaderType);

		Ref<Shader> GetShader(Shader::Type shaderType);

		bool Link();
		void Use();

		std::string GetLog() const;

		size_t GetAttachedShaderCount() const;

		AttributeLocationType GetAttributeLocation(std::string_view name) const;
		UniformLocationType GetUniformLocation(std::string_view name) const;
		UniformBlockIndexType GetUniformBlockIndex(std::string_view name);

		bool HasUniform(std::string_view name);

		const std::vector<UniformInfo>& GetActiveUniforms() const;
		const std::vector<UniformBlockInfo>& GetActiveUniformBlocks() const;

		UniformInfo QueryUniform(UniformLocationType location) const;
		UniformBlockInfo QueryUniformBlock(UniformBlockIndexType index) const;

		UniformInfo QueryUniform(std::string_view name) const;
		UniformBlockInfo QueryUniformBlock(std::string_view name) const;

		int Get(ParametersName pName) const;
		void Get(ParametersName pName, int *params);

		void UniformValue(std::string_view name, bool value) {
			UniformValue(GetUniformLocation(name), static_cast<int32_t>(value));
		}

		void UniformValue(std::string_view name, bool value1, bool value2) {
			UniformValue(GetUniformLocation(name), static_cast<int32_t>(value1), static_cast<int32_t>(value2));
		}

		void UniformValue(std::string_view name, bool value1, bool value2, bool value3) {
			UniformValue(
			             GetUniformLocation(name),
			             static_cast<int32_t>(value1),
			             static_cast<int32_t>(value2),
			             static_cast<int32_t>(value3)
			            );
		}

		void UniformValue(std::string_view name, int32_t value) {
			UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(std::string_view name, int32_t value1, int32_t value2) {
			UniformValue(GetUniformLocation(name), value1, value2);
		}

		void UniformValue(std::string_view name, int32_t value1, int32_t value2, int32_t value3) {
			UniformValue(GetUniformLocation(name), value1, value2, value3);
		}

		void UniformValue(std::string_view name, uint32_t value) {
			UniformValue(GetUniformLocation(name), value);
		}

		void UniformValue(std::string_view name, uint32_t value1, uint32_t value2) {
			UniformValue(GetUniformLocation(name), value1, value2);
		}

		void UniformValue(std::string_view name, uint32_t value1, uint32_t value2, uint32_t value3) {
			UniformValue(GetUniformLocation(name), value1, value2, value3);
		}

		void UniformValue(std::string_view name, const glm::vec2 &vec) {
			UniformValue(GetUniformLocation(name), vec);
		}

		void UniformValue(std::string_view name, const glm::vec3 &vec) {
			UniformValue(GetUniformLocation(name), vec);
		}

		void UniformValue(std::string_view name, const glm::vec4 &vec) {
			UniformValue(GetUniformLocation(name), vec);
		}

		void UniformValue(std::string_view name, const glm::mat2x2& mat, bool transpose = false) {
			UniformValue(GetUniformLocation(name), mat, transpose);
		}

		void UniformValue(std::string_view name, const glm::mat2x3& mat, bool transpose = false) {
			UniformValue(GetUniformLocation(name), mat, transpose);
		}

		void UniformValue(std::string_view name, const glm::mat2x4& mat, bool transpose = false) {
			UniformValue(GetUniformLocation(name), mat, transpose);
		}

		void UniformValue(std::string_view name, const glm::mat3x2& mat, bool transpose = false) {
			UniformValue(GetUniformLocation(name), mat, transpose);
		}

		void UniformValue(std::string_view name, const glm::mat3x3& mat, bool transpose = false) {
			UniformValue(GetUniformLocation(name), mat, transpose);
		}

		void UniformValue(std::string_view name, const glm::mat3x4& mat, bool transpose = false) {
			UniformValue(GetUniformLocation(name), mat, transpose);
		}

		void UniformValue(std::string_view name, const glm::mat4x2& mat, bool transpose = false) {
			UniformValue(GetUniformLocation(name), mat, transpose);
		}

		void UniformValue(std::string_view name, const glm::mat4x3& mat, bool transpose = false) {
			UniformValue(GetUniformLocation(name), mat, transpose);
		}

		void UniformValue(std::string_view name, const glm::mat4x4& mat, bool transpose = false) {
			UniformValue(GetUniformLocation(name), mat, transpose);
		}

		void UniformValue(std::string_view name, Ref<Texture2D> texture, uint32_t sampleUnit = 0) {
			UniformValue(GetUniformLocation(name), texture, sampleUnit);
		}

		void UniformValue(UniformLocationType location, bool value) {
			UniformValue(location, static_cast<int32_t>(value));
		}

		void UniformValue(UniformLocationType location, bool value1, bool value2) {
			UniformValue(location, static_cast<int32_t>(value1), static_cast<int32_t>(value2));
		}

		void UniformValue(UniformLocationType location, bool value1, bool value2, bool value3) {
			UniformValue(
			             location,
			             static_cast<int32_t>(value1),
			             static_cast<int32_t>(value2),
			             static_cast<int32_t>(value3)
			            );
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

		void UniformValue(UniformLocationType location, double value);
		void UniformValue(UniformLocationType location, double value1, double value2);
		void UniformValue(UniformLocationType location, double value1, double value2, double value3);

		void UniformValue(UniformLocationType location, const glm::vec2 &vec);
		void UniformValue(UniformLocationType location, const glm::vec3 &vec);
		void UniformValue(UniformLocationType location, const glm::vec4 &vec);

		void UniformValue(UniformLocationType location, const glm::mat2x2 &mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat2x3 &mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat2x4 &mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat3x2 &mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat3x3 &mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat3x4 &mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat4x2 &mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat4x3 &mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat4x4 &mat, bool transpose = false);

		void UniformValue(UniformLocationType location, Ref<Texture2D> texture, uint32_t sampleUnit = 0);

		void BindUniformBuffer(UniformLocationType location, const UniformBuffer &buffer);
		void BindUniformBuffer(UniformLocationType location, const UniformBuffer &buffer, size_t size, size_t offset);

	private:
		class Internals {
		public:
			IDType Program = 0;

			std::unordered_map<std::string, UniformLocationType, TransparentStringHash, std::equal_to<>> UniformLocations;
			std::unordered_map<std::string, AttributeLocationType, TransparentStringHash, std::equal_to<>> Attributes;
			std::unordered_map<std::string, UniformBlockIndexType, TransparentStringHash, std::equal_to<>> UniformBlockIndices;

			std::vector<UniformInfo> ActiveUniforms;
			std::vector<UniformBlockInfo> ActiveUniformBlocks;

			std::unordered_set<Ref<Shader>> Shaders;

			bool Linked = false;
			bool Changed = true;

			std::string Name;

			Internals(const std::string &name);
			Internals(IDType id, const std::string &name);
			~Internals();

			void Attach(Ref<Shader> shader);
			void Detach(Ref<Shader> shader);

			bool Link();

			void Use();

			std::string GetLog();

			AttributeLocationType GetAttributeLocation(std::string_view name);
			UniformLocationType GetUniformLocation(std::string_view name);
			UniformBlockIndexType GetUniformBlockIndex(std::string_view name);

			int Get(ParametersName pName);
			void Get(ParametersName pName, int *params);
		private:
			void Populate();
			void PopulateUniformBlocks();
			void PopulateUniforms();

			int GetActiveUniformI(uint32_t index, GLenum pName);
			int GetActiveUniformBlockI(uint32_t index, GLenum pName);
			std::string GetActiveUniformBlockName(uint32_t index);

			UniformBlockInfo QueryUniformBlock(uint32_t index);
			UniformInfo QueryUniform(uint32_t index);

			template<class STDContainer, typename GlFunction, typename LocationType>
			auto GetLocation(std::string_view name, STDContainer &container, GlFunction glFunction, LocationType invalidNumber) {
				const auto iter = container.find(name);
				LocationType location = invalidNumber;

				if (iter != std::end(container))
					location = iter->second;
				else
					location = container.emplace(std::string(name), glFunction(Program, name.data())).first->second;

				return location;
			}
		};

		Ref<Internals> m_Internals;

		template<typename MatrixType>
		void UniformMatrix(UniformLocationType location, MatrixType matrix, bool transpose) {
			if (location == InvalidUniformLocation)
				return;

			if constexpr (std::is_same_v<MatrixType, glm::mat2x2>)
				glUniformMatrix2fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
			if constexpr (std::is_same_v<MatrixType, glm::mat2x3>)
				glUniformMatrix2x3fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
			if constexpr (std::is_same_v<MatrixType, glm::mat2x4>)
				glUniformMatrix2x4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));

			if constexpr (std::is_same_v<MatrixType, glm::mat3x2>)
				glUniformMatrix3x2fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
			if constexpr (std::is_same_v<MatrixType, glm::mat3x3>)
				glUniformMatrix3fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
			if constexpr (std::is_same_v<MatrixType, glm::mat3x4>)
				glUniformMatrix3x4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));

			if constexpr (std::is_same_v<MatrixType, glm::mat4x2>)
				glUniformMatrix4x2fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
			if constexpr (std::is_same_v<MatrixType, glm::mat4x3>)
				glUniformMatrix4x3fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
			if constexpr (std::is_same_v<MatrixType, glm::mat4x4>)
				glUniformMatrix4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
		}

		template <typename VectorType>
		void UniformVector(UniformLocationType location, const VectorType &vector) {
			if (location == InvalidUniformLocation)
				return;

			if constexpr (std::is_same_v<VectorType, glm::vec2>)
				glUniform2fv(location, 1, &vector.x);
			if constexpr (std::is_same_v<VectorType, glm::vec3>)
				glUniform3fv(location, 1, &vector.x);
			if constexpr (std::is_same_v<VectorType, glm::vec4>)
				glUniform4fv(location, 1, &vector.x);
		}

		template <typename ValueType>
		void UniformValue1(UniformLocationType location, ValueType value) {
			if (location == InvalidUniformLocation)
				return;

			if constexpr (std::is_same_v<ValueType, int32_t>)
				glUniform1i(location, value);
			if constexpr (std::is_same_v<ValueType, uint32_t>)
				glUniform1ui(location, value);
			if constexpr (std::is_same_v<ValueType, float>)
				glUniform1f(location, value);
			if constexpr (std::is_same_v<ValueType, double>)
				glUniform1d(location, value);
		}

		template <typename ValueType>
		void UniformValue2(UniformLocationType location, ValueType value1, ValueType value2) {
			if (location == InvalidUniformLocation)
				return;

			if constexpr (std::is_same_v<ValueType, int32_t>)
				glUniform2i(location, value1, value2);
			if constexpr (std::is_same_v<ValueType, uint32_t>)
				glUniform2ui(location, value1, value2);
			if constexpr (std::is_same_v<ValueType, float>)
				glUniform2f(location, value1, value2);
			if constexpr (std::is_same_v<ValueType, double>)
				glUniform2d(location, value1, value2);
		}

		template <typename ValueType>
		void UniformValue3(UniformLocationType location, ValueType value1, ValueType value2, ValueType value3) {
			if (location == InvalidUniformLocation)
				return;

			if constexpr (std::is_same_v<ValueType, int32_t>)
				glUniform3i(location, value1, value2, value3);
			if constexpr (std::is_same_v<ValueType, uint32_t>)
				glUniform3ui(location, value1, value2, value3);
			if constexpr (std::is_same_v<ValueType, float>)
				glUniform3f(location, value1, value2, value3);
			if constexpr (std::is_same_v<ValueType, double>)
				glUniform3d(location, value1, value2, value3);
			
		}
	};
}
