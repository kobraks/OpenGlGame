#pragma once
#include "Engine/Core/Base.h"

#include <sol/as_args.hpp>
#include <sol/variadic_args.hpp>
#include <sol/table.hpp>
#include <sol/metatable.hpp>
#include <functional>
#include <filesystem>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Game
{
	sol::table GetMetaTable(const sol::object &obj);
	sol::function GetMetaFunction(const sol::object &obj, sol::meta_function function);

	bool HasMetatable(const sol::object &obj);
	bool TableContains(const sol::table &table, const std::string &key);
	bool HasMetaFunction(const sol::object &obj, sol::meta_function function);

	bool DoFile(sol::state &state, const std::string &file);
	bool DoFile(sol::state &state, const std::string &file, sol::environment &env);

	bool DoString(sol::state &state, const std::string &string);
	bool DoString(sol::state &state, const std::string &string, sol::environment &env);

	std::string ToString(const sol::object &obj);
	std::string ToString(sol::variadic_args::const_iterator begin, const sol::variadic_args::const_iterator &end);
	std::string ToString(const sol::variadic_args &args);

	std::string TableToString(const sol::table &table);

	std::string GetString(lua_State *L, int index);
	void ForEach(lua_State* L, int tableIndex, std::function<void(int, int)> function);

	void PrintLuaStack(lua_State *L);

	glm::vec2 ReadVector2(const sol::table &vector);
	glm::vec3 ReadVector3(const sol::table &vector);
	glm::vec4 ReadVector4(const sol::table &vector);

	constexpr int GetGlobalIndex(lua_State *L, int index)
	{
		return index < 0 ? lua_gettop(L) + index + 1 : index;
	}

	int LuaForEach(lua_State *L);

	inline int Deny(lua_State *L)
	{
		return luaL_error(L, "Access denied");
	}

	template<typename TableType, typename MetaTableType>
	void SetAsMetaTable(TableType&& table, MetaTableType&& metaTable)
	{
		if (!table.is<sol::table>() || !metaTable.is<sol::table>())
			return;

		table[sol::metatable_key] = metaTable;
	}

	template <typename TableType, typename MetaTableType, typename DenyFunctionType>
	void SetAsReadOnlyTable(TableType&& table, MetaTableType&& metaTable, DenyFunctionType denyFunction)
	{
		if (!table.is<sol::table>() || !metaTable.is<sol::table>())
			return;

		metaTable[sol::meta_function::new_index] = denyFunction;
		metaTable[sol::meta_function::index] = metaTable;

		SetAsMetaTable(table, metaTable);
	}

	template <typename TableType, typename MetaTableType>
	void SetAsReadOnlyTable(TableType&& table, MetaTableType&& metaTable)
	{
		SetAsReadOnlyTable(table, metaTable, Deny);
	}
}
