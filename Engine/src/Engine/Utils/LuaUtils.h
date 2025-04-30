#pragma once

#include <lua.hpp>
#include <sol/metatable.hpp>

namespace Engine {
	inline int Deny(lua_State* L)
	{
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