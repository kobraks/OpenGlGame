#include "pch.h"
#include "LuaUtils.h"

#include <vector>

namespace Game
{
	std::string ToString(const sol::state_view &lua, const sol::table &table, std::vector<sol::object> &visited, int level = 0)
	{
		const auto iter = std::find(visited.begin(), visited.end(), static_cast<sol::object>(table));
		if(iter != visited.end())
			return {};
		visited.push_back(static_cast<sol::object>(table));

		if (table.empty())
		{
			sol::table metaTable = table[sol::metatable_key];

			if (!metaTable.empty())
			{
				return ToString(lua, metaTable, visited, level);
			}
		}
		
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

			if(value.get_type() == sol::type::table)
			{
				const auto tableContent = ToString(lua, value.as<sol::table>(), visited, level + 1);
				if(!tableContent.empty())
				{
					result.append(beg);
					result.append(ToString(lua, key));
					result.append(" = ");

					wasTable = true;
					result.append("{\n");
					result.append(tableContent);
					result.append(beg);
					result.append("}");
				}
			}
			else
			{
				result.append(beg);
				result.append(ToString(lua, key));
				result.append(" = ");

				result.append(ToString(lua, value.as<sol::object>()));
				result.append("\n");
			}
		}

		return result;
	}

	std::string ToString(
		const sol::state_view &lua,
		sol::variadic_args::const_iterator begin,
		const sol::variadic_args::const_iterator &end,
		std::vector<sol::object> &visited
		)
	{
		if(begin != end)
		{
			std::string result;

			if(begin->get_type() == sol::type::table)
			{
				result.append("\n");
				result.append(ToString(lua, begin->get<sol::table>(), visited));
			}
			else
				result.append(ToString(lua, begin->get<sol::object>()));

			result.append(ToString(lua, ++begin, end));
			return result;
		}

		return "";
	}

	std::string ToString(const sol::state_view &lua, const sol::variadic_args &args, std::vector<sol::object> &visited)
	{
		std::string result;
		for(auto arg : args)
		{
			if(arg.get_type() == sol::type::table)
			{
				result.append("\n");
				result.append(ToString(lua, arg.get<sol::table>(), visited));
			}
			else
				result.append(ToString(lua, arg.get<sol::object>()));
		}

		return result;
	}

	std::string ToString(const sol::state_view &lua, const sol::object &object)
	{
		return lua["tostring"](object);
	}

	std::string ToString(const sol::state_view &lua, const sol::table &table, int level)
	{
		std::vector<sol::object> visited;

		return ToString(lua, table, visited, level);
	}

	std::string ToString(const sol::state_view &lua, sol::variadic_args::const_iterator begin, const sol::variadic_args::const_iterator &end)
	{
		std::vector<sol::object> visited;

		return ToString(lua, begin, end, visited);
	}

	std::string ToString(const sol::state_view &lua, const sol::variadic_args &args)
	{
		std::vector<sol::object> visited;

		return ToString(lua, args, visited);
	}

	template <class Type>
	static Type ReadVector(const sol::table &table)
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

		return result;
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
