#include "pch.h"
#include "LuaUtils.h"

#include <mutex>
#include <vector>

#include "Utils.h"

namespace Game
{
	static std::mutex s_Mutex;

	static int GetGlobalIndex(lua_State *L, int index)
	{
		return index < 0 ? lua_gettop(L) + index + 1 : index;
	}

	static std::string ToString(const sol::table &table, std::vector<sol::object> &visited, int level = 0)
	{
		if(table == sol::nil)
			return ToString(table.as<sol::object>());

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
				return ToString(*metaTable, visited, level);
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
				const auto tableContent = ToString(value.as<sol::table>(), visited, level + 1);
				if(!tableContent.empty())
				{
					result.append(beg);
					result.append(key.as<std::string>());
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
				result.append(key.as<std::string>());
				result.append(" = ");

				result.append(ToString(value.as<sol::object>()));
				result.append("\n");
			}
		}

		return result;
	}

	static std::string ToString(
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
				result.append(ToString(begin->get<sol::table>(), visited));
			}
			else
				result.append(ToString(begin->get<sol::object>()));

			result.append(ToString(++begin, end));
			return result;
		}

		return "";
	}

	static std::string ToString(const sol::variadic_args &args, std::vector<sol::object> &visited)
	{
		std::string result;
		for(auto arg : args)
		{
			if(arg.get_type() == sol::type::table)
			{
				result.append("\n");
				result.append(ToString(arg.get<sol::table>(), visited));
			}
			else
				result.append(ToString(arg.get<sol::object>()));
		}

		return result;
	}

	std::string ToString(const sol::object &object)
	{
		if(!object.valid() && object != sol::nil)
			return {};

		lua_State *L = object.lua_state();
		bool pushed  = false;

		std::string result;
		std::string oldValue;

		if(lua_gettop(L) >= 2 && lua_type(L, -2) == LUA_TTABLE && lua_type(L, -1) == LUA_TSTRING)
		{
			pushed = true;

			oldValue = LuaGetString(L, -1);
			lua_rawget(L, -2);
		}

		switch(lua_type(L, -1))
		{
			case LUA_TNUMBER:
				result = fmt::format("{}", lua_tonumber(L, -1));
				break;
			case LUA_TSTRING:
				result = fmt::format("{}", lua_tostring(L, -1));
				break;
			case LUA_TBOOLEAN:
				result = fmt::format("{}", (lua_toboolean(L, -1) ? "true" : "false"));
				break;
			case LUA_TNIL:
				result = fmt::format("nil");
				break;
			default:
				result = fmt::format("{}: {}", luaL_typename(L, -1), lua_topointer(L, -1));
				break;
		}

		if(pushed)
		{
			lua_pop(L, 1);
			lua_pushstring(L, oldValue.c_str());
		}

		return result;
	}

	std::string ToString(const sol::table &table)
	{
		std::vector<sol::object> visited;
		return ToString(table, visited, 0);
	}

	std::string ToString(const sol::table &table, int level)
	{
		std::vector<sol::object> visited;

		return ToString(table, visited, level);
	}

	std::string ToString(sol::variadic_args::const_iterator begin, const sol::variadic_args::const_iterator &end)
	{
		std::vector<sol::object> visited;

		return ToString(begin, end, visited);
	}

	std::string ToString(const sol::variadic_args &args)
	{
		std::vector<sol::object> visited;

		return ToString(args, visited);
	}

	bool DoFile(sol::state &state, const std::string &fileName)
	{
		std::unique_lock guard(s_Mutex);

		const auto result = state.do_file(fileName);
		if(!result.valid())
		{
			const sol::error error = result;
			LOG_ERROR("Unable to execute script \"{}\", because: {}", fileName, error.what());
			return false;
		}

		LOG_INFO("Sucessfuly executed script \"{}\"", fileName);
		return true;
	}

	bool DoFile(sol::state &state, const std::string &fileName, sol::environment &environment)
	{
		std::unique_lock guard(s_Mutex);

		const auto result = state.do_file(fileName, environment);
		if(!result.valid())
		{
			const sol::error error = result;
			LOG_ERROR("Unable to execute script \"{}\", because: {}", fileName, error.what());
			return false;
		}

		LOG_INFO("Sucessfuly executed script \"{}\"", fileName);
		return true;
	}

	bool DoString(sol::state &state, const std::string &string)
	{
		std::unique_lock guard(s_Mutex);

		const auto result = state.do_string(string);
		if(!result.valid())
		{
			const sol::error error = result;
			LOG_ERROR("Unable to execute script \"{}\", because: {}", string, error.what());
			return false;
		}

		return true;
	}

	bool DoString(sol::state &state, const std::string &string, sol::environment &environment)
	{
		std::unique_lock guard(s_Mutex);

		const auto result = state.do_string(string, environment);
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

			const char *str = lua_tolstring(L, index, &len);
			string.append(str, len);
			return string;
		}

		return {};
	}

	void ForEach(lua_State *L, int tableIndex, std::function<void(int, int)> function)
	{
		const int index = GetGlobalIndex(L, tableIndex);

		luaL_checktype(L, index, LUA_TTABLE);

		lua_pushnil(L);

		while(lua_next(L, index) != 0)
		{
			lua_pushvalue(L, -2);
			function(-1, -2);
			lua_pop(L, 2);
		}
	}

	int LuaForEach(lua_State *L)
	{
		luaL_checktype(L, -2, LUA_TTABLE);
		luaL_checktype(L, -1, LUA_TFUNCTION);

		lua_pushvalue(L, -2);
		lua_pushnil(L);

		while(lua_next(L, -2) != 0)
		{
			lua_pushvalue(L, -4);
			lua_pushvalue(L, -3);
			lua_pushvalue(L, -3);

			lua_call(L, 2, 0);
			lua_pop(L, 1);
		}

		lua_pop(L, 1);

		return 0;
	}

	template <class Type>
	static Type ReadVector(const sol::table &table)
	{
		Type result;

		if(table != sol::nil)
		{
			for(const auto &[k, value] : table)
			{
				const std::string key = ToUpperCopy(
				                                    k.is<std::string>()
					                                    ? TrimCopy(k.as<std::string>())
					                                    : std::to_string(k.as<int>())
				                                   );

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

	void PrintLuaStack(lua_State *L)
	{
		int top = lua_gettop(L);
		for(int i = 1; i <= top; ++i)
		{
			printf("%d\t%s\t", i, luaL_typename(L, i));
			switch(lua_type(L, i))
			{
				case LUA_TNUMBER:
					printf("%g\n", lua_tonumber(L, i));
					break;
				case LUA_TSTRING:
					printf("%s\n", lua_tostring(L, i));
					break;
				case LUA_TBOOLEAN:
					printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
					break;
				case LUA_TNIL:
					printf("%s\n", "nil");
				default:
					printf("%p\n", lua_topointer(L, i));
					break;
			}
		}
	}
}
