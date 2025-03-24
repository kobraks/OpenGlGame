#include "pch.h"
#include "ShaderPRogram.h"

#include "Engine/Renderer/Texture2D.h"

#include "glm/gtc/type_ptr.hpp"

namespace Engine {
	ShaderProgram::ShaderProgram(const std::string &name) {
		LOG_GL_INFO("Creating shader program");
		m_Internals = MakeRef<Internals>(name);
		LOG_GL_INFO("New {} shader program id: {}", m_Internals->Name, m_Internals->Program);
	}

	ShaderProgram::ShaderProgram(const std::string &name, Ref<Shader> shader) : ShaderProgram(name) {
		Attach(shader);

		Link();

		Detach(shader);
	}

	ShaderProgram::operator unsigned int() const {
		return m_Internals->Program;
	}

	ShaderProgram::IDType ShaderProgram::ID() const {
		return m_Internals->Program;
	}

	bool ShaderProgram::IsLinked() const {
		return m_Internals->Linked;
	}

	std::string_view ShaderProgram::Name() const {
		return m_Internals->Name;
	}

	void ShaderProgram::Attach(Ref<Shader> shader) {
		m_Internals->Attach(shader);
	}

	void ShaderProgram::Detach(Ref<Shader> shader) {
		m_Internals->Detach(shader);
	}

	bool ShaderProgram::IsAttached(Ref<Shader> shader) {
		return m_Internals->Shaders.contains(shader);
	}

	bool ShaderProgram::IsAttached(Shader::Type shaderType) {
		return std::ranges::find_if(
		                            m_Internals->Shaders,
		                            [shaderType](Ref<Shader> shader) { return shaderType == shader->GetType(); }
		                           ) != m_Internals->Shaders.end();
	}

	Ref<Shader> ShaderProgram::GetShader(Shader::Type shaderType) {
		const auto iter = std::ranges::find_if(
		                                       m_Internals->Shaders,
		                                       [shaderType](Ref<Shader> shader) {
			                                       return shaderType == shader->GetType();
		                                       }
		                                      );
		return (iter != std::end(m_Internals->Shaders)) ? *iter : nullptr;
	}

	bool ShaderProgram::Link() {
		return m_Internals->Link();
	}

	void ShaderProgram::Use() {
		m_Internals->Use();
	}

	std::string ShaderProgram::GetLog() const {
		return m_Internals->GetLog();
	}

	size_t ShaderProgram::GetAttachedShaderCount() const {
		return m_Internals->Shaders.size();
	}

	ShaderProgram::AttributeLocationType ShaderProgram::GetAttributeLocation(std::string_view name) const {
		return m_Internals->GetAttributeLocation(name);
	}

	ShaderProgram::UniformLocationType ShaderProgram::GetUniformLocation(std::string_view name) const {
		return m_Internals->GetUniformLocation(name);
	}

	ShaderProgram::UniformBlockIndexType ShaderProgram::GetUniformBlockIndex(std::string_view name) {
		return m_Internals->GetUniformBlockIndex(name);
	}

	bool ShaderProgram::HasUniform(std::string_view name) {
		return m_Internals->GetUniformLocation(name) != InvalidUniformLocation;
	}

	const std::vector<ShaderProgram::UniformInfo>& ShaderProgram::GetActiveUniforms() const {
		return m_Internals->ActiveUniforms;
	}

	const std::vector<ShaderProgram::UniformBlockInfo>& ShaderProgram::GetActiveUniformBlocks() const {
		return m_Internals->ActiveUniformBlocks;
	}

	ShaderProgram::UniformInfo ShaderProgram::QueryUniform(UniformLocationType location) const {
		const auto iter = std::ranges::find_if(
		                                       m_Internals->ActiveUniforms,
		                                       [location](const UniformInfo &info) { return info.Location == location; }
		                                      );

		if(iter != m_Internals->ActiveUniforms.end())
			return *iter;

		ENGINE_ASSERT(false, fmt::format("Shader does not contains uniform at given location, {}", location));
		throw std::runtime_error(fmt::format("Shader does not contains uniform at given location, {}", location));
	}

	ShaderProgram::UniformBlockInfo ShaderProgram::QueryUniformBlock(UniformBlockIndexType index) const {
		const auto iter = std::ranges::find_if(
		                                       m_Internals->ActiveUniformBlocks,
		                                       [index](const UniformBlockInfo &info) { return info.Index == index; }
		                                      );

		if(iter != m_Internals->ActiveUniformBlocks.end())
			return *iter;

		ENGINE_ASSERT(false, fmt::format("Shader does not contains uniform block at given index, {}", index));
		throw std::runtime_error(fmt::format("Shader does not contains uniform block at given index, {}", index));
	}

	ShaderProgram::UniformInfo ShaderProgram::QueryUniform(std::string_view name) const {
		const auto iter = std::ranges::find_if(
		                                       m_Internals->ActiveUniforms,
		                                       [name](const UniformInfo &info) { return info.Name == name; }
		                                      );

		if(iter != m_Internals->ActiveUniforms.end())
			return *iter;

		ENGINE_ASSERT(false, fmt::format("Shader does not contains uniform with given name: \"{}\"", name));
		throw std::runtime_error(fmt::format("Shader does not contains uniform with given name: \"{}\"", name));
	}

	ShaderProgram::UniformBlockInfo ShaderProgram::QueryUniformBlock(std::string_view name) const {
		const auto iter = std::ranges::find_if(
		                                       m_Internals->ActiveUniformBlocks,
		                                       [name](const UniformBlockInfo &info) { return info.Name == name; }
		                                      );

		if(iter != m_Internals->ActiveUniformBlocks.end())
			return *iter;

		ENGINE_ASSERT(false, fmt::format("Shader does not contains uniform block with given name: \"{}\"", name));
		throw std::runtime_error(fmt::format("Shader does not contains uniform block with given name: \"{}\"", name));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value) {
		UniformValue1(location, value);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value1, int32_t value2) {
		UniformValue2(location, value1, value2);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value1, int32_t value2, int32_t value3) {
		UniformValue3(location, value1, value2, value3);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value) {
		UniformValue1(location, value);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value1, uint32_t value2) {
		UniformValue2(location, value1, value2);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value1, uint32_t value2, uint32_t value3) {
		UniformValue3(location, value1, value2, value3);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value) {
		UniformValue1(location, value);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value1, float value2) {
		UniformValue2(location, value1, value2);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value1, float value2, float value3) {
		UniformValue3(location, value1, value2, value3);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, double value) {
		UniformValue1(location, value);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, double value1, double value2) {
		UniformValue2(location, value1, value2);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, double value1, double value2, double value3) {
		UniformValue3(location, value1, value2, value3);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec2 &vec) {
		UniformVector(location, vec);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec3 &vec) {
		UniformVector(location, vec);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec4 &vec) {
		UniformVector(location, vec);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x2 &mat, bool transpose) {
		UniformMatrix(location, mat, transpose);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x3 &mat, bool transpose) {
		UniformMatrix(location, mat, transpose);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x4 &mat, bool transpose) {
		UniformMatrix(location, mat, transpose);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x2 &mat, bool transpose) {
		UniformMatrix(location, mat, transpose);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x3 &mat, bool transpose) {
		UniformMatrix(location, mat, transpose);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x4 &mat, bool transpose) {
		UniformMatrix(location, mat, transpose);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x2 &mat, bool transpose) {
		UniformMatrix(location, mat, transpose);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x3 &mat, bool transpose) {
		UniformMatrix(location, mat, transpose);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x4 &mat, bool transpose) {
		UniformMatrix(location, mat, transpose);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, Ref<Texture2D> texture, uint32_t samplerUnit) {
		/*glActiveTexture(GL_TEXTURE0 + samplerUnit);
		texture->Bind();*/
		texture->BindUnit(samplerUnit);

		glUniform1ui(location, samplerUnit);
	}

	void ShaderProgram::BindUniformBuffer(UniformLocationType location, const UniformBuffer &buffer) {}

	void ShaderProgram::BindUniformBuffer(
		UniformLocationType location,
		const UniformBuffer &buffer,
		size_t size,
		size_t offset
		) {
		//TODO
	}

	ShaderProgram::Internals::Internals(const std::string &name) : Name(name) {
		Program = glCreateProgram();
	}

	ShaderProgram::Internals::Internals(IDType id, const std::string &name) {}

	ShaderProgram::Internals::~Internals() {
		Shaders.clear();
		glDeleteProgram(Program);
	}

	void ShaderProgram::Internals::Attach(Ref<Shader> shader) {
		if(Shaders.contains(shader)) {
			LOG_GL_WARN("Trying to attach attached shader");
			return;
		}

		LOG_GL_INFO(
		            "Attaching {} shader {} to shader program {} (id: {})",
		            shader->TypeToString(),
		            shader->ID(),
		            Name,
		            Program
		           );
		glAttachShader(Program, *shader);
		Shaders.emplace(shader);
	}

	void ShaderProgram::Internals::Detach(Ref<Shader> shader) {
		auto iter = Shaders.find(shader);

		ENGINE_ASSERT(iter != Shaders.end());

		if(iter == Shaders.end())
			throw std::runtime_error("Detaching not attached shader");

		LOG_GL_INFO(
		            "Detaching {} shader {} from shader program {} (id: {})",
		            shader->TypeToString(),
		            shader->ID(),
		            Name,
		            Program
		           );

		glDetachShader(Program, *shader);
		Shaders.erase(iter);
	}

	bool ShaderProgram::Internals::Link() {
		LOG_GL_INFO("Linking {} (ID: {}) shader program", Name, Program);

		glLinkProgram(Program);

		if(Get(ParametersName::LinkStatus) == GL_FALSE) {
			LOG_GL_ERROR("Unable to link {} (id: {}) shader program", Name, Program);
			return Linked = false;
		}

		LOG_GL_INFO("Sucessfuly linked {} (id: {}) shader program", Name, Program);

		Populate();
		return Linked = true;
	}

	void ShaderProgram::Internals::Use() {
		if(!Linked)
			return;

		glUseProgram(Program);
	}

	std::string ShaderProgram::Internals::GetLog() {
		int length = Get(ParametersName::InfoLogLength);

		if(length > 0) {
			std::string log(length, 0);

			glGetProgramInfoLog(Program, length, &length, &log[0]);
			return log;
		}

		return {};
	}

	ShaderProgram::AttributeLocationType ShaderProgram::Internals::GetAttributeLocation(std::string_view name) {
		return GetLocation(name, Attributes, glGetAttribLocation, InvalidAttributeLocation);
	}

	ShaderProgram::UniformLocationType ShaderProgram::Internals::GetUniformLocation(std::string_view name) {
		return GetLocation(name, UniformLocations, glGetUniformLocation, InvalidUniformLocation);
	}

	ShaderProgram::UniformBlockIndexType ShaderProgram::Internals::GetUniformBlockIndex(std::string_view name) {
		return GetLocation(name, UniformBlockIndices, glGetUniformBlockIndex, InvalidUniformBlockIndex);
	}

	int ShaderProgram::Internals::Get(ParametersName pName) {
		int result = 0;
		Get(pName, &result);

		return result;
	}

	void ShaderProgram::Internals::Get(ParametersName pName, int *params) {
		glGetProgramiv(Program, static_cast<GLenum>(pName), params);
	}

	void ShaderProgram::Internals::Populate() {
		PopulateUniforms();
		PopulateUniformBlocks();
	}

	void ShaderProgram::Internals::PopulateUniformBlocks() {
		const uint32_t count = static_cast<uint32_t>(Get(ParametersName::ActiveUniformBlocks));
		LOG_GL_INFO("Shader program {} (ID: {}) has {} active uniform blocks: ", Name, Program, count);

		UniformBlockIndices.reserve(count);
		ActiveUniformBlocks.reserve(count);

		for(uint32_t i = 0; i < count; ++i) {
			const auto info = QueryUniformBlock(i);

			ActiveUniformBlocks.emplace_back(info);
			UniformBlockIndices[info.Name] = info.Index;

			LOG_GL_DEBUG(
			             "UniformBlock {}, name: {}, Size: {}, Index: {}, ReferedBy: {}",
			             i,
			             info.Name,
			             info.Size,
			             info.Index,
			             Shader::TypeToString(info.ShaderType)
			            );
		}
	}

	void ShaderProgram::Internals::PopulateUniforms() {
		const uint32_t count = static_cast<uint32_t>(Get(ParametersName::ActiveUniforms));
		LOG_GL_INFO("Shader program {} (ID: {}) has {} active uniforms", Name, Program, count);

		for(uint32_t i = 0; i < count; ++i) {
			const auto info = QueryUniform(i);

			UniformLocations.emplace(info.Name, info.Location);
			ActiveUniforms.emplace_back(info);

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

	int ShaderProgram::Internals::GetActiveUniformI(uint32_t index, GLenum pName) {
		int value = 0;
		glGetActiveUniformsiv(Program, 1, &index, pName, &value);
		return value;
	}

	int ShaderProgram::Internals::GetActiveUniformBlockI(uint32_t index, GLenum pName) {
		int value = 0;
		glGetActiveUniformBlockiv(Program, index, pName, &value);

		return value;
	}

	std::string ShaderProgram::Internals::GetActiveUniformBlockName(uint32_t index) {
		const size_t length = static_cast<size_t>(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_NAME_LENGTH));
		std::string name(length + 1, 0);

		glGetActiveUniformName(Program, index, static_cast<GLsizei>(length), nullptr, &name[0]);
		return name;
	}

	ShaderProgram::UniformBlockInfo ShaderProgram::Internals::QueryUniformBlock(uint32_t index) {
		UniformBlockInfo info;

		info.Name  = GetActiveUniformBlockName(index);
		info.Size  = static_cast<uint32_t>(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_DATA_SIZE));
		info.Index = static_cast<UniformBlockIndexType>(GetActiveUniformI(index, GL_UNIFORM_BLOCK_BINDING));

		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER) == GL_TRUE)
			info.ShaderType = Shader::Type::Vertex;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER) == GL_TRUE)
			info.ShaderType = Shader::Type::Control;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER) == GL_TRUE)
			info.ShaderType = Shader::Type::Evaluation;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER) == GL_TRUE)
			info.ShaderType = Shader::Type::Geometry;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER) == GL_TRUE)
			info.ShaderType = Shader::Type::Fragment;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER) == GL_TRUE)
			info.ShaderType = Shader::Type::Compute;

		return info;
	}

	ShaderProgram::UniformInfo ShaderProgram::Internals::QueryUniform(uint32_t index) {
		UniformInfo info;

		uint32_t length = GetActiveUniformI(index, GL_UNIFORM_NAME_LENGTH);

		info.Name = std::string(length, 0);
		glGetActiveUniform(Program, index, length, nullptr, &info.Size, &info.Type, &info.Name[0]);
		info.Location = glGetUniformLocation(Program, &info.Name[0]);

		return info;
	}
}
