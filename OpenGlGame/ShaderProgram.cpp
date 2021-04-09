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
	                                                                   Name(std::move(name))
	{
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

		GL_CHECK(glDetachShader(Program, *shader));
		shaders.erase(iter);
	}

	bool ShaderProgram::Internals::IsAttached(const Pointer<Shader> shader) const
	{
		return Shaders.contains(shader);
	}

	bool ShaderProgram::Internals::Link()
	{
		GL_LOG_INFO("Linking {} (id: {}) shader program", Name, Program);

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

		GL_CHECK(glUseProgram(Program));
	}

	std::string ShaderProgram::Internals::GetLog() const
	{
		int length = Get(ParametersName::InfoLogLength);

		if(length > 0)
		{
			std::string log(static_cast<IdType>(length), 0);
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
			GL_CHECK(location = glGetAttribLocation(Program, name.c_str()));
			Attributes.emplace(std::make_pair(name, location));
		}

		return location;
	}

	ShaderProgram::UniformLocationType ShaderProgram::Internals::GetUniformLocation(const std::string &name) const
	{
		const auto iter = UniformsLocation.find(name);
		auto location   = INVALID_UNIFORM_LOCATION;
		if(iter != UniformsLocation.end())
			location = iter->second;
		else
			GL_CHECK(location = UniformsLocation.emplace(name, glGetUniformLocation(Program, name.c_str())).first->second);

		return location;
	}

	int ShaderProgram::Internals::Get(ParametersName name) const
	{
		int result = 0;
		Get(name, &result);

		return result;
	}

	void ShaderProgram::Internals::Get(ParametersName name, int *params) const
	{
		GL_CHECK(glGetProgramiv(Program, static_cast<GLenum>(name), params));
	}

	void ShaderProgram::Internals::Populate()
	{
		int count = Get(ParametersName::ActiveUniforms);
		GL_LOG_INFO("Shader program {} (id: {}) has {} active uniforms", Name, Program, count);

		static constexpr uint32_t BUFFER_SIZE = 524;
		std::array<char, BUFFER_SIZE> buffer{};
		int size      = 0;
		int length    = 0;
		uint32_t type = 0;

		for(auto i = 0; i < count; ++i)
		{
			GL_CHECK(glGetActiveUniform(Program, i, BUFFER_SIZE, &length, &size, &type, buffer.data()));
			std::string uniformName(buffer.data());
			GLint location = INVALID_UNIFORM_LOCATION;
			GL_CHECK(location = glGetUniformLocation(Program, uniformName.c_str()));

			UniformsLocation.emplace(uniformName, location);
			ActiveUniforms.emplace_back(uniformName, size, static_cast<UniformType>(type));

			GL_LOG_DEBUG("Uniform {}, Name: {} Size: {} Type: {}", i, uniformName, size, type);
		}
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
		return *std::ranges::find_if(
		                             m_Internals->Shaders,
		                             [type](const Pointer<Shader> &shader) { return shader->GetType() == type; }
		                            );
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

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec2 &value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform2fv(location, 1, &value.x));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec3 &value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform3fv(location, 1, &value.x));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::vec4 &value)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniform4fv(location, 1, &value.x));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x2 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix2fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x3 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix2x3fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat2x4 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix2x4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x2 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix3x2fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x3 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix3fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat3x4 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix3x4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x2 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix4x2fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x3 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix4x3fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const glm::mat4x4 &value, bool transpose)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glUniformMatrix4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)));
	}

	void ShaderProgram::UniformValue(UniformLocationType location, const Texture &texture, int32_t sampleUnit)
	{
		if(location == INVALID_UNIFORM_LOCATION)
			return;

		GL_CHECK(glActiveTexture(GL_TEXTURE0 + sampleUnit));
		GL_CHECK(glUniform1i(location, sampleUnit));
		texture.Bind();
	}

	ShaderProgram* ShaderProgram::GetDefault()
	{
		static ShaderProgram shader(0, DEFAULT_PROGRAM_SHADER_NAME);

		return &shader;
	}

	const std::vector<ShaderProgram::UniformInfo> & ShaderProgram::GetActiveUniforms() const
	{
		return m_Internals->ActiveUniforms;
	}
}
