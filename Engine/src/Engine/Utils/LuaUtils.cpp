#include "pch.h"
#include "LuaUtils.h"

#include <boost/algorithm/string.hpp>

namespace Engine {
	static std::string NormalizeKey(const std::string& string) {
		return boost::to_upper_copy(boost::trim_copy(string));
	}

	static int GetGlobalIndex(lua_State* L, int index)
	{
		return index < 0 ? lua_gettop(L) + index + 1 : index;
	}

	void WarnUnsupportedKey(const std::string& key, const std::string& reason) {
		if (reason.empty()) {
			LOG_SCRIPT_WARN("Unsupported key \"{}\" - ignored", key);
		} else {
			LOG_SCRIPT_WARN("Unsupported key \"{}\" - {}", key, reason);
		}
	}

	std::string LuaGetString(lua_State* L, int index) {
		if (lua_isstring(L, index)) {
			std::string string;
			size_t len;

			const char* str = lua_tolstring(L, index, &len);
			string.append(str, len);
			return string;
		}

		return {};
	}

	void ForEach(lua_State* L, int tableIndex, std::function<void(int, int)> function) {
		const int index = GetGlobalIndex(L, tableIndex);

		luaL_checktype(L, index, LUA_TTABLE);
		lua_pushnil(L);

		while (lua_next(L, index) != 0) {
			lua_pushvalue(L, -2);
			function(-1, -2);
			lua_pop(L, 2);
		}
	}

	int LuaForEach(lua_State* L) {
		if (!lua_istable(L, -2)) {
			LOG_SCRIPT_ERROR("Expected table at index -2, got {}", luaL_typename(L, -2));
			return luaL_error(L, "Expected table at index -2, got %s", luaL_typename(L, -2));
		}

		if (!lua_isfunction(L, -1)) {
			LOG_SCRIPT_ERROR("Expected function at index -1, got {}", luaL_typename(L, -1));
			return luaL_error(L, "Expected function at index -1, got %s", luaL_typename(L, -1));
		}

		const int tableIndex = lua_gettop(L) - 1; // -2 in absolute
		const int funcIndex = lua_gettop(L); // -1 in absolute

		lua_pushvalue(L, tableIndex); //push copy of table 
		lua_pushnil(L); //push initial value

		while (lua_next(L, -2) != 0) {
			// table ... func table key value
			lua_pushvalue(L, funcIndex); //push function
			lua_pushvalue(L, -3); // key
			lua_pushvalue(L, -3); // value

			if (!SafeLuaCall(L, 2, 0, "LuaForEach handler"))
				return luaL_error(L, "LuaForEach Handler execution failed");

			lua_pop(L, 1); //pop value
		}

		lua_pop(L, 1); //pop table copy
		return 0;
	}

	bool SafeLuaCall(lua_State* L, int args, int results, const char* context) {
		const int tracebackIndex = PushStandardLuaErrorHandler(L);
		const int base = lua_gettop(L) - args - 1; // -1 accounts for the pushed handler

		lua_insert(L, base); // place header below function and args

		const int error = lua_pcall(L, args, results, base);
		lua_remove(L, base); //remove handler

		if (error != LUA_OK) {
			const char* message = lua_tostring(L, -1);
			if (context) {
				LOG_SCRIPT_ERROR("[{}] lua error: {}", context, message);
			}
			else {
				LOG_SCRIPT_ERROR("Lua error: {}", message);
			}

			lua_pop(L, 1); //pop error message
			return false;
		}

		return true;
	}

	int PushStandardLuaErrorHandler(lua_State* L) {
		lua_getglobal(L, "debug");
		if (!lua_istable(L, -1)) {
			lua_pop(L, 1); // remove invalid "debug"
			LOG_SCRIPT_WARN("'debug' table not found in Lua state");
			lua_pushcfunction(L, lua_error); //fallback: push no-op error handler
			return lua_gettop(L);
		}

		lua_getfield(L, -1, "traceback"); //+1
		lua_remove(L, -2); //remove "debug"

		if (!lua_isfunction(L, -1)) {
			lua_pop(L, 1);
			LOG_SCRIPT_WARN("'debug.traceback' not found, using lua_error fallback");
			lua_pushcfunction(L, lua_error); //fallback
		}

		return lua_gettop(L); // return absolute index of the header
	}

	template<class T>
	static T ReadVector(const sol::table& table) {
		T result;
		enum Channel { X = 0, Y = 1, Z = 2, W = 3 };
		std::unordered_map<std::string, Channel> keyMap = {
			{"X", X }, {"WIDTH", X}, {"R", X}, {"1", X}, {"RED", X},
			{"Y", Y }, {"HEIGHT", Y}, {"G", Y}, {"2", Y}, {"GREEN", Y},
			{"Z", Z }, {"DEPTH", Z}, {"B", Z}, {"3", Z}, {"BLUE", Z},
			{"W", W }, {"ALPHA", W}, {"A", W}, {"4", W},
		};

		if (table != sol::nil) {
			for (const auto&[k, value] : table) {
				const std::string rawKey = k.is<std::string>() ? k.as<std::string>() : std::to_string(k.as<int>());
				const std::string key = NormalizeKey(rawKey);

				const auto it = keyMap.find(key);
				if (it == keyMap.end()) {
					WarnUnsupportedKey(key);
					continue;
				}

				const float val = value.as<float>();
				switch (const auto channel = it->second) {
				case X:
					result.x = val;
					break;
				case Y:
					result.y = val;
					break;
				case Z:
					if constexpr (std::is_same_v<glm::vec3, T>) {
						result.z = val;
					} else {
						WarnUnsupportedKey(key, fmt::format("not valid in type {}", typeid(T).name()));
					}
					break;
				case W:
					if constexpr (std::is_same_v<glm::vec4, T>) {
						result.w = val;
					} else {
						WarnUnsupportedKey(key, fmt::format("not valid in type {}", typeid(T).name()));
					}
					break;
				default: 
					WarnUnsupportedKey(key);
					break;
				}
			}
		}

		return result;
	}

	glm::vec2 ReadVector2(const sol::table& vector) {
		return ReadVector<glm::vec2>(vector);
	}

	glm::vec3 ReadVector3(const sol::table& vector) {
		return ReadVector<glm::vec3>(vector);
	}

	glm::vec4 ReadVector4(const sol::table& vector) {
		return ReadVector<glm::vec4>(vector);
	}

	void PrintLuaStack(lua_State* L)
	{
		fmt::memory_buffer out;

		int top = lua_gettop(L);
		for (int i = 1; i <= top; ++i)
		{
			fmt::format_to(std::back_inserter(out), "[{}] {:<8} ", i, luaL_typename(L, i));
			switch (lua_type(L, i))
			{
			case LUA_TNUMBER:
				fmt::format_to(std::back_inserter(out), "{:g}\t-- number\n", lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				fmt::format_to(std::back_inserter(out), "\"{}\"\t-- string\n", lua_tostring(L, i));
				break;
			case LUA_TBOOLEAN:
				fmt::format_to(std::back_inserter(out), "{}\t-- boolean\n", lua_toboolean(L, i));
				break;
			case LUA_TNIL:
				fmt::format_to(std::back_inserter(out), "nil\t-- nil\n");
			default:
				fmt::format_to(std::back_inserter(out), "{:p}\t-- {}", lua_topointer(L, i), luaL_typename(L, i));
				break;
			}
		}

		LOG_SCRIPT_DEBUG("Lua Stack\n{}", out.data());
	}
}
