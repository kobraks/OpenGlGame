#include "pch.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include "Assert.h"
#include "Log.h"
#include "GLCheck.h"
#include "Texture.h"
#include "Buffer.h"

namespace Game
{
	ShaderProgram::Internals::Internals(std::string name) : Name(std::move(name))
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(Program = glCreateProgram());
	}

	ShaderProgram::Internals::Internals(IdType id, std::string name) : Program(id),
	                                                                   Name(std::move(name))
	{
		CHECK_IF_VALID_CONTEXT;
		bool isProgram = false;
		GL_CHECK(isProgram = glIsProgram(id) == GL_TRUE ? true : false);

		if(!isProgram)
		{
			ASSERT(false, "Provided id {} do not belong to shader program", id);
			std::runtime_error(fmt::format("Provided id {} do not belong to shader program", id));
		}


		if(Get(ParametersName::LinkStatus) == GL_TRUE)
			Linked = true;

		GL_LOG_INFO("Link status of provided shader: {}", Linked);

		if(Linked)
			Populate();
	}

	ShaderProgram::Internals::~Internals()
	{
		Shaders.clear();
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glDeleteProgram(Program));
	}

	void ShaderProgram::Internals::Attach(Pointer<Shader> shader)
	{
		if(Shaders.contains(shader))
		{
			GL_LOG_WARN("Trying to attach attached shader");
			return;
		}

		GL_LOG_INFO("Attaching {} shader {} to shader program {} (id: {})", shader->TypeToString(), shader->ID(), Name, Program);

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glAttachShader(Program, *shader));
		Shaders.emplace(shader);
	}

	void ShaderProgram::Internals::Detach(Pointer<Shader> shader)
	{
		auto &shaders = Shaders;
		auto iter     = shaders.find(shader);

		ASSERT(iter != shaders.end(), "Detaching not attached shader");

		if(iter == shaders.end())
			throw std::runtime_error("Detaching not attached shader");

		GL_LOG_INFO("Detaching {} shader {} to shader program {} (id: {})", shader->TypeToString(), shader->ID(), Name, Program);

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glDetachShader(Program, *shader));
		shaders.erase(iter);
	}

	bool ShaderProgram::Internals::IsAttached(const Pointer<Shader> &shader) const
	{
		return Shaders.contains(shader);
	}

	bool ShaderProgram::Internals::IsAttached(Shader::Type type)
	{
		return std::ranges::find_if(Shaders, [type](const Pointer<Shader> &shader) { return type == shader->GetType(); }) != Shaders.end();
	}

	bool ShaderProgram::Internals::Link()
	{
		GL_LOG_INFO("Linking {} (id: {}) shader program", Name, Program);

		CHECK_IF_VALID_CONTEXT false;
		GL_CHECK(glLinkProgram(Program));

		if(Get(ParametersName::LinkStatus) == GL_FALSE)
		{
			GL_LOG_ERROR("Unable to link {} (id: {}) shader program", Name, Program);
			return Linked = false;
		}

		GL_LOG_INFO("Sucessful linked {} (id: {}) shader program", Name, Program);

		Populate();
		return Linked = true;
	}

	void ShaderProgram::Internals::Use()
	{
		if(!Linked)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUseProgram(Program));
	}

	std::string ShaderProgram::Internals::GetLog() const
	{
		int length = Get(ParametersName::InfoLogLength);

		if(length > 0)
		{
			std::string log((length), 0);
			CHECK_IF_VALID_CONTEXT {};
			GL_CHECK(glGetProgramInfoLog(Program, length, &length, &log[0]));
			return log;
		}

		return {};
	}

	bool ShaderProgram::Internals::HasUniform(const std::string &name) const
	{
		return GetUniformLocation(name) != INVALID_UNIFORM_LOCATION;
	}

	ShaderProgram::AttributeLocationType ShaderProgram::Internals::GetAttributeLocation(const std::string &name) const
	{
		const auto iter = Attributes.find(name);
		auto location   = INVALID_ATTRIBUTE_LOCATION;

		if(iter != Attributes.end())
			location = iter->second;
		else
		{
			CHECK_IF_VALID_CONTEXT location;
			GL_CHECK(location = glGetAttribLocation(Program, name.c_str()));
			Attributes.emplace(std::make_pair(name, location));
		}

		return location;
	}

	ShaderProgram::UniformLocationType ShaderProgram::Internals::GetUniformLocation(const std::string &name) const
	{
		const auto iter = UniformsLocation.find(name);
		auto location   = INVALID_UNIFORM_LOCATION;
		CHECK_IF_VALID_CONTEXT location;
		if(iter != UniformsLocation.end())
			location = iter->second;
		else
			GL_CHECK(location = UniformsLocation.emplace(name, glGetUniformLocation(Program, name.c_str())).first->second);

		return location;
	}

	ShaderProgram::UniformBlockIndexType ShaderProgram::Internals::GetUniformBlockIndex(const std::string &name) const
	{
		const auto iter = UniformBlocksIndex.find(name);
		auto index      = INVALID_UNIFORM_BLOCK_INDEX;

		CHECK_IF_VALID_CONTEXT index;
		if(iter != UniformBlocksIndex.end())
			index = iter->second;
		else
			GL_CHECK(index = UniformsLocation.emplace(name, glGetUniformBlockIndex(Program, name.c_str())).first->second);

		return index;
	}

	int ShaderProgram::Internals::Get(ParametersName name) const
	{
		int result = 0;
		Get(name, &result);

		return result;
	}

	void ShaderProgram::Internals::Get(ParametersName name, int *params) const
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glGetProgramiv(Program, static_cast<GLenum>(name), params));
	}

	void ShaderProgram::Internals::Populate()
	{
		PopulateUniforms();
		PopulateUniformBlocks();
	}

	void ShaderProgram::Internals::PopulateUniformBlocks()
	{
		const uint32_t count = static_cast<uint32_t>(Get(ParametersName::ActiveUniformBlocks));
		GL_LOG_INFO("Shader program {} (id: {}) has {} active uniforms blocks", Name, Program, count);
		UniformBlocksIndex.reserve(count);
		ActiveUniformBlocks.reserve(count);

		for(uint32_t i = 0; i < count; ++i)
		{
			const auto info = QueryUniformBlock(i);
			ActiveUniformBlocks.emplace_back(info);
			UniformBlocksIndex[info.Name] = info.Index;

			GL_LOG_DEBUG(
			             "UniformBlock {}, Name: {}, Size: {}, Index: {}, ReferedBy: {}",
			             i,
			             info.Name,
			             info.Size,
			             info.Index,
			             Shader::TypeToString(info.ShaderType)
			            );
		}
	}

	void ShaderProgram::Internals::PopulateUniforms()
	{
		const uint32_t count = static_cast<uint32_t>(Get(ParametersName::ActiveUniforms));
		GL_LOG_INFO("Shader program {} (id: {}) has {} active uniforms", Name, Program, count);

		for(uint32_t i = 0; i < count; ++i)
		{
			const auto info = QueryUniform(i);

			UniformsLocation.emplace(info.Name, info.Location);
			ActiveUniforms.emplace_back(info);

			GL_LOG_DEBUG("Uniform {}, Name: {} Size: {} Type: {}, Location: {}", i, info.Name, info.Size, static_cast<uint32_t>(info.Type), info.Location);
		}
	}

	int ShaderProgram::Internals::GetActiveUniformI(uint32_t index, GLenum pName) const
	{
		int value = 0;
		GL_CHECK(glGetActiveUniformsiv(Program, 1, &index, pName, &value));
		CHECK_IF_VALID_CONTEXT 0;

		return value;
	}

	int ShaderProgram::Internals::GetActiveUniformBlockI(uint32_t index, GLenum pName) const
	{
		int value = 0;
		CHECK_IF_VALID_CONTEXT 0;
		GL_CHECK(glGetActiveUniformBlockiv(Program, index, pName, &value));

		return value;
	}

	std::string ShaderProgram::Internals::GetActiveUniformBlockName(uint32_t index) const
	{
		const size_t length = GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_NAME_LENGTH);
		std::string name(length, 0);

		CHECK_IF_VALID_CONTEXT {};
		GL_CHECK(glGetActiveUniformBlockName(Program, index, length, nullptr, &name[0]));
		return name;
	}

	ShaderProgram::UniformBlockInfo ShaderProgram::Internals::QueryUniformBlock(uint32_t index) const
	{
		UniformBlockInfo info;

		info.Name  = GetActiveUniformBlockName(index);
		info.Size  = static_cast<uint32_t>(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_DATA_SIZE));
		info.Index = static_cast<UniformBlockIndexType>(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_BINDING));

		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER))
			info.ShaderType = Shader::Type::Vertex;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER))
			info.ShaderType = Shader::Type::Control;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER))
			info.ShaderType = Shader::Type::Evaluation;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER))
			info.ShaderType = Shader::Type::Geometry;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER))
			info.ShaderType = Shader::Type::Fragment;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER))
			info.ShaderType = Shader::Type::Compute;

		return info;
	}

	ShaderProgram::UniformInfo ShaderProgram::Internals::QueryUniform(uint32_t index) const
	{
		UniformInfo info;

		uint32_t length = static_cast<uint32_t>(GetActiveUniformI(index, GL_UNIFORM_NAME_LENGTH));
		int size        = 0;
		uint32_t type   = 0;

		std::string name((length), 0);

		CHECK_IF_VALID_CONTEXT info;
		GL_CHECK(glGetActiveUniform(Program, index, length, nullptr, &size, &type, &name[0]));
		UniformLocationType location = INVALID_UNIFORM_LOCATION;
		GL_CHECK(location = glGetUniformLocation(Program, &name[0]));

		info.Name     = std::move(name);
		info.Size     = size;
		info.Type     = static_cast<UniformType>(type);
		info.Location = location;

		return info;
	}

	ShaderProgram::ShaderProgram(const std::string &name)
	{
		GL_LOG_INFO("Creating shader program");
		m_Internals = MakePointer<Internals>(name);
		GL_LOG_INFO("New {} shader program id: {}", m_Internals->Name, m_Internals->Program);
	}

	ShaderProgram::ShaderProgram(IdType id, const std::string &name)
	{
		m_Internals = MakePointer<Internals>(id, name);
	}

	ShaderProgram::ShaderProgram(const std::string &name, Pointer<Shader> shader) : ShaderProgram(name)
	{
		Attach(shader);

		Link();

		Detach(shader);
	}

	Pointer<Shader> ShaderProgram::GetShader(Shader::Type type)
	{
		return *std::ranges::find_if(m_Internals->Shaders, [type](const Pointer<Shader> &shader) { return shader->GetType() == type; });
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniform1iv(location, 1, &value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value, int32_t value2)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniform2i(location, value, value2));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value, int32_t value2, int32_t value3)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniform3i(location, value, value2, value3));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniform1uiv(location, 1, &value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value, uint32_t value2)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniform2ui(location, value, value2));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value, uint32_t value2, uint32_t value3)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniform3ui(location, value, value2, value3));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniform1fv(location, 1, &value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value, float value2)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniform2f(location, value, value2));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value, float value2, float value3)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniform3f(location, value, value2, value3));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec2 &value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniform2fv(location, 1, &value.x));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec3 &value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniform3fv(location, 1, &value.x));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec4 &value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniform4fv(location, 1, &value.x));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x2 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniformMatrix2fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x3 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniformMatrix2x3fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x4 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniformMatrix2x4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x2 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniformMatrix3x2fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x3 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniformMatrix3fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x4 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniformMatrix3x4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x2 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniformMatrix4x2fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x3 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniformMatrix4x3fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x4 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUniformMatrix4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const Texture &texture, int32_t sampleUnit)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glActiveTexture(GL_TEXTURE0 + sampleUnit));
		GL_CHECK(glUniform1i(location, sampleUnit));
		texture.Bind();
	}

	void ShaderProgram::BindUniformBuffer(UniformBlockIndexType index, const UniformBuffer &buffer)
	{
		if(index == INVALID_UNIFORM_BLOCK_INDEX)
			return;

		CHECK_IF_VALID_CONTEXT;
		buffer.Bind();
		GL_CHECK(glBindBufferBase(static_cast<GLenum>(buffer.Type()), index, buffer));
	}

	void ShaderProgram::BindUniformBuffer(UniformBlockIndexType index, const UniformBuffer &buffer, size_t size, size_t offset)
	{
		if(index == INVALID_UNIFORM_BLOCK_INDEX)
			return;

		CHECK_IF_VALID_CONTEXT;
		buffer.Bind();
		GL_CHECK(glBindBufferRange(static_cast<GLenum>(buffer.Type()), index, buffer, offset, size));
	}

	ShaderProgram* ShaderProgram::GetDefault()
	{
		static ShaderProgram shader(0, DEFAULT_PROGRAM_SHADER_NAME);

		return &shader;
	}

	const std::vector<ShaderProgram::UniformInfo>& ShaderProgram::GetActiveUniforms() const
	{
		return m_Internals->ActiveUniforms;
	}

	ShaderProgram::UniformInfo ShaderProgram::QueryUniform(UniformLocationType location) const
	{
		const auto iter = std::ranges::find_if(m_Internals->ActiveUniforms, [location](const UniformInfo &info) { return info.Location == location; });

		if(iter != m_Internals->ActiveUniforms.end())
			return *iter;

		ASSERT(false, "Shader does not contains uniform at given location, {}", location);
		throw std::runtime_error("Shader does not contains uniform at given location");
	}

	ShaderProgram::UniformBlockInfo ShaderProgram::QueryUniformBlock(UniformBlockIndexType index) const
	{
		const auto iter = std::ranges::find_if(m_Internals->ActiveUniformBlocks, [index](const UniformBlockInfo &info) { return info.Index == index; });

		if(iter != m_Internals->ActiveUniformBlocks.end())
			return *iter;

		ASSERT(false, "Shader does not contains uniform at given index, {}", index);
		throw std::runtime_error("Shader does not contains uniform at given index");
	}

	ShaderProgram::UniformInfo ShaderProgram::QueryUniform(const std::string &name) const
	{
		return QueryUniform(GetUniformLocation(name));
	}

	ShaderProgram::UniformBlockInfo ShaderProgram::QueryUniformBlock(const std::string &name) const
	{
		return QueryUniformBlock(GetUniformBlockIndex(name));
	}
}
