#pragma once
#include "Engine/Core/Base.h"

#include <lua.hpp>
#include <sol/metatable.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Engine {
	static void WarnUnsupportedKey(const std::string& key, const std::string& reason = {});

	//Getting string from stack at given index
	std::string LuaGetString(lua_State* L, int index);

	void ForEach(lua_State* L, int tableIndex, std::function<void(int, int)> function);
	int LuaForEach(lua_State* L);

	glm::vec2 ReadVector2(const sol::table& vector);
	glm::vec3 ReadVector3(const sol::table& vector);
	glm::vec4 ReadVector4(const sol::table& vector);

	void PrintLuaStack(lua_State* L);

	inline int Deny(lua_State* L)
	{
		LOG_SCRIPT_ERROR("Access denied");
		return luaL_error(L, "Access denied");
	}

	template<typename TableType, typename MetatableType, typename DenyFunctionType>
	void SetAsReadOnlyTable(TableType table, MetatableType &metatable, DenyFunctionType denyFunction) {
		metatable[sol::meta_function::new_index] = denyFunction;
		metatable[sol::meta_function::index] = metatable;

		table[sol::metatable_key] = metatable;
	}

	template<typename TableType, typename MetatableType>
	void SetAsReadOnlyTable(TableType& table, MetatableType& metatable) {
		SetAsReadOnlyTable(table, metatable, Deny);
	}
}