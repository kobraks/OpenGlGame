#include "pch.h"
#include "GLCheck.h"

#include <string>
#include <glad/glad.h>
#include <fmt/format.h>

#include "Log.h"

namespace Game::Priv
{
	struct Error
	{
		std::string_view Description;
		std::string_view Name;
	};

	static constexpr Error ErrorString(GLenum errorCode)
	{
		switch(errorCode)
		{
			default: return {"", ""};

			case GL_INVALID_ENUM: return {
					"An unacceptable value has been specified for an enumerated argument.",
					"GL_INVALID_ENUM"
				};
			case GL_INVALID_VALUE: return {"A numeric argument is out of range.", "GL_INVALID_VALUE"};
			case GL_INVALID_OPERATION: return {
					"The specified operation is not allowed in the current state.",
					"GL_INVALID_OPERATION"
				};
			case GL_INVALID_FRAMEBUFFER_OPERATION: return {
					"The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\".",
					"GL_INVALID_FRAMEBUFFER_OPERATION"
				};
			case GL_OUT_OF_MEMORY: return {
					"There is not enough memory left to execute the command.",
					"GL_OUT_OF_MEMORY"
				};
			case GL_STACK_UNDERFLOW: return {"This command would cause a stack underflow.", "GL_STACK_UNDERFLOW"};
			case GL_STACK_OVERFLOW: return {"This command would cause a stack overflow.", "GL_STACK_OVERFLOW"};
		}
	}

	void GLCheckError(const std::string_view &expr, const std::string_view &fileName, const size_t line)
	{
		if (GLenum errorCode = glGetError(); errorCode != GL_NO_ERROR)
		{
			fmt::memory_buffer buffer;
			while(errorCode != GL_NO_ERROR)
			{
				const auto& error = ErrorString(errorCode);
				fmt::format_to(std::back_inserter(buffer), "{}: {}\n", error.Name, error.Description);
				
				errorCode = glGetError();
			}

			OPENGL_LOG_ERROR("{}: {} {}, {}", expr, fileName, line, buffer.data());
			__debugbreak();
			throw std::runtime_error(fmt::format("{}: {} {}, {}", expr, fileName, line, buffer.data()));
		}
	}
}
