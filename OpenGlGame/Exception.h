#pragma once
#include <exception>
#include <fmt/format.h>

namespace Game
{
	class Exception : public std::exception
	{
		std::string m_Message;

	public:
		Exception(const std::string &string) : m_Message(string) {}

		template <class ...Args>
		Exception(const std::string_view &format, Args &&... args) : m_Message(fmt::format(format, std::forward<Args>(args)...)) {}

		~Exception() override = default;

		const char* what() const noexcept override
		{
			return m_Message.c_str();
		}
	};
}
