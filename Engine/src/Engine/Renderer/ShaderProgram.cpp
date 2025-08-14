#include "pch.h"
#include "ShaderProgram.h"

#include "Engine/Renderer/Texture.h"
#include "glad/glad.h"

#include <boost/algorithm/string.hpp>

namespace Engine {
	namespace Utils {
		constexpr bool HasStage(ShaderStage::Type mask, ShaderStage::Type test) {
			return (mask & test) != ShaderStage::Type::None;
		}

		constexpr void RemoveStage(ShaderStage::Type& mask, ShaderStage::Type stage) {
			mask &= ~stage;
		}
	}

	Ref<ShaderProgram> ShaderProgram::Create(const std::string& label) {
		auto program = Ref<ShaderProgram>(new ShaderProgram());
		program->SetLabel(label);

		return program;
	}

	Ref<ShaderProgram> ShaderProgram::Create(std::initializer_list<Ref<ShaderStage>> stages, const std::string& label) {
		auto program = Ref<ShaderProgram>(new ShaderProgram());

		program->SetLabel(label);
		program->Attach(stages);
		program->Link();

		return program;
	}

	bool ShaderProgram::SaveBinary(Ref<ShaderProgram> program, std::filesystem::path path) {
		GLint formats = 0;

		glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
		if (formats == 0) {
			LOG_WARN("Program binary formats not supported on this GPU");
			return false;
		}

		const GLint length = program->GetParameter(GL_PROGRAM_BINARY_LENGTH);
		if (length == 0) {
			LOG_GL_WARN("Cannot save shader binary: no binary data available");
			return false;
		}

		std::vector<std::byte> binary(length);
		GLenum format = 0;

		glGetProgramBinary(static_cast<IDType>(*program), length, nullptr, &format, binary.data());

		std::ofstream out(path, std::ios::binary);
		if (!out) {
			LOG_GL_ERROR("Failed to open file for shader binary: '{}'", path.string());
			return false;
		}

		const uint64_t labelLength = program->Label().size();
		out.write(reinterpret_cast<const char*>(&labelLength), sizeof(labelLength));
		out.write(program->Label().data(), static_cast<std::streamsize>(labelLength));

		out.write(reinterpret_cast<const char*>(&length), sizeof(length));
		out.write(reinterpret_cast<const char*>(&format), sizeof(format));
		out.write(reinterpret_cast<const char*>(binary.data()), static_cast<std::streamsize>(binary.size()));

		return true;
	}

	Ref<ShaderProgram> ShaderProgram::LoadBinary(std::filesystem::path path) {
		std::ifstream in(path, std::ios::binary);
		if (!in) {
			LOG_GL_ERROR("Failed to open shader binary file: '{}'", path.string());
			return nullptr;
		}

		uint64_t labelLength = 0;
		in.read(reinterpret_cast<char*>(&labelLength), sizeof(labelLength));

		std::string label(labelLength, 0);
		in.read(label.data(), static_cast<std::streamsize>(labelLength));

		GLint length = 0;
		in.read(reinterpret_cast<char*>(&length), sizeof(length));

		GLenum format = 0;
		in.read(reinterpret_cast<char*>(&format), sizeof(format));

		if (length == 0) {
			LOG_GL_ERROR("Shader binary file is empty: '{}'", path.string());
			return nullptr;
		}

		std::vector<std::byte> binary(length);
		in.read(reinterpret_cast<char*>(binary.data()), static_cast<std::streamsize>(binary.size()));

		Ref<ShaderProgram> program = Ref<ShaderProgram>(new ShaderProgram());
		program->SetLabel(label);
		glProgramBinary(static_cast<IDType>(*program), format, binary.data(), static_cast<GLsizei>(binary.size()));

		if (!program->CheckIfLinked()) {
			return nullptr;
		}
		program->Populate();

		return program;
	}

	void ShaderProgram::SetLabel(const std::string& label) {
		if (label.empty())
			return;

		m_GLState->Label = label;
		glObjectLabel(GL_PROGRAM, static_cast<IDType>(*this), -1, label.c_str());
	}

	void ShaderProgram::Attach(Ref<ShaderStage> stage) {
		if (Utils::HasStage(m_GLState->Reflection.UsedStages, stage->GetType()) || m_GLState->Stages.contains(stage)) {
			LOG_GL_WARN("Trying to attach attached stage");
			return;
		}

		LOG_GL_DEBUG(
			"Attaching {} shader {} to shader program {} (id: {})",
			stage->TypeToString(),
			stage->RendererID(),
			m_GLState->Label,
			m_GLState->Program
		);

		glAttachShader(static_cast<IDType>(*this), static_cast<ShaderStage::IDType>(*stage));
		m_GLState->Stages.emplace(stage);
		m_GLState->Reflection.UsedStages |= stage->GetType();
		FetchLog();
	}

	void ShaderProgram::Attach(std::initializer_list<Ref<ShaderStage>> stages) {
		for (auto stage : stages) {
			Attach(stage);
		}
	}

	void ShaderProgram::Detach(Ref<ShaderStage> stage) {
		if (!Utils::HasStage(m_GLState->Reflection.UsedStages, stage->GetType())) {
			LOG_GL_WARN("Tried to detach not attached shader");
			return;
		}

		auto& stages = m_GLState->Stages;
		auto iter = stages.find(stage);

		ENGINE_ASSERT(iter != stages.end());

		if (iter == stages.end())
			throw std::runtime_error("Detaching not attached shader");

		LOG_GL_DEBUG(
			"Detaching {} shader {} from shader program {} (id: {})",
			stage->TypeToString(),
			stage->RendererID(),
			m_GLState->Label,
			m_GLState->Program
		);

		glDetachShader(static_cast<IDType>(*this), static_cast<ShaderStage::IDType>(*stage));
		stages.erase(iter);
		Utils::RemoveStage(m_GLState->Reflection.UsedStages, stage->GetType());
		FetchLog();
	}

	void ShaderProgram::Detach(std::initializer_list<Ref<ShaderStage>> stages) {
		for (auto stage : stages) {
			Detach(stage);
		}
	}

	void ShaderProgram::DetachAll() {
		auto& stages = m_GLState->Stages;

		for (auto stage : stages) {
			LOG_GL_DEBUG(
				"Detaching {} shader {} from shader program {} (id: {})",
				stage->TypeToString(),
				stage->RendererID(),
				m_GLState->Label,
				m_GLState->Program
			);

			glDetachShader(static_cast<IDType>(*this), static_cast<ShaderStage::IDType>(*stage));
		}
		stages.clear();
		
		FetchLog();
	}


	bool ShaderProgram::IsAttached(Ref<ShaderStage> stage) {
		return m_GLState->Stages.contains(stage);
	}

	bool ShaderProgram::IsAttached(ShaderStage::Type type) {
		return std::ranges::find_if(m_GLState->Stages, [type](Ref<ShaderStage> stage) { return type == stage->GetType(); }) != m_GLState->Stages.end();
	}

	ShaderLinkResult ShaderProgram::Link() {
		LOG_GL_DEBUG("Linking {} (ID: {}) shader program", m_GLState->Label, m_GLState->Program);
		ShaderLinkResult result;

		glLinkProgram(static_cast<IDType>(*this));

		if (GetParameter(GL_LINK_STATUS) == GL_FALSE) {
			LOG_GL_ERROR("Failed to link {} (id: {}) shader program", m_GLState->Label, m_GLState->Program);
			result.Success = m_GLState->Linked = false;
		} else {
			LOG_GL_INFO("Successfully linked {} (id: {}) shader program", m_GLState->Label, m_GLState->Program);

			result.Success = m_GLState->Linked = true;

			Populate();
		}

		FetchLog();
		result.LogMessage = m_GLState->LogMessage;

		if (!result.LogMessage.empty()) {
			if (!result.Success)
				LOG_GL_ERROR("Linking log [ID: {}] '{}': {}", m_GLState->Program, m_GLState->Label, result.LogMessage);
			else
				LOG_GL_DEBUG("Linking log [ID: {}] '{}': {}", m_GLState->Program, m_GLState->Label, result.LogMessage);
		}

		return result;
	}

	ShaderValidationResult ShaderProgram::Validate() {
		ShaderValidationResult result;

		glValidateProgram(static_cast<IDType>(*this));

		const auto success = GetParameter(GL_VALIDATE_STATUS);

		if (success == GL_TRUE) {
			result.Valid = true;
		} else {
			result.Valid = false;
		}

		FetchLog();
		result.LogMessage = m_GLState->LogMessage;

		if (!result.LogMessage.empty()) {
			if (!result.Valid)
				LOG_GL_ERROR("Validation log [ID: {}] '{}': {}", m_GLState->Program, m_GLState->Label, result.LogMessage);
			else
				LOG_GL_DEBUG("Validation log [ID: {}] '{}': {}", m_GLState->Program, m_GLState->Label, result.LogMessage);
		}

		return result;
	}

	void ShaderProgram::Use() const {
		if (!IsLinked())
			return;

		glUseProgram(static_cast<IDType>(*this));
	}

	ShaderProgram::AttributeLocationType ShaderProgram::GetAttributeLocation(std::string_view name) const {
		return GetLocation(name, m_GLState->Attributes, glGetAttribLocation, InvalidAttributeLocation);
	}

	ShaderProgram::UniformLocationType ShaderProgram::GetUniformLocation(std::string_view name) const {
		return GetLocation(name, m_GLState->UniformLocations, glGetUniformLocation, InvalidUniformLocation);
	}

	ShaderProgram::UniformBlockIndexType ShaderProgram::GetUniformBlockIndex(std::string_view name) const {
		return GetLocation(name, m_GLState->UniformBlockIndices, glGetUniformBlockIndex, InvalidUniformBlockIndex);
	}

	bool ShaderProgram::HasUniform(std::string_view name) const {
		return GetUniformLocation(name) != InvalidUniformLocation;
	}

	const std::vector<ShaderProgram::UniformInfo>& ShaderProgram::GetActiveUniforms() const {
		return m_GLState->Reflection.Uniforms;
	}

	const std::vector<ShaderProgram::UniformBlockInfo>& ShaderProgram::GetActiveUniformBlocks() const {
		return m_GLState->Reflection.Blocks;
	}

	ShaderProgram::UniformInfo ShaderProgram::QueryUniform(UniformLocationType location) const {
		const auto& uniforms = m_GLState->Reflection.Uniforms;

		const auto iter = std::ranges::find_if(uniforms, [location](const UniformInfo& info) { return info.Location == location; });

		if (iter != uniforms.end())
			return *iter;

		ENGINE_ASSERT(false, fmt::format("Shader does not contains uniform at given location, {}", location));
		throw std::runtime_error(fmt::format("Shader does not contains uniform at given location, {}", location));
	}

	ShaderProgram::UniformBlockInfo ShaderProgram::QueryUniformBlock(UniformBlockIndexType index) const {
		const auto& blocks = m_GLState->Reflection.Blocks;

		const auto iter = std::ranges::find_if(blocks, [index](const UniformBlockInfo& info) { return info.Index == index; });

		if (iter != blocks.end())
			return *iter;

		ENGINE_ASSERT(false, fmt::format("Shader does not contains uniform block at given index, {}", index));
		throw std::runtime_error(fmt::format("Shader does not contains uniform block at given index, {}", index));
	}

	ShaderProgram::UniformInfo ShaderProgram::QueryUniform(std::string_view name) const {
		const auto& uniforms = m_GLState->Reflection.Uniforms;

		const auto iter = std::ranges::find_if(uniforms, [name](const UniformInfo& info) { return info.Name == name; });

		if (iter != uniforms.end())
			return *iter;

		ENGINE_ASSERT(false, fmt::format("Shader does not contains uniform with given name: \"{}\"", name));
		throw std::runtime_error(fmt::format("Shader does not contains uniform with given name: \"{}\"", name));
	}

	ShaderProgram::UniformBlockInfo ShaderProgram::QueryUniformBlock(std::string_view name) const {
		const auto& blocks = m_GLState->Reflection.Blocks;

		const auto iter = std::ranges::find_if(blocks, [name](const UniformBlockInfo& info) { return info.Name == name; });

		if (iter != blocks.end())
			return *iter;

		ENGINE_ASSERT(false, fmt::format("Shader does not contains uniform block with given name: \"{}\"", name));
		throw std::runtime_error(fmt::format("Shader does not contains uniform block with given name: \"{}\"", name));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value) {
		if (location == InvalidUniformLocation)
			return;

		glProgramUniform1i(m_GLState->Program, location, value);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value1, int32_t value2) {
		if (location == InvalidUniformLocation)
			return;

		glProgramUniform2i(m_GLState->Program, location, value1, value2);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value1, int32_t value2, int32_t value3) {
		if (location == InvalidUniformLocation)
			return;

		glProgramUniform3i(m_GLState->Program, location, value1, value2, value3);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value) {
		if (location == InvalidUniformLocation)
			return;

		glProgramUniform1ui(m_GLState->Program, location, value);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value1, uint32_t value2) {
		if (location == InvalidUniformLocation)
			return;

		glProgramUniform2ui(m_GLState->Program, location, value1, value2);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value1, uint32_t value2, uint32_t value3) {
		if (location == InvalidUniformLocation)
			return;

		glProgramUniform3ui(m_GLState->Program, location, value1, value2, value3);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value) {
		if (location == InvalidUniformLocation)
			return;

		glProgramUniform1f(m_GLState->Program, location, value);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value1, float value2) {
		if (location == InvalidUniformLocation)
			return;

		glProgramUniform2f(m_GLState->Program, location, value1, value2);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value1, float value2, float value3) {
		if (location == InvalidUniformLocation)
			return;

		glProgramUniform3f(m_GLState->Program, location, value1, value2, value3);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, double value) {
		if (location == InvalidUniformLocation)
			return;

		glProgramUniform1d(m_GLState->Program, location, value);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, double value1, double value2) {
		if (location == InvalidUniformLocation)
			return;

		glProgramUniform2d(m_GLState->Program, location, value1, value2);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, double value1, double value2, double value3) {
		if (location == InvalidUniformLocation)
			return;

		glProgramUniform3d(m_GLState->Program, location, value1, value2, value3);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec2& vec) {
		return UniformVector(location, vec, glProgramUniform2fv);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec3& vec) {
		return UniformVector(location, vec, glProgramUniform3fv);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec4& vec) {
		return UniformVector(location, vec, glProgramUniform4fv);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x2& mat, bool transpose) {
		return UniformMatrix(location, mat, transpose ? GL_TRUE : GL_FALSE, glProgramUniformMatrix2fv);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x3& mat, bool transpose) {
		return UniformMatrix(location, mat, transpose ? GL_TRUE : GL_FALSE, glProgramUniformMatrix2x3fv);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x4& mat, bool transpose) {
		return UniformMatrix(location, mat, transpose ? GL_TRUE : GL_FALSE, glProgramUniformMatrix2x4fv);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x2& mat, bool transpose) {
		return UniformMatrix(location, mat, transpose ? GL_TRUE : GL_FALSE, glProgramUniformMatrix3x2fv);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x3& mat, bool transpose) {
		return UniformMatrix(location, mat, transpose ? GL_TRUE : GL_FALSE, glProgramUniformMatrix3fv);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x4& mat, bool transpose) {
		return UniformMatrix(location, mat, transpose ? GL_TRUE : GL_FALSE, glProgramUniformMatrix3x4fv);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x2& mat, bool transpose) {
		return UniformMatrix(location, mat, transpose ? GL_TRUE : GL_FALSE, glProgramUniformMatrix4x2fv);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x3& mat, bool transpose) {
		return UniformMatrix(location, mat, transpose ? GL_TRUE : GL_FALSE, glProgramUniformMatrix4x3fv);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x4& mat, bool transpose) {
		return UniformMatrix(location, mat, transpose ? GL_TRUE : GL_FALSE, glProgramUniformMatrix4fv);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, Ref<Texture> texture, uint32_t samplerUnit) {
		if (location == InvalidUniformLocation)
			return;

		texture->BindUnit(samplerUnit);

		glProgramUniform1i(m_GLState->Program, location, samplerUnit);
	}

	void ShaderProgram::BindUniformBuffer(UniformBlockIndexType location, const UniformBuffer& buffer, uint32_t bindingPoint) {
		if (location == InvalidUniformLocation)
			return;

		//TODO
	}

	void ShaderProgram::BindUniformBuffer(UniformBlockIndexType location, const UniformBuffer& buffer, uint32_t bindingPoint, size_t size, size_t offset) {
		if (location == InvalidUniformLocation)
			return;

		//TODO
	}

	ShaderProgram::ShaderProgram() : m_GLState(MakeRef<GLState>()) {
	}

	int ShaderProgram::GetParameter(uint32_t pName) const {
		int value = 0;
		GetParameter(pName, &value);

		return value;
	}

	void ShaderProgram::GetParameter(uint32_t pName, int* params) const {
		glGetProgramiv(static_cast<IDType>(*this), pName, params);
	}

	void ShaderProgram::FetchLog() {
		int length = GetParameter(GL_INFO_LOG_LENGTH);

		if (length > 0) {
			m_GLState->LogMessage.resize(length + 1, 0);

			glGetProgramInfoLog(static_cast<IDType>(*this), length, nullptr, m_GLState->LogMessage.data());

			boost::trim(m_GLState->LogMessage);
		}
		else
			m_GLState->LogMessage.resize(1, 0);

		if (!m_GLState->LogMessage.empty() && m_GLState->LogMessage.back() == '\0')
			m_GLState->LogMessage.pop_back();
	}

	bool ShaderProgram::CheckIfLinked() {
		const auto linked = GetParameter(GL_LINK_STATUS);

		if (linked == GL_TRUE) {
			return m_GLState->Linked = true;
		}

		return m_GLState->Linked = false ;
	}

	void ShaderProgram::Populate() {
		m_GLState->UniformLocations.clear();
		m_GLState->Attributes.clear();
		m_GLState->UniformBlockIndices.clear();

		m_GLState->Reflection.Uniforms.clear();
		m_GLState->Reflection.Blocks.clear();

		PopulateUniforms();
		PopulateUniformBlocks();
	}

	void ShaderProgram::PopulateUniformBlocks() {
		const uint32_t count = static_cast<uint32_t>(GetParameter(GL_ACTIVE_UNIFORM_BLOCKS));
		LOG_GL_DEBUG("Shader program {} (ID: {}) has {} active uniform blocks: ", m_GLState->Label, m_GLState->Program, count);

		m_GLState->UniformBlockIndices.reserve(count);
		m_GLState->Reflection.Blocks.reserve(count);

		for (uint32_t i = 0; i < count; ++i) {
			const auto info = GetUniformBlockInfo(i);

			m_GLState->Reflection.Blocks.emplace_back(info);
			m_GLState->UniformBlockIndices[info.Name] = info.Index;

			LOG_GL_DEBUG(
				"UniformBlock {}, name: {}, Size: {}, Index: {}, ReferencedBy: {}",
				i,
				info.Name,
				info.Size,
				info.Index,
				info.ShaderType
			);
		}
	}

	void ShaderProgram::PopulateUniforms() {
		const uint32_t count = static_cast<uint32_t>(GetParameter(GL_ACTIVE_UNIFORMS));
		LOG_GL_DEBUG("Shader program {} (ID: {}) has {} active uniforms", m_GLState->Label, m_GLState->Program, count);

		for (uint32_t i = 0; i < count; ++i) {
			const auto info = GetUniformInfo(i);

			m_GLState->UniformLocations.emplace(info.Name, info.Location);
			m_GLState->Reflection.Uniforms.emplace_back(info);

			if (info.Size > 1) {
				LOG_GL_DEBUG("Uniform {} is table", info.Name);
				for (int j = 0; j < info.Size; ++j) {
					const std::string indexedName = fmt::format("{}[{}]", info.Name, j);
					const int loc = glGetUniformLocation(static_cast<IDType>(*this), indexedName.c_str());

					if (loc != -1) {
						m_GLState->UniformLocations.emplace(indexedName, loc);
						m_GLState->Reflection.Uniforms.emplace_back(indexedName, 1, info.Type, loc);
					}
				}
			}

			LOG_GL_DEBUG(
				"Uniform {}, Name: {}, Size: {}, Type: {}, Location: {}",
				i,
				info.Name,
				info.Size,
				info.Type,
				info.Location
			);
		}
	}

	int ShaderProgram::GetActiveUniformI(uint32_t index, uint32_t pName) const {
		int value = 0;
		glGetActiveUniformsiv(static_cast<IDType>(*this), 1, &index, pName, &value);

		return value;
	}

	int ShaderProgram::GetActiveUniformBlockI(uint32_t index, uint32_t pName) const {
		int value = 0;
		glGetActiveUniformBlockiv(static_cast<IDType>(*this), index, pName, &value);

		return value;
	}

	std::string ShaderProgram::GetActiveUniformBlockName(uint32_t index) const {
		const size_t length = static_cast<size_t>(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_NAME_LENGTH));
		std::string name(length, '\0');
		GLsizei written = 0;

		glGetActiveUniformBlockName(static_cast<IDType>(*this), index, static_cast<GLsizei>(length), &written, name.data());

		if (written > 0) name.resize(written);
		// glGetActiveUniformName(static_cast<IDType>(*this), index, static_cast<GLsizei>(length), nullptr, name.data());
		return name;
	}

	ShaderProgram::UniformBlockInfo ShaderProgram::GetUniformBlockInfo(uint32_t index) const {
		UniformBlockInfo info;

		info.Name = GetActiveUniformBlockName(index);
		info.Size = static_cast<uint32_t>(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_DATA_SIZE));
		info.Index = index;
		info.Binding = static_cast<UniformBlockBindingType>(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_BINDING));

		if (GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER) == GL_TRUE)
			info.ShaderType |= ShaderStage::Type::Vertex;
		if (GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER) == GL_TRUE)
			info.ShaderType |= ShaderStage::Type::Control;
		if (GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER) == GL_TRUE)
			info.ShaderType |= ShaderStage::Type::Evaluation;
		if (GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER) == GL_TRUE)
			info.ShaderType |= ShaderStage::Type::Geometry;
		if (GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER) == GL_TRUE)
			info.ShaderType |= ShaderStage::Type::Fragment;
		if (GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER) == GL_TRUE)
			info.ShaderType |= ShaderStage::Type::Compute;

		return info;
	}

	ShaderProgram::UniformInfo ShaderProgram::GetUniformInfo(uint32_t index) const {
		UniformInfo info;

		uint32_t length = static_cast<uint32_t>(GetActiveUniformI(index, GL_UNIFORM_NAME_LENGTH));

		info.Name = std::string(length, '\0');
		GLsizei written = 0;
		glGetActiveUniform(static_cast<IDType>(*this), index, static_cast<GLsizei>(length), &written, &info.Size, &info.Type, info.Name.data());
		if (written > 0) info.Name.resize(written);

		info.Location = glGetUniformLocation(static_cast<IDType>(*this), info.Name.data());

		if (info.Size > 1 && info.Name.ends_with("[0]")) {
			info.Name.resize(info.Name.size() - 3); //remove "[0]"
		}

		return info;
	}

	ShaderProgram::GLState::GLState() : Program(glCreateProgram()){
		ENGINE_ASSERT(Program != 0);
		glProgramParameteri(Program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
	}

	ShaderProgram::GLState::~GLState() {
		Stages.clear();
		glDeleteProgram(Program);
	}
}
