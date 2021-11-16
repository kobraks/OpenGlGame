#pragma once
#include <sol/as_args.hpp>
#include <sol/variadic_args.hpp>
#include <sol/table.hpp>
#include <sol/metatable.hpp>


#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Game
{
	std::string ToString(const sol::object &object);
	std::string ToString(const sol::table &table);
	std::string ToString(sol::variadic_args::const_iterator begin, const sol::variadic_args::const_iterator &end);
	std::string ToString(const sol::variadic_args &args);

	bool DoFile(sol::state &state, const std::string &fileName);
	bool DoFile(sol::state &state, const std::string &fileName, sol::environment &environment);

	bool DoString(sol::state &state, const std::string &string);
	bool DoString(sol::state &state, const std::string &string, sol::environment &environment);

	//Getting string from stack at given index
	std::string LuaGetString(lua_State *L, int index);

	glm::vec2 ReadVector2(const sol::table &vector);
	glm::vec3 ReadVector3(const sol::table &vector);
	glm::vec4 ReadVector4(const sol::table &vector);

	void PrintStack(lua_State *L);

	inline int Deny(lua_State *L)
	{
		return luaL_error(L, "Access denied");
	}

	template <typename TableType, typename MetaTableType, typename DenyFunctionType>
	void SetAsReadOnlyTable(TableType &&table, MetaTableType &&metaTable, DenyFunctionType denyFunction)
	{
		metaTable[sol::meta_function::new_index] = denyFunction;
		metaTable[sol::meta_function::index]     = metaTable;

		table[sol::metatable_key] = metaTable;
	}

	template <typename TableType, typename MetaTableType>
	void SetAsReadOnlyTable(TableType &&table, MetaTableType &&metaTable)
	{
		SetAsReadOnlyTable(table, metaTable, Deny);
	}
}
