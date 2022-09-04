#include "pch.h"
#include "Engine/Utils/LuaUtils.h"

#include "Engine/Core/Log.h"
#include "Engine/Utils/Utils.h"

#include <algorithm>
#include <mutex>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>
#include <sol/sol.hpp>

namespace Game
{
	static std::mutex s_Mutex;

	static constexpr std::string_view TypeToStr(sol::type type)
	{
		switch(type)
		{
			case sol::type::none:
				return "none";
			case sol::type::lua_nil:
				return "nil_t";
			case sol::type::string:
				return "string";
			case sol::type::number:
				return "number";
			case sol::type::thread:
				return "thread";
			case sol::type::boolean:
				return "boolean";
			case sol::type::function:
				return "function";
			case sol::type::userdata:
				return "userdata";
			case sol::type::lightuserdata:
				return "lightuserdata";
			case sol::type::table:
				return "table";
			case sol::type::poly:
				return "poly";
			default:
				return "";
		}
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

	static std::string TableToString(const sol::table &table, std::vector<sol::object> &visited, int level = 0)
	{
		if(!table.valid())
			return {};

		if(table == sol::nil)
			return ToString(table);

		//Checking if already processed this table
		const auto iter = std::ranges::find(visited, static_cast<sol::object>(table));
		if(iter != visited.end())
			return {};
		visited.emplace_back(static_cast<sol::object>(table));

		if(auto meta = GetMetaTable(table); meta != sol::nil)
			return TableToString(meta, visited, level);

		const std::string begining(level, '\t');
		bool wasTable = false;
		std::stringstream str;

		for(const auto [k, v] : table)
		{
			std::string key = ToString(k.as<sol::object>());

			if(wasTable)
			{
				wasTable = false;
				str << "\n";
			}

			str << begining << key << " = ";
			if(v.is<sol::table>())
			{
				const auto content = TableToString(v.as<sol::table>(), visited, level + 1);
				if(!content.empty())
				{
					str << "{\n";
					str << content;
					str << begining << "}";
				}
				else
					str << "nil";
			}
			else
				str << ToString(v.as<sol::object>());

			str << "\n";
		}

		return str.str();
	}

	sol::table GetMetaTable(const sol::object &obj)
	{
		if(!obj.valid() || obj == sol::nil || !obj.is<sol::table>())
			return sol::nil;

		const auto meta = obj.as<sol::table>()[sol::metatable_key];

		if(meta.valid() && meta.is<sol::table>())
			return meta;

		return sol::nil;
	}

	sol::function GetMetaFunction(const sol::object &obj, sol::meta_function function)
	{
		const auto meta = GetMetaTable(obj);

		if(meta != sol::nil)
			if(const auto fun = meta[function]; fun.is<sol::function>())
				return fun;

		return sol::nil;
	}

	bool HasMetatable(const sol::object &obj)
	{
		return GetMetaTable(obj) != sol::nil;
	}

	bool TableContains(const sol::table &table, const std::string &key)
	{
		if(!table.valid() || table == sol::nil)
			return false;

		return table[key].valid();
	}

	bool HasMetaFunction(const sol::object &obj, sol::meta_function function)
	{
		return GetMetaFunction(obj, function) != sol::nil;
	}

	bool DoFile(sol::state &state, const std::string &file)
	{
		std::unique_lock guard(s_Mutex);

		const auto result = state.do_file(file);
		if(!result.valid())
		{
			const sol::error error = result;
			LOG_ERROR("Unable to execute script \"{}\", because: {}", file, error.what());
			return false;
		}
		LOG_INFO("Sucessfuly executed script \"{}\"", file);
		return true;
	}

	bool DoFile(sol::state &state, const std::string &file, sol::environment &env)
	{
		std::unique_lock guard(s_Mutex);

		const auto result = state.do_file(file, env);
		if(!result.valid())
		{
			const sol::error error = result;
			LOG_ERROR("Unable to execute script \"{}\", because: {}", file, error.what());
			return false;
		}
		LOG_INFO("Sucessfuly executed script \"{}\"", file);
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

	bool DoString(sol::state &state, const std::string &string, sol::environment &env)
	{
		std::unique_lock guard(s_Mutex);

		const auto result = state.do_string(string, env);
		if(!result.valid())
		{
			const sol::error error = result;
			LOG_ERROR("Unable to execute script \"{}\", because: {}", string, error.what());
			return false;
		}

		return true;
	}

	std::string ToString(const sol::object &obj)
	{
		const auto type = obj.get_type();

		switch(type)
		{
			case sol::type::number:
				return fmt::format("{:g}", obj.as<double>());
			case sol::type::string:
				return obj.as<std::string>();
			case sol::type::boolean:
				return fmt::format("{}", obj.as<bool>());
			case sol::type::nil:
				return "nil";
			case sol::type::table:
				if(auto fun = GetMetaFunction(obj, sol::meta_function::to_string); fun != sol::nil)
					return fun().get<std::string>();
			default:
				return fmt::format("{}: {}", TypeToStr(type), obj.pointer());
		}
	}

	std::string ToString(sol::variadic_args::const_iterator begin, const sol::variadic_args::const_iterator &end)
	{
		std::stringstream str;

		for(; begin != end; ++begin)
		{
			str << ToString(begin->as<sol::object>());
		}

		return str.str();
	}

	std::string ToString(const sol::variadic_args &args)
	{
		return ToString(args.begin(), args.end());
	}

	std::string TableToString(const sol::table &table)
	{
		std::vector<sol::object> visited;

		return TableToString(table, visited);
	}

	std::string GetString(lua_State *L, int index)
	{
		if(lua_isstring(L, index))
		{
			size_t len;

			const auto str = lua_tolstring(L, index, &len);
			return {str, len};
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

	void PrintLuaStack(lua_State *L)
	{
		fmt::memory_buffer out;


		int top = lua_gettop(L);
		for(int i = 1; i <= top; ++i)
		{
			fmt::format_to(std::back_inserter(out), "{}\t{}\t", i, luaL_typename(L, i));
			switch(lua_type(L, i))
			{
				case LUA_TNUMBER:
					fmt::format_to(std::back_inserter(out), "{:g}\n", lua_tonumber(L, i));
					break;
				case LUA_TSTRING:
					fmt::format_to(std::back_inserter(out), "{}\n", lua_tostring(L, i));
					break;
				case LUA_TBOOLEAN:
					fmt::format_to(std::back_inserter(out), "{}\n", lua_toboolean(L, i));
					break;
				case LUA_TNIL:
					fmt::format_to(std::back_inserter(out), "nil\n");
				default:
					fmt::format_to(std::back_inserter(out), "{:p}", lua_topointer(L, i));
					break;
			}
		}

		PRINT_SCRIPT_DEBUG("Lua Stack\n{}", out.data());
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

	int LuaForEach(lua_State *L)
	{
		luaL_checktype(L, 1, LUA_TTABLE);
		luaL_checktype(L, 2, LUA_TFUNCTION);

		lua_pushvalue(L, 1);
		lua_pushnil(L);

		while(lua_next(L, -2) != 0)
		{
			//Coping Values
			lua_pushvalue(L, 2);  //Function
			lua_pushvalue(L, -3); //Key
			lua_pushvalue(L, -3); //Value

			lua_call(L, 2, 0); //Pops from stack Key Value
			lua_pop(L, 1);
		}

		lua_pop(L, 1);

		return 0;
	}
}
