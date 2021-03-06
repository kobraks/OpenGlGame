#include "pch.h"
#include "LuaUtils.h"

#include <vector>
#include "Utils.h"

namespace Game
{
	std::string ToString(const sol::state_view &lua, const sol::table &table, std::vector<sol::object> &visited, int level = 0)
	{
		if(table == sol::nil)
			return ToString(lua, table.as<sol::object>());

		if(!table.valid())
			return {};

		const auto iter = std::find(visited.begin(), visited.end(), static_cast<sol::object>(table));
		if(iter != visited.end())
			return {};
		visited.push_back(static_cast<sol::object>(table));

		if(table.is<sol::metatable>())
		{
			sol::optional<sol::metatable> metaTable = table[sol::metatable_key];

			if(metaTable && !metaTable->empty())
			{
				return ToString(lua, *metaTable, visited, level);
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
		if(!object.valid() && object != sol::nil)
			return {};

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

	bool DoFile(sol::state &state, const std::string &fileName)
	{
		const auto result = state.do_file(fileName);
		if(!result.valid())
		{
			const sol::error error = result;
			LOG_ERROR("Unable to execute script \"{}\", because: {}", fileName, error.what());
			return false;
		}

		LOG_INFO("Sucessfuly executed script \"{}\"", fileName);
	}

	bool DoFile(sol::state &state, const std::string &fileName, sol::environment &env)
	{
		const auto result = state.do_file(fileName, env);
		if(!result.valid())
		{
			const sol::error error = result;
			LOG_ERROR("Unable to execute script \"{}\", because: {}", fileName, error.what());
			return false;
		}

		LOG_INFO("Sucessfuly executed script \"{}\"", fileName);
	}

	bool DoString(sol::state &state, const std::string &string)
	{
		const auto result = state.do_file(string);
		if(!result.valid())
		{
			const sol::error error = result;
			LOG_ERROR("Unable to execute script \"{}\", because: {}", string, error.what());
			return false;
		}

		return true;
	}

	bool DoString(sol::state &state, const std::string &string, sol::environment &env)
	{
		const auto result = state.do_file(string, env);
		if(!result.valid())
		{
			const sol::error error = result;
			LOG_ERROR("Unable to execute script \"{}\", because: {}", string, error.what());
			return false;
		}

		return true;
	}

	std::string LuaGetString(lua_State *L, int index)
	{
		if(lua_isstring(L, index))
		{
			std::string string;
			size_t len;

			const char *str = lua_tolstring(L, 2, &len);
			string.append(str, len);
			return string;
		}

		return {};
	}

	template <class Type>
	static Type ReadVector(const sol::table &table)
	{
		Type result;

		if(table != sol::nil)
		{
			for(const auto &[k, value] : table)
			{
				const std::string key = ToUpperCopy(k.is<std::string>() ? TrimCopy(k.as<std::string>()) : std::to_string(k.as<int>()));

				if(key == "WIDTH" || key == "X" || key == "1" || key == "R" || key == "RED")
					result.x = value.as<float>();

				if(key == "HEIGHT" || key == "Y" || key == "2" || key == "G" || key == "GREEN")
					result.y = value.as<float>();

				if constexpr(!std::is_same_v<glm::vec2, Type>)
				{
					if(key == "DEPTH" || key == "Z" || key == "3" || key == "B" || key == "BLUE")
						result.z = value.as<float>();

					if constexpr(std::is_same_v<glm::vec4, Type>)
					{
						if(key == "W" || key == "4" || key == "A" || key == "ALPHA")
							result.w = value.as<float>();
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
