#pragma once
#include <string>

#include <sol/as_args.hpp>
#include <sol/variadic_args.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Game
{
	std::string& TrimLeft(std::string &string);
	std::string& TrimRight(std::string &string);
	std::string& Trim(std::string &string);

	std::string TrimLeftCopy(std::string string);
	std::string TrimRightCopy(std::string string);
	std::string TrimCopy(std::string string);

	std::string& ToLower(std::string &string);
	std::string& ToUpper(std::string &string);

	std::string ToUpperCopy(std::string string);
	std::string ToLowerCopy(std::string string);

	std::string ToString(const sol::state_view &lua, const sol::object &object);
	std::string ToString(const sol::state_view &lua, const sol::table &table, int level = 0);
	std::string ToString(
		const sol::state_view &lua,
		sol::variadic_args::const_iterator begin,
		const sol::variadic_args::const_iterator &end
		);
	std::string ToString(const sol::state_view &lua, const sol::variadic_args &args);

	glm::vec2 ReadVector2(const sol::table &vector);
	glm::vec3 ReadVector3(const sol::table &vector);
	glm::vec4 ReadVector4(const sol::table &vector);
}
