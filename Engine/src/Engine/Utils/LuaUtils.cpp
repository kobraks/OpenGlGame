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
		luaL_checktype(L, -2, LUA_TTABLE);
		luaL_checktype(L, -1, LUA_TFUNCTION);

		lua_pushvalue(L, -2);
		lua_pushnil(L);

		while (lua_next(L, -2) != 0) {
			lua_pushvalue(L, -4);
			lua_pushvalue(L, -3);
			lua_pushvalue(L, -3);

			lua_call(L, 2, 0);
			lua_pop(L, 1);
		}

		lua_pop(L, 1);

		return 0;
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
			fmt::format_to(std::back_inserter(out), "{}\t{}\t", i, luaL_typename(L, i));
			// printf("%d\t%s\t", i, luaL_typename(L, i));
			switch (lua_type(L, i))
			{
			case LUA_TNUMBER:
				fmt::format_to(std::back_inserter(out), "{:g}\n", lua_tonumber(L, i));
				// printf("%g\n", lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				fmt::format_to(std::back_inserter(out), "{}\n", lua_tostring(L, i));
				// printf("%s\n", lua_tostring(L, i));
				break;
			case LUA_TBOOLEAN:
				fmt::format_to(std::back_inserter(out), "{}\n", lua_toboolean(L, i));
				// printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
				break;
			case LUA_TNIL:
				fmt::format_to(std::back_inserter(out), "nil\n");
				// printf("%s\n", "nil");
			default:
				fmt::format_to(std::back_inserter(out), "{:p}", lua_topointer(L, i));
				// printf("%p\n", lua_topointer(L, i));
				break;
			}
		}

		LOG_SCRIPT_DEBUG("Lua Stack\n{}", out.data());
	}
}
