#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Renderer/ShaderStage.h"
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
#include <glm/mat4x2.hpp>
#include <glm/mat4x3.hpp>
#include <glm/mat4x4.hpp>

#include "glm/gtc/type_ptr.hpp"

namespace Engine {
	class UniformBuffer;
	class Texture;

	struct ShaderLinkResult {
		bool Success = false;
		std::string LogMessage;

		explicit operator bool() const { return Success; }
	};

	struct ShaderValidationResult {
		bool Valid = false;
		std::string LogMessage;

		explicit operator bool() const { return Valid; }
	};

	class ShaderProgram {
	public:
		using IDType = uint32_t;
		using UniformLocationType = int32_t;
		using AttributeLocationType = int32_t;
		using UniformBlockIndexType = uint32_t;

		constexpr static UniformLocationType InvalidUniformLocation = -1;
		constexpr static AttributeLocationType InvalidAttributeLocation = -1;
		constexpr static UniformBlockIndexType InvalidUniformBlockIndex = 0xFFFFFFFF;

		struct UniformInfo {
			std::string Name;
			int Size;
			uint32_t Type;
			UniformLocationType Location;
		};

		struct UniformBlockInfo {
			std::string Name;
			uint32_t Size;
			ShaderStage::Type ShaderType;
			UniformBlockIndexType Index;
		};

		struct Reflection {
			std::vector<UniformInfo> Uniforms;
			std::vector<UniformBlockInfo> Blocks;
			ShaderStage::Type UsedStages = ShaderStage::Type::None;
		};

		static Ref<ShaderProgram> Create(const std::string& label = {});
		static Ref<ShaderProgram> Create(std::initializer_list<Ref<ShaderStage>> stages, const std::string& label = {});

		static bool SaveBinary(Ref<ShaderProgram> program, std::filesystem::path path);
		static Ref<ShaderProgram> LoadBinary(std::filesystem::path path);

		operator IDType() const { return m_GLState->Program;  }
		IDType ID() const { return m_GLState->Program; }

		bool IsLinked() const { return m_GLState->Linked; }
		std::string_view Label() const { return m_GLState->Label; }

		void SetLabel(const std::string& label);

		void Attach(Ref<ShaderStage> stage);
		void Attach(std::initializer_list<Ref<ShaderStage>> stages);

		void Detach(Ref<ShaderStage> stage);
		void Detach(std::initializer_list<Ref<ShaderStage>> stages);

		void DetachAll();

		bool IsAttached(Ref<ShaderStage> stage);
		bool IsAttached(ShaderStage::Type type);

		ShaderLinkResult Link();
		void Use() const;

		ShaderValidationResult Validate();

		std::string_view Log() const { return m_GLState->LogMessage; }

		uint64_t GetAttachedStagesCount() const { return m_GLState->Stages.size(); }

		AttributeLocationType GetAttributeLocation(std::string_view name) const;
		UniformLocationType GetUniformLocation(std::string_view name) const;
		UniformBlockIndexType GetUniformBlockIndex(std::string_view name) const;

		bool HasUniform(std::string_view name) const;

		const std::vector<UniformInfo>& GetActiveUniforms() const;
		const std::vector<UniformBlockInfo>& GetActiveUniformBlocks() const;

		UniformInfo QueryUniform(UniformLocationType location) const;
		UniformBlockInfo QueryUniformBlock(UniformBlockIndexType index) const;

		UniformInfo QueryUniform(std::string_view name) const;
		UniformBlockInfo QueryUniformBlock(std::string_view name) const;

		const Reflection& Reflect() const { return m_GLState->Reflection; }

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

		void UniformValue(std::string_view name, const glm::vec2& vec) {
			UniformValue(GetUniformLocation(name), vec);
		}

		void UniformValue(std::string_view name, const glm::vec3& vec) {
			UniformValue(GetUniformLocation(name), vec);
		}

		void UniformValue(std::string_view name, const glm::vec4& vec) {
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

		void UniformValue(std::string_view name, Ref<Texture> texture, uint32_t samplerUnit = 0) {
			UniformValue(GetUniformLocation(name), texture, samplerUnit);
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

		void UniformValue(UniformLocationType location, const glm::vec2& vec);
		void UniformValue(UniformLocationType location, const glm::vec3& vec);
		void UniformValue(UniformLocationType location, const glm::vec4& vec);

		void UniformValue(UniformLocationType location, const glm::mat2x2& mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat2x3& mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat2x4& mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat3x2& mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat3x3& mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat3x4& mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat4x2& mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat4x3& mat, bool transpose = false);
		void UniformValue(UniformLocationType location, const glm::mat4x4& mat, bool transpose = false);

		void UniformValue(UniformLocationType location, Ref<Texture> texture, uint32_t samplerUnit = 0);

		void BindUniformBuffer(UniformLocationType location, const UniformBuffer& buffer);
		void BindUniformBuffer(UniformLocationType location, const UniformBuffer& buffer, size_t size, size_t offset);
		
	private:
		ShaderProgram();

		int GetParameter(uint32_t pName);
		void GetParameter(uint32_t pName, int* params);

		void FetchLog();

		bool CheckIfLinked();

		void Populate();
		void PopulateUniformBlocks();
		void PopulateUniforms();

		int GetActiveUniformI(uint32_t index, uint32_t pName);
		int GetActiveUniformBlockI(uint32_t index, uint32_t pName);
		std::string GetActiveUniformBlockName(uint32_t index);

		UniformBlockInfo QueryUniformBlock(uint32_t index);
		UniformInfo QueryUniform(uint32_t index);

		template<class STDContainer, typename GlFunction, typename LocationType>
		auto GetLocation(std::string_view name, STDContainer& container, GlFunction glFunction, LocationType invalidNumber) const;

		template<typename MatrixType, typename GLFunction>
		void UniformMatrix(UniformLocationType location, MatrixType matrix, uint8_t transpose, GLFunction function) const;

		template<typename VectorType, typename GLFunction>
		void UniformVector(UniformLocationType location, const VectorType& vector, GLFunction function);

		struct GLState {
			IDType Program;

			std::unordered_map<std::string, UniformLocationType, TransparentStringHash, std::equal_to<>> UniformLocations;
			std::unordered_map<std::string, AttributeLocationType, TransparentStringHash, std::equal_to<>> Attributes;
			std::unordered_map<std::string, UniformBlockIndexType, TransparentStringHash, std::equal_to<>> UniformBlockIndices;

			Reflection Reflection;

			std::unordered_set<Ref<ShaderStage>> Stages;

			bool Linked = false;
			bool Changed = true;

			std::string Label;
			std::string LogMessage;

			GLState();
			~GLState();
		};

		Ref<GLState> m_GLState;
	};

	template <class STDContainer, typename GlFunction, typename LocationType>
	auto ShaderProgram::GetLocation(std::string_view name, STDContainer& container, GlFunction glFunction,
		LocationType invalidNumber) const {
		const auto iter = container.find(name);
		LocationType location = invalidNumber;

		if (iter != std::end(container))
			location = iter->second;
		else
			location = container.emplace(std::string(name), glFunction(*this, name.data())).first->second;

		return location;
	}

	template <typename MatrixType, typename GLFunction>
	void ShaderProgram::UniformMatrix(UniformLocationType location, MatrixType matrix, uint8_t transpose,
		GLFunction function) const {
		if (location == InvalidUniformLocation)
			return;

		function(location, 1, transpose, glm::value_ptr(matrix));
	}

	template <typename VectorType, typename GLFunction>
	void ShaderProgram::UniformVector(UniformLocationType location, const VectorType& vector, GLFunction function) {
		if (location == InvalidUniformLocation)
			return;

		function(location, 1, &vector.x);
	}
}
