#include "pch.h"
#include "Utils.h"

#include <algorithm>

namespace Game
{
	std::string& TrimLeft(std::string &string)
	{
		string.erase(
		             string.begin(),
		             std::find_if(
		                          string.begin(),
		                          string.end(),
		                          [](char c)
		                          {
			                          return !std::isspace(c);
		                          }
		                         )
		            );

		return string;
	}

	std::string& TrimRight(std::string &string)
	{
		string.erase(
		             std::find_if(
		                          string.rbegin(),
		                          string.rend(),
		                          [](char c)
		                          {
			                          return !std::isspace(c);
		                          }
		                         ).base(),
		             string.end()
		            );

		return string;
	}

	std::string& Trim(std::string &string)
	{
		return TrimLeft(TrimRight(string));
	}

	std::string TrimLeftCopy(std::string string)
	{
		return TrimLeft(string);
	}

	std::string TrimRightCopy(std::string string)
	{
		return TrimRight(string);
	}

	std::string TrimCopy(std::string string)
	{
		return Trim(string);
	}

	std::string& ToLower(std::string &string)
	{
		std::transform(string.begin(), string.end(), string.begin(), tolower);
		return string;
	}

	std::string& ToUpper(std::string &string)
	{
		std::transform(string.begin(), string.end(), string.begin(), toupper);
		return string;
	}

	std::string ToUpperCopy(std::string string)
	{
		return ToUpper(string);
	}

	std::string ToLowerCopy(std::string string)
	{
		return ToLower(string);
	}

	std::string ToString(const sol::state_view &lua, const sol::object &object)
	{
		return lua["tostring"](object);
	}

	std::string ToString(const sol::state_view &lua, const sol::table &table, int level)
	{
		const std::string beg(level, '\t');
		auto wasTable = false;
		std::string result;

		for(const auto [key, value] : table)
		{
			if(wasTable)
			{
				wasTable = false;
				result.append("\n");
			}

			result.append(beg);
			result.append(ToString(lua, key));
			result.append(" = ");

			if(value.is<sol::table>())
			{
				wasTable = true;
				result.append("{\n");
				result.append(ToString(lua, value.as<sol::table>(), level + 1));
				result.append(beg);
				result.append("}");
			}
			else
			{
				result.append(ToString(lua, value.as<sol::object>()));
				result.append("\n");
			}
		}

		return result;
	}

	std::string ToString(
		const sol::state_view &lua,
		sol::variadic_args::const_iterator begin,
		const sol::variadic_args::const_iterator &end
		)
	{
		if(begin != end)
		{
			std::string result;

			if(begin->get_type() == sol::type::table)
			{
				result.append("\n");
				result.append(ToString(lua, begin->get<sol::table>()));
			}
			else
				result.append(ToString(lua, begin->get<sol::object>()));

			result.append(ToString(lua, ++begin, end));
			return result;
		}

		return "";
	}

	std::string ToString(const sol::state_view &lua, const sol::variadic_args &args)
	{
		std::string result;
		for(auto arg : args)
		{
			if(arg.get_type() == sol::type::table)
			{
				result.append("\n");
				result.append(ToString(lua, arg.get<sol::table>()));
			}
			else
				result.append(ToString(lua, arg.get<sol::object>()));
		}

		return result;
	}

	template <class Type>
	Type ReadVector(const sol::table &table)
	{
		Type result;

		if(table != sol::nil)
		{
			for(const auto &[k, value] : table)
			{
				const std::string key = k.as<std::string>();

				if(key == "Width" || key == "X" || key == "1")
					result.x = value.as<float>();

				if(key == "Height" || key == "Y" || key == "2")
					result.x = value.as<float>();

				if constexpr(!std::is_same_v<glm::vec2, Type>)
				{
					if(key == "Z" || key == "3")
						result.y = value.as<float>();

					if constexpr(std::is_same_v<glm::vec4, Type>)
					{
						if(key == "W" || key == "4")
							result.y = value.as<float>();
					}
				}
			}
		}
	}

	glm::vec2 ReadVector2(const sol::table &vector)
	{
		return ReadVector<glm::vec2>(vector);
	}

	glm::vec3 ReadVector3(const sol::table &vector)
	{
		return ReadVector<glm::vec3>(vector);
	}

	glm::vec4 ReadVector4(const sol::table &vector)
	{
		return ReadVector<glm::vec4>(vector);
	}
}
