#include "pch.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include "Assert.h"
#include "Log.h"
#include "GLCheck.h"

namespace Game
{
	static void DestroyShaderProgram(ShaderProgram::IdType *id)
	{
		GL_CHECK(glDeleteProgram(*id));
		delete id;
	}

	static Pointer<ShaderProgram::IdType> CreateShaderProgram()
	{
		auto shader = Pointer<ShaderProgram::IdType>(new ShaderProgram::IdType{}, DestroyShaderProgram);

		GL_CHECK(*shader = glCreateProgram());

		return shader;
	}

	ShaderProgram::Internals::Internals(std::string name) : Name(std::move(name))
	{
		GL_CHECK(Program = glCreateProgram());
	}

	ShaderProgram::Internals::Internals(IdType id, std::string name) : Program(id),
	                                                                   Name(std::move(name)) { }

	ShaderProgram::Internals::~Internals()
	{
		GL_CHECK(glDeleteProgram(Program));
	}

	ShaderProgram::ShaderProgram(const std::string &name)
	{
		GL_LOG_INFO("Creating shader program");
		m_Internals = MakePointer<Internals>(name);
		GL_LOG_INFO("New {} shader program id: {}", m_Internals->Name, m_Internals->Program);
	}

	ShaderProgram::ShaderProgram(IdType id, const std::string &name)
	{
		bool isProgram = false;
		GL_CHECK(isProgram = glIsProgram(id) == GL_TRUE ? true : false);

		if(!isProgram)
		{
			ASSERT(false, "Provided id {} do not belong to shader program", id);
			std::runtime_error(fmt::format("Provided id {} do not belong to shader program", id));
		}

		m_Internals = MakePointer<Internals>(id, name);

		int val = 0;
		GL_CHECK(glGetProgramiv(id, GL_LINK_STATUS, &val));

		if(GetProgramI(ParametersName::LinkStatus) == GL_TRUE)
			m_Internals->Linked = true;

		GL_LOG_INFO("Link status of provided shader: {}", m_Internals->Linked);

		if(m_Internals->Linked)
			Populate();
	}

	ShaderProgram::ShaderProgram(const std::string &name, Pointer<Shader> shader) : ShaderProgram(name)
	{
		Attach(shader);

		Link();

		Detach(shader);
	}

	void ShaderProgram::Attach(Pointer<Shader> shader)
	{
		if(m_Internals->Shaders.contains(shader))
		{
			GL_LOG_WARN("Trying to attach attached shader");
			return;
		}

		GL_LOG_INFO("Attaching {} shader {} to shader program {} (id: {})", shader->TypeToString(), shader->ID(), m_Internals->Name, m_Internals->Program);

		GL_CHECK(glAttachShader(m_Internals->Program, *shader));
		m_Internals->Shaders.emplace(shader);
	}

	void ShaderProgram::Detach(Pointer<Shader> shader)
	{
		auto &shaders = m_Internals->Shaders;
		auto iter     = shaders.find(shader);

		ASSERT(iter != shaders.end(), "Detaching not attached shader");

		if(iter == shaders.end())
			throw std::runtime_error("Detaching not attached shader");

		GL_LOG_INFO("Detaching {} shader {} to shader program {} (id: {})", shader->TypeToString(), shader->ID(), m_Internals->Name, m_Internals->Program);

		GL_CHECK(glDetachShader(m_Internals->Program, *shader));
		shaders.erase(iter);
	}

	bool ShaderProgram::Link()
	{
		GL_LOG_INFO("Linking {} (id: {}) shader program", m_Internals->Name, m_Internals->Program);

		GL_CHECK(glLinkProgram(m_Internals->Program));

		if(GetProgramI(ParametersName::LinkStatus) == GL_FALSE)
		{
			GL_LOG_ERROR("Unable to link {} (id: {}) shader program", m_Internals->Name, m_Internals->Program);
			return m_Internals->Linked = false;
		}

		GL_LOG_INFO("Sucessful linked {} (id: {}) shader program", m_Internals->Name, m_Internals->Program);

		Populate();
		return m_Internals->Linked = true;
	}

	void ShaderProgram::Use() const
	{
		if(!m_Internals->Linked)
			return;

		GL_CHECK(glUseProgram(m_Internals->Program));
	}

	std::string ShaderProgram::GetLog() const
	{
		int length = GetProgramI(ParametersName::InfoLogLength);

		if(length > 0)
		{
			std::string log(static_cast<IdType>(length), 0);
			GL_CHECK(glGetProgramInfoLog(m_Internals->Program, length, &length, &log[0]));
			return log;
		}

		return {};
	}

	ShaderProgram::AttributeLocationType ShaderProgram::GetAttributeLocation(const std::string &name) const
	{
		auto &attributes = m_Internals->Attributes;
		auto iter        = attributes.find(name);
		auto location    = INVALID_ATTRIBUTE_LOCATION;

		if(iter != attributes.end())
			location = iter->second;
		else
		{
			GL_CHECK(location = glGetAttribLocation(m_Internals->Program, name.c_str()));
			attributes.emplace(std::make_pair(name, location));
		}

		if(location == -1)
			GL_LOG_WARN("Shader program {} (id: {}) do not posses an attribute named: {}", m_Internals->Name, m_Internals->Program, name.c_str());

		return location;
	}

	ShaderProgram::UniformLocationType ShaderProgram::GetUniformLocation(const std::string &name) const
	{
		auto &uniformsLocation = m_Internals->UniformsLocation;
		const auto iter        = uniformsLocation.find(name);
		auto location          = INVALID_UNIFORM_LOCATION;
		if(iter != uniformsLocation.end())
			location = iter->second;
		else
			GL_CHECK(location = uniformsLocation.emplace(name, glGetUniformLocation(*this, name.c_str())).first->second);

		if(location == -1)
			GL_LOG_WARN("Uniform \"{}\" not found in shader \"{}\" (id: {})", name, m_Internals->Name, m_Internals->Program);

		return location;
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform1iv(location, 1, &value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value, int32_t value2)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform2i(location, value, value2));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, int32_t value, int32_t value2, int32_t value3)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform3i(location, value, value2, value3));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform1uiv(location, 1, &value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value, uint32_t value2)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform2ui(location, value, value2));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, uint32_t value, uint32_t value2, uint32_t value3)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform3ui(location, value, value2, value3));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform1fv(location, 1, &value));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value, float value2)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform2f(location, value, value2));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, float value, float value2, float value3)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform3f(location, value, value2, value3));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, glm::vec2 value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform2fv(location, 1, &value.x));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, glm::vec3 value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform3fv(location, 1, &value.x));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, glm::vec4 value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform4fv(location, 1, &value.x));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, glm::mat2x2 value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix2fv(location, 1, false, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, glm::mat2x3 value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix2x3fv(location, 1, false, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, glm::mat2x4 value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix2x4fv(location, 1, false, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, glm::mat3x2 value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix3x2fv(location, 1, false, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, glm::mat3x3 value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix3fv(location, 1, false, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, glm::mat3x4 value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix3x4fv(location, 1, false, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, glm::mat4x2 value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix4x2fv(location, 1, false, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, glm::mat4x3 value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix4x3fv(location, 1, false, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, glm::mat4x4 value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix4fv(location, 1, false, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const Texture &texture, int32_t sampleUnit)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glActiveTexture(GL_TEXTURE0 + sampleUnit));
		texture.Bind();
		GL_CHECK(glUniform1i(location, sampleUnit));
	}

	bool ShaderProgram::HasUniform(const std::string &name) const
	{
		return GetUniformLocation(name) != INVALID_UNIFORM_LOCATION;
	}

	ShaderProgram* ShaderProgram::GetDefault()
	{
		static ShaderProgram shader(0, DEFAULT_PROGRAM_SHADER_NAME);

		return &shader;
	}

	void ShaderProgram::Populate()
	{
		int count = GetProgramI(ParametersName::ActiveUniforms);
		GL_LOG_INFO("Shader program {} (id: {}) has {} active uniforms", m_Internals->Name, m_Internals->Program, count);

		static constexpr uint32_t BUFFER_SIZE = 524;
		std::array<char, BUFFER_SIZE> buffer{};
		int size      = 0;
		int length    = 0;
		uint32_t type = 0;

		auto &program          = m_Internals->Program;
		auto &uniformsLocation = m_Internals->UniformsLocation;

		for(auto i = 0; i < count; ++i)
		{
			GL_CHECK(glGetActiveUniform(program, i, BUFFER_SIZE, &length, &size, &type, buffer.data()));
			std::string uniformName(buffer.data());
			GLint location = INVALID_UNIFORM_LOCATION;
			GL_CHECK(location = glGetUniformLocation(*this, uniformName.c_str()));

			uniformsLocation.emplace(uniformName, location);

			GL_LOG_DEBUG("Uniform {}, Name: {} Size: {} Type: {}", i, uniformName, size, type);
		}
	}

	int ShaderProgram::GetProgramI(ParametersName name) const
	{
		int result = 0;
		GetProgramIV(name, &result);

		return result;
	}

	void ShaderProgram::GetProgramIV(ParametersName name, int *params) const
	{
		GL_CHECK(glGetProgramiv(m_Internals->Program, static_cast<GLenum>(name), params));
	}
}
