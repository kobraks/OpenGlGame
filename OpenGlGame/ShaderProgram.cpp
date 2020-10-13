#include "pch.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include "Assert.h"
#include "Log.h"

namespace Game
{
	static constexpr UniformType TranslateUniformType(uint32_t type)
	{
		switch(type)
		{
			case GL_FLOAT:
				return UniformType::Float;
			case GL_INT:
				return UniformType::Int;
			case GL_FLOAT_VEC2:
				return UniformType::Float2;
			case GL_FLOAT_VEC3:
				return UniformType::Float2;
			case GL_FLOAT_VEC4:
				return UniformType::Float2;
			case GL_FLOAT_MAT3:
				return UniformType::Mat3X3;
			case GL_FLOAT_MAT4:
				return UniformType::Mat4X4;
			default:
				return UniformType::Invalid;
		}
	}

	static void DestroyShaderProgram(ShaderProgram::IdType *id)
	{
		glDeleteProgram(*id);
		delete id;
	}

	static Pointer<ShaderProgram::IdType> CreateShaderProgram()
	{
		auto shader = Pointer<ShaderProgram::IdType>(new ShaderProgram::IdType{}, DestroyShaderProgram);
		*shader     = glCreateProgram();

		return shader;
	}

	ShaderProgram::ShaderProgram(const std::string &name) : m_Name(name)
	{
		GL_LOG_INFO("Creating shader program");
		m_Program = CreateShaderProgram();
		GL_LOG_INFO("New {} shader program id: {}", m_Name, *m_Program);
	}

	ShaderProgram::ShaderProgram(IdType id, const std::string &name) : m_Name(name)
	{
		if(glIsProgram(id))
		{
			ASSERT(false, "Provided id {} do not belong to shader program", id);
			std::runtime_error(fmt::format("Provided id {} do not belong to shader program", id));
		}

		m_Program = std::shared_ptr<IdType>(new IdType(id), [](IdType *id) { glDeleteProgram(*id), delete id; });

		int val = 0;
		glGetProgramiv(id, GL_LINK_STATUS, &val);

		if(val == GL_TRUE)
			m_Linked = true;

		GL_LOG_INFO("Link status of provided shader: {}", m_Linked);

		if(m_Linked)
			Populate();
	}

	ShaderProgram::ShaderProgram(const std::string &name, const Shader &shader) : ShaderProgram(name)
	{
		Attach(shader);

		Link();

		Detach(shader);
	}

	void ShaderProgram::Attach(const Shader &shader)
	{
		GL_LOG_INFO(
		            "Attaching {} shader {} to shader program {} (id: {})",
		            shader.TypeToString(),
		            shader.ID(),
		            m_Name,
		            *m_Program
		           );

		glAttachShader(*m_Program, shader);
	}

	void ShaderProgram::Detach(const Shader &shader)
	{
		GL_LOG_INFO(
		            "Detaching {} shader {} to shader program {} (id: {})",
		            shader.TypeToString(),
		            shader.ID(),
		            m_Name,
		            *m_Program
		           );

		glDetachShader(*m_Program, shader);
	}

	bool ShaderProgram::Link()
	{
		int status = GL_FALSE;
		GL_LOG_INFO("Linking {} (id: {}) shader program", m_Name, *m_Program);

		glLinkProgram(*m_Program);
		glGetProgramiv(*m_Program, GL_LINK_STATUS, &status);

		if(status == GL_FALSE)
		{
			GL_LOG_ERROR("Unable to link {} (id: {}) shader program", m_Name, *m_Program);
			return m_Linked = false;
		}

		GL_LOG_INFO("Sucessful linked {} (id: {}) shader program", m_Name, *m_Program);

		Populate();
		return m_Linked = true;
	}

	void ShaderProgram::Use() const
	{
		if(!m_Linked)
			return;

		glUseProgram(*m_Program);
		Flush();
	}

	void ShaderProgram::Flush() const
	{
		m_Changed = false;

		for(const auto &[name, uniform] : m_Uniforms)
		{
			if(uniform->IsChanged())
				uniform->SendData();
		}
	}

	std::string ShaderProgram::GetLog() const
	{
		int length = 0;

		glGetProgramiv(*m_Program, GL_INFO_LOG_LENGTH, &length);

		if(length > 0)
		{
			std::string log(static_cast<IdType>(length), 0);
			glGetProgramInfoLog(*m_Program, length, &length, &log[0]);
			return log;
		}

		return {};
	}

	int32_t ShaderProgram::GetAttributeLocation(const std::string &name) const
	{
		auto iter    = m_Attributes.find(name);
		int location = -1;

		if(iter != m_Attributes.end())
			location = iter->second;
		else
		{
			location = glGetAttribLocation(*m_Program, name.c_str());
			m_Attributes.emplace(std::make_pair(name, location));
		}

		if(location == -1)
			GL_LOG_WARN(
		            "Shader program {} (id: {}) do not posses an attribute named: {}",
		            m_Name,
		            *m_Program,
		            name.c_str()
		           );

		return location;
	}

	int32_t ShaderProgram::GetUniformLocation(const std::string &name) const
	{
		const auto iter = m_UniformsLocation.find(name);
		if(iter != m_UniformsLocation.end())
		{
			return iter->second;
		}

		return m_UniformsLocation.emplace(name, glGetUniformLocation(*this, name.c_str())).second;
	}

	void ShaderProgram::UniformValue(const std::string &name, const int32_t value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniform1iv(location, 1, &value);
	}

	void ShaderProgram::UniformValue(const std::string &name, const int32_t value, const int32_t value2)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniform2i(location, value, value2);
	}

	void ShaderProgram::UniformValue(
		const std::string &name,
		const int32_t value,
		const int32_t value2,
		const int32_t value3
		)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniform3i(location, value, value2, value3);
	}

	void ShaderProgram::UniformValue(const std::string &name, const uint32_t value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniform1uiv(location, 1, &value);
	}

	void ShaderProgram::UniformValue(const std::string &name, const uint32_t value, const uint32_t value2)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniform2ui(location, value, value2);
	}

	void ShaderProgram::UniformValue(
		const std::string &name,
		const uint32_t value,
		const uint32_t value2,
		const uint32_t value3
		)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniform3ui(location, value, value2, value3);
	}

	void ShaderProgram::UniformValue(const std::string &name, const float value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniform1fv(location, 1, &value);
	}

	void ShaderProgram::UniformValue(const std::string &name, const float value, const float value2)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniform2f(location, value, value2);
	}

	void ShaderProgram::UniformValue(const std::string &name, const float value, const float value2, const float value3)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniform3f(location, value, value2, value3);
	}

	void ShaderProgram::UniformValue(const std::string &name, const glm::vec2 value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniform2fv(location, 1, &value.x);
	}

	void ShaderProgram::UniformValue(const std::string &name, const glm::vec3 value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniform3fv(location, 1, &value.x);
	}

	void ShaderProgram::UniformValue(const std::string &name, const glm::vec4 value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniform4fv(location, 1, &value.x);
	}

	void ShaderProgram::UniformValue(const std::string &name, const glm::mat2x2 value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniformMatrix2fv(location, 1, false, value_ptr(value));
	}

	void ShaderProgram::UniformValue(const std::string &name, const glm::mat2x3 value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniformMatrix2x3fv(location, 1, false, value_ptr(value));
	}

	void ShaderProgram::UniformValue(const std::string &name, const glm::mat2x4 value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniformMatrix2x4fv(location, 1, false, value_ptr(value));
	}

	void ShaderProgram::UniformValue(const std::string &name, const glm::mat3x2 value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniformMatrix3x2fv(location, 1, false, value_ptr(value));
	}

	void ShaderProgram::UniformValue(const std::string &name, const glm::mat3x3 value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniformMatrix3fv(location, 1, false, value_ptr(value));
	}

	void ShaderProgram::UniformValue(const std::string &name, const glm::mat3x4 value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniformMatrix3x4fv(location, 1, false, value_ptr(value));
	}

	void ShaderProgram::UniformValue(const std::string &name, const glm::mat4x2 value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniformMatrix4x2fv(location, 1, false, value_ptr(value));
	}

	void ShaderProgram::UniformValue(const std::string &name, const glm::mat4x3 value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniformMatrix4x3fv(location, 1, false, value_ptr(value));
	}

	void ShaderProgram::UniformValue(const std::string &name, const glm::mat4x4 value)
	{
		const auto location = GetUniformLocation(name);
		if(location != -1)
			glUniformMatrix4fv(location, 1, false, value_ptr(value));
	}

	void ShaderProgram::UniformValue(const std::string &name, const Texture &texture, const int32_t sampleUnit)
	{
		const auto location = GetUniformLocation(name);
		if (location != -1)
		{
			glActiveTexture(GL_TEXTURE0 + sampleUnit);
			texture.Bind();
			
			glUniform1i(location, sampleUnit);
			
		}
	}

	bool ShaderProgram::HasUniform(const std::string &name) const
	{
		return m_Uniforms.find(name) != m_Uniforms.end();
	}

	ShaderProgram* ShaderProgram::GetDefault()
	{
		static ShaderProgram shader(0, DEFAULT_PROGRAM_SHADER_NAME);

		return &shader;
	}

	void ShaderProgram::Populate()
	{
		int count = 0;
		glad_glGetProgramiv(*m_Program, GL_ACTIVE_UNIFORMS, &count);
		GL_LOG_INFO("Shader program {} (id: {}) has {} active uniforms", m_Name, *m_Program, count);

		static constexpr uint32_t BUFFER_SIZE = 524;
		std::array<char, BUFFER_SIZE> buffer{};
		int size      = 0;
		int length    = 0;
		uint32_t type = 0;

		for(auto i = 0; i < count; ++i)
		{
			glGetActiveUniform(*m_Program, i, BUFFER_SIZE, &length, &size, &type, buffer.data());
			std::string uniformName(buffer.data());
			const auto location = glGetUniformLocation(*this, uniformName.c_str());

			m_UniformsLocation.emplace(uniformName, location);

			if(size != 1)
			{
				if(const auto pos = uniformName.find_first_of('['); pos != std::string::npos)
					uniformName = uniformName.substr(0, pos);
			}


			const UniformDetails uniformDetails{
				uniformName,
				TranslateUniformType(type),
				location,
				static_cast<uint32_t>(size)
			};

			GL_LOG_DEBUG("Uniform {}, Name: {} Size: {} Type: {}", i, uniformName, size, type);

			m_Uniforms.emplace(uniformName, std::make_shared<UniformObject>(*this, uniformDetails));
		}
	}

	Pointer<UniformObject> ShaderProgram::GetUniformObject(const std::string &name) const
	{
		auto iter = m_Uniforms.find(name);
		if(iter != m_Uniforms.end())
		{
			m_Changed = true;
			return iter->second;
		}

		GL_LOG_ERROR("Shader program {} (id: {}) has not uniform with name: {}", m_Name, *m_Program, name);

		return nullptr;
	}
}
