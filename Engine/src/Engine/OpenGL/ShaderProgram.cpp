#include "pch.h"
#include "Engine/OpenGL/ShaderProgram.h"

#include "Engine/OpenGL/Buffer.h"
#include "Engine/OpenGL/Texture.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Log.h"

#include <glm/gtc/type_ptr.hpp>

namespace Game
{
	ShaderProgram::Internals::Internals(std::string name) : Name(std::move(name))
	{
		Program = glCreateProgram();
	}

	ShaderProgram::Internals::Internals(IDType id, std::string name) : Program(id),
	                                                                   Name(std::move(name))
	{
		bool isProgram = false;

		isProgram = glIsProgram(id) == GL_TRUE ? true : false;

		if(!isProgram)
		{
			ASSERT(false, fmt::format("Provided id {} do not belong to shader pgoram", id));
			throw std::runtime_error(fmt::format("Provided id {} do not belong to shader pgoram", id));
		}

		if(Get(ParametersName::LinkStatus) == GL_TRUE)
			Linked = true;

		GL_LOG_INFO("Link status for provided shader: {}", Linked);

		if(Linked)
			Populate();
	}

	ShaderProgram::Internals::~Internals()
	{
		Shaders.clear();

		glDeleteProgram(Program);
	}

	void ShaderProgram::Internals::Attach(Ref<Shader> shader)
	{
		if(Shaders.contains(shader))
		{
			GL_LOG_WARN("Trying to attach attached shader");
			return;
		}

		GL_LOG_INFO(
		            "Attaching {} shader {} to shader program {} (id: {})",
		            shader->TypeToString(),
		            shader->ID(),
		            Name,
		            Program
		           );

		glAttachShader(Program, *shader);
		Shaders.emplace(shader);
	}

	void ShaderProgram::Internals::Detach(Ref<Shader> shader)
	{
		auto &shaders = Shaders;
		auto it       = shaders.find(shader);

		ASSERT(it != shaders.end(), "Detaching not attached shader");

		if(it == shaders.end())
			throw std::runtime_error("Detaching not attached shader");

		GL_LOG_INFO(
		            "Detaching {} shader {} to shader program {} (id: {})",
		            shader->TypeToString(),
		            shader->ID(),
		            Name,
		            Program
		           );

		glDetachShader(Program, *shader);
		shaders.erase(shader);
	}

	bool ShaderProgram::Internals::IsAttached(Ref<Shader> shader) const
	{
		return Shaders.contains(shader);
	}

	bool ShaderProgram::Internals::IsAttached(Shader::Type type) const
	{
		return std::ranges::find_if(
		                            Shaders,
		                            [type](const Ref<Shader> &shader) { return type == shader->GetType(); }
		                           ) != Shaders.end();
	}

	bool ShaderProgram::Internals::Link()
	{
		GL_LOG_INFO("Linking {} (id: {}) shader program", Name, Program);

		glLinkProgram(Program);

		if(Get(ParametersName::LinkStatus) == GL_FALSE)
		{
			GL_LOG_ERROR("Unable to link {} (id: {}) Shader progarm", Name, Program);
			return Linked = false;
		}

		GL_LOG_INFO("Sucessful linked {} (id: {}) shader progarm", Name, Program);

		Populate();
		return Linked = true;
	}

	void ShaderProgram::Internals::Use() const
	{
		if(!Linked)
			return;

		glUseProgram(Program);
	}

	std::string ShaderProgram::Internals::GetLog() const
	{
		int length = Get(ParametersName::InfoLogLength);

		if(length > 0)
		{
			std::string log(length + 1, 0);

			glGetProgramInfoLog(Program, length, &length, &log[0]);
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
		const auto it = Attributes.find(name);
		auto location = INVALID_ATTRIBUTE_LOCATION;

		if(it != Attributes.end())
			location = it->second;
		else
			location = Attributes.emplace(name, glGetAttribLocation(Program, name.c_str())).first->second;

		return location;
	}

	ShaderProgram::UniformLocationType ShaderProgram::Internals::GetUniformLocation(const std::string &name) const
	{
		const auto it = UniformsLocation.find(name);
		auto location = INVALID_UNIFORM_LOCATION;

		if(it != UniformsLocation.end())
			location = it->second;
		else
			location = Attributes.emplace(name, glGetUniformLocation(Program, name.c_str())).first->second;

		return location;
	}

	ShaderProgram::UniformBlockIndexType ShaderProgram::Internals::GetUniformBlockIndex(const std::string &name) const
	{
		const auto it = UniformBlocksIndex.find(name);
		auto index    = INVALID_UNIFORM_BLOCK_INDEX;

		if(it != UniformBlocksIndex.end())
			index = it->second;
		else
			index = Attributes.emplace(name, glGetUniformBlockIndex(Program, name.c_str())).first->second;

		return index;
	}

	int ShaderProgram::Internals::Get(ParametersName name) const
	{
		int value = 0;
		Get(name, &value);

		return value;
	}

	void ShaderProgram::Internals::Get(ParametersName name, int *params) const
	{
		glGetProgramiv(Program, static_cast<GLenum>(name), params);
	}

	void ShaderProgram::Internals::Populate()
	{
		PopulateUniforms();
		PopulateUniformBlocks();
	}

	void ShaderProgram::Internals::PopulateUniformBlocks()
	{
		const uint32_t count = static_cast<uint32_t>(Get(ParametersName::ActiveUniformBlocks));
		GL_LOG_INFO("Shader program {} (id: {}) has {} active uniform blocks", Name, Program, count);

		UniformBlocksIndex.clear();
		ActiveUniformBlocks.clear();

		UniformBlocksIndex.reserve(count);
		ActiveUniformBlocks.reserve(count);

		for(uint32_t i = 0; i < count; ++i)
		{
			const auto info = QueryUniformBlock(i);
			ActiveUniformBlocks.emplace_back(info);
			UniformBlocksIndex.emplace(info.Name, info.Index);

			GL_LOG_TRACE(
			             "UniformBlock: {}, Name: {}, Size: {}, Index: {}, ReferedBy: {}",
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

		ActiveUniforms.clear();
		UniformsLocation.clear();

		ActiveUniforms.reserve(count);
		UniformsLocation.reserve(count);

		for(uint32_t i = 0; i < count; ++i)
		{
			const auto info = QueryUniform(i);

			UniformsLocation.emplace(info.Name, info.Location);
			ActiveUniforms.emplace_back(info);

			GL_LOG_TRACE(
			             "Uniform {}, Name: {}, Size: {}, Type: {}, Location: {}",
			             i,
			             info.Name,
			             info.Size,
			             static_cast<uint32_t>(info.Type),
			             info.Location
			            );
		}
	}

	int ShaderProgram::Internals::GetActiveUniformI(uint32_t index, GLenum pName) const
	{
		int value = 0;
		glGetActiveUniformsiv(Program, 1, &index, pName, &value);
		return value;
	}

	int ShaderProgram::Internals::GetActiveUniformBlockI(uint32_t index, GLenum pName) const
	{
		int value = 0;
		glGetActiveUniformBlockiv(Program, index, pName, &value);
		return value;
	}

	std::string ShaderProgram::Internals::GetActiveUniformBlockName(uint32_t index) const
	{
		const size_t length = static_cast<size_t>(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_NAME_LENGTH));
		std::string name(length + 1, 0);

		glGetActiveUniformBlockName(Program, index, static_cast<GLsizei>(length), nullptr, &name[0]);
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
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER))
			info.ShaderType = Shader::Type::Geometry;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER))
			info.ShaderType = Shader::Type::Fragment;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER))
			info.ShaderType = Shader::Type::Compute;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER))
			info.ShaderType = Shader::Type::Evaluation;
		if(GetActiveUniformBlockI(index, GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER))
			info.ShaderType = Shader::Type::Control;

		return info;
	}

	ShaderProgram::UniformInfo ShaderProgram::Internals::QueryUniform(uint32_t index) const
	{
		UniformInfo info;

		size_t length = static_cast<size_t>(GetActiveUniformI(index, GL_UNIFORM_NAME_LENGTH));
		int size      = 0;
		GLenum type   = 0;

		std::string name(length + 1, 0);

		glGetActiveUniform(Program, index, static_cast<GLsizei>(length), nullptr, &size, &type, &name[0]);
		UniformLocationType location = INVALID_UNIFORM_LOCATION;
		location                     = glGetUniformLocation(Program, name.c_str());

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

	ShaderProgram::ShaderProgram(IDType id, const std::string &name)
	{
		m_Internals = MakePointer<Internals>(id, name);
	}

	ShaderProgram::ShaderProgram(const std::string &name, Ref<Shader> shader) : ShaderProgram(name)
	{
		Attach(shader);
		Link();
		Detach(shader);
	}

	Ref<Shader> ShaderProgram::GetShader(Shader::Type type) const
	{
		return *std::ranges::find_if(
		                             m_Internals->Shaders,
		                             [type](const Ref<Shader> &shader) { return shader->GetType() == type; }
		                            );
	}

	ShaderProgram* ShaderProgram::GetDefault()
	{
		static ShaderProgram program(0, DEFAULT_PROGRAM_SHADER_NAME);

		return &program;
	}

	const std::vector<ShaderProgram::UniformInfo>& ShaderProgram::GetActiveUniforms() const
	{
		return m_Internals->ActiveUniforms;
	}

	ShaderProgram::UniformInfo ShaderProgram::QueryUniform(UniformLocationType location) const
	{
		const auto it = std::ranges::find_if(
		                                     m_Internals->ActiveUniforms,
		                                     [location](const UniformInfo &info) { return info.Location == location; }
		                                    );

		if(it != m_Internals->ActiveUniforms.end())
			return *it;

		ASSERT(false, fmt::format("Shader does not contains uniform at given location, {}", location));
		throw std::runtime_error("Shader does not contains uniform at give location");

		return {};
	}

	ShaderProgram::UniformBlockInfo ShaderProgram::QueryUniformBlock(UniformBlockIndexType index) const
	{
		const auto it = std::ranges::find_if(
		                                     m_Internals->ActiveUniformBlocks,
		                                     [index](const UniformBlockInfo &info) { return info.Index == index; }
		                                    );

		if(it != m_Internals->ActiveUniformBlocks.end())
			return *it;

		ASSERT(false, fmt::format("Shader do not conatain uniform block at given index: {}", index));
		throw std::runtime_error("Shader do not contains uniform block at give index");
	}

	ShaderProgram::UniformInfo ShaderProgram::QueryUniform(const std::string &name) const
	{
		return QueryUniform(GetUniformLocation(name));
	}

	ShaderProgram::UniformBlockInfo ShaderProgram::QueryUniformBlock(const std::string &name) const
	{
		return QueryUniformBlock(GetUniformBlockIndex(name));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniform1i(location, value);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value, int32_t value2)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniform2i(location, value, value2);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value, int32_t value2, int32_t value3)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniform3i(location, value, value2, value3);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniform1ui(location, value);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value, uint32_t value2)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;
		glUniform2ui(location, value, value2);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value, uint32_t value2, uint32_t value3)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniform3ui(location, value, value2, value3);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniform1f(location, value);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value1, float value2)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniform2f(location, value1, value2);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value1, float value2, float value3)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniform3f(location, value1, value2, value3);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec2 &value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniform2fv(location, 1, &value.x);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec3 &value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniform3fv(location, 1, &value.x);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec4 &value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniform4fv(location, 1, &value.x);
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x2 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniformMatrix2fv(location, 1, transpose ? GL_TRUE : GL_FALSE, value_ptr(value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x3 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniformMatrix2x3fv(location, 1, transpose ? GL_TRUE : GL_FALSE, value_ptr(value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x4 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniformMatrix2x4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, value_ptr(value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x2 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniformMatrix3x2fv(location, 1, transpose ? GL_TRUE : GL_FALSE, value_ptr(value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x3 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniformMatrix3fv(location, 1, transpose ? GL_TRUE : GL_FALSE, value_ptr(value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x4 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniformMatrix3x4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, value_ptr(value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x2 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniformMatrix4x2fv(location, 1, transpose ? GL_TRUE : GL_FALSE, value_ptr(value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x3 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniformMatrix4x3fv(location, 1, transpose ? GL_TRUE : GL_FALSE, value_ptr(value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x4 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glUniformMatrix4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, value_ptr(value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const Texture &texture, int32_t sampleUnit)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		glActiveTexture(GL_TEXTURE0 + sampleUnit);
		glUniform1i(location, sampleUnit);
		texture.Bind();
	}

	void ShaderProgram::BindUniformBuffer(UniformBlockIndexType index, const UniformBuffer &buffer)
	{
		if(index == INVALID_UNIFORM_BLOCK_INDEX)
			return;

		buffer.Bind();
		glBindBufferBase(static_cast<GLenum>(buffer.Type()), index, buffer);
	}

	void ShaderProgram::BindUniformBuffer(
		UniformBlockIndexType index,
		const UniformBuffer &buffer,
		size_t size,
		size_t offset
		)
	{
		if(index == INVALID_UNIFORM_BLOCK_INDEX)
			return;

		buffer.Bind();
		glBindBufferRange(static_cast<GLenum>(buffer.Type()), index, buffer, offset, size);
	}
}
